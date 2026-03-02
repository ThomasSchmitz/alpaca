# MonkSynth VST3

## Audio Thread Real-Time Safety & Concurrency Specification

This document defines **hard real-time constraints** and the
**concurrency design** for MonkSynth. It exists to prevent glitches,
deadlocks, denormal stalls, and state/automation/MIDI edge-case bugs.

Non-negotiable rule: - `processBlock()` must be **real-time safe** on
all supported hosts.

------------------------------------------------------------------------

# 1. Definitions

## 1.1 Threads (JUCE context)

-   **Audio thread**: executes `processBlock()` (or equivalent RT
    callback).
-   **Message thread**: UI, parameter edits, state loading, timers.
-   **Host threads**: may call plugin methods unpredictably (some hosts
    invoke state or parameter APIs from non-message threads). Assume
    hostile scheduling.

## 1.2 RT-Safe

RT-safe means: - No unbounded latency operations - No locks that can
block - No heap allocations - No file/network IO - No OS calls that can
sleep

------------------------------------------------------------------------

# 2. Absolute Prohibitions in Audio Thread

Inside `processBlock()` (and any function it calls), **do not**:

1.  Allocate or free memory
    -   no `new/delete`
    -   no `malloc/free`
    -   no `std::vector::push_back` growth
    -   no `std::string` concatenation
    -   no `std::function` captures that may allocate
2.  Use any blocking synchronization
    -   no `std::mutex`, `juce::CriticalSection`, `ScopedLock`
    -   no condition variables
    -   no `MessageManagerLock`
3.  Perform IO
    -   no file reads/writes
    -   no logging to disk/console
    -   no network
4.  Call UI code
    -   no repaint calls
    -   no component access
5.  Use unpredictable system calls
    -   sleep, wait, timers
6.  Throw exceptions or rely on RTTI-heavy calls
    -   compile with exceptions disabled if feasible (project choice)
    -   never throw from DSP

------------------------------------------------------------------------

# 3. Memory Model & Allocation Strategy

## 3.1 Preallocation

Allocate all DSP buffers in: - `prepareToPlay()` - `releaseResources()`
/ destructor - or a dedicated `prepare(spec)` call for each module

Examples of preallocated memory: - Delay buffers (L/R) - Reverb delay
lines / FDN matrices - Ensemble delay lines - Temporary scratch buffers
(if needed) - MIDI decoding buffers (fixed capacity)

## 3.2 Fixed-Capacity Containers

For audio thread data structures: - Use fixed arrays (`std::array`) - Or
custom ring buffers with fixed capacity - If using `std::vector`,
reserve once at prepare-time and never exceed capacity

------------------------------------------------------------------------

# 4. Parameter Access & Automation

## 4.1 Single Read Per Block (Required)

Audio thread must not call `apvts.getRawParameterValue()` per sample.
Instead: - Read parameters once per block - Construct `ParamSnapshot`
(POD struct) - Pass snapshot to DSP modules

## 4.2 Atomic Read Pattern

In JUCE, `getRawParameterValue()` returns `std::atomic<float>*`. Safe
pattern at block start: -
`float value = paramAtomic->load(std::memory_order_relaxed);`

Then never touch the atomic again until the next block.

## 4.3 Smoothing Location

All smoothing happens in DSP modules, consuming values from
`ParamSnapshot`. - Smoothers should be stateful per module. - Update
smoother targets at block start (or control rate). - Never recompute
expensive coefficients per sample unless unavoidable.

## 4.4 Detecting "Automation Active" (for policy)

If implementing "automation wins" policy: - Maintain last block's base
parameter value - If abs(current - last) \> epsilon, mark parameter as
host-touched this block - Epsilon should be small but \> float noise,
e.g. 1e-6 for normalized params

Avoid: - any host callback dependence inside audio thread

------------------------------------------------------------------------

# 5. MIDI Handling Concurrency

## 5.1 MIDI Input Parsing

Parsing incoming `juce::MidiBuffer` occurs in audio thread (RT-safe).

Rules: - No dynamic allocations during parsing - Use stack/local fixed
structs - For NRPN decoding, maintain a small per-channel state machine
(fixed storage)

## 5.2 MIDI Learn Updates

MIDI learn edits come from the message thread (UI actions). Audio thread
must not lock to read those edits.

Required approach: - UI builds an **immutable mapping snapshot** - Swap
it atomically into the audio thread

------------------------------------------------------------------------

# 6. Immutable Snapshot Swapping (Key Pattern)

## 6.1 Why

Mapping tables and more complex state (e.g., vowel tables if
user-editable) cannot be safely mutated while audio thread reads them.

## 6.2 Snapshot Design

Create: - `struct MidiMapSnapshot { ... fixed mapping arrays ... };` -
Store as `std::shared_ptr<const MidiMapSnapshot>`

Audio thread owns: -
`std::atomic<std::shared_ptr<const MidiMapSnapshot>> midiMapPtr;`

Message thread update: 1. Build new snapshot (may allocate) 2.
`midiMapPtr.store(newSnapshot, std::memory_order_release);`

Audio thread read (once per block): -
`auto map = midiMapPtr.load(std::memory_order_acquire);`

Then treat it as read-only for the whole block.

Notes: - `shared_ptr` atomic operations are supported in C++20; in C++17
you can use `std::atomic_load` / `std::atomic_store` free functions or a
lock-free custom pointer swap. Choose one consistent technique.

Minimum viable alternative: - double-buffer snapshots + atomic index (no
shared_ptr), if you want zero refcount overhead.

------------------------------------------------------------------------

# 7. Telemetry Pipe (Audio → UI)

## 7.1 Requirements

UI needs low-rate meters and state: - RMS/peak - current pitch - current
vowel - gate state

Audio thread must not call UI directly.

## 7.2 Recommended Method: Atomics

Define `UiTelemetry` as a set of atomics: - `std::atomic<float> rms;` -
`std::atomic<float> pitchHz;` - `std::atomic<float> vowel01;` -
`std::atomic<int> gate;`

Audio thread updates once per block: - store with `memory_order_relaxed`

UI reads on a timer (30--60 Hz): - load relaxed

No locks required.

## 7.3 Meter Smoothing

Perform meter smoothing in UI (message thread) to keep audio thread
minimal.

------------------------------------------------------------------------

# 8. State Save/Load Concurrency

## 8.1 getStateInformation()

This is generally called on a non-audio thread, but hosts vary. Rules: -
Ensure it does not touch audio thread-only buffers - Serialize only
APVTS state + mapping state (already immutable or message-thread owned)

## 8.2 setStateInformation()

Typically called on message thread, but assume host may call
unexpectedly.

Required safe pattern: - Parse the incoming state into a
`juce::ValueTree` on the caller thread - Apply to APVTS using
JUCE-approved calls - Rebuild mapping snapshot on message thread if
possible

If called while audio is running: - Atomically swap new mapping
snapshot - DSP modules should handle parameter jumps gracefully via
smoothing

------------------------------------------------------------------------

# 9. Lock-Free Data Structures & Atomics

## 9.1 Atomics Guidance

-   Use `memory_order_relaxed` for simple telemetry and parameter reads.
-   Use acquire/release semantics for pointer/snapshot swaps.

## 9.2 Avoid False Sharing

If you store multiple atomics, consider padding or grouping to avoid
cache-line contention if needed (only if profiling indicates).

------------------------------------------------------------------------

# 10. Denormals & Floating-Point Safety

## 10.1 Denormal Avoidance

In audio thread: - Enable flush-to-zero / denormals-are-zero where
supported - Or apply tiny DC offset or clamp small values

JUCE helper: - `juce::ScopedNoDenormals noDenormals;` inside
`processBlock`

## 10.2 Feedback Stability

-   Clamp delay feedback \<= 0.95
-   Clamp reverb internal feedback to stable range
-   Avoid Q factors that explode for filter formants

------------------------------------------------------------------------

# 11. Coefficient Updates and Control Rate

## 11.1 Control Rate Updates

Expensive ops should run at: - once per block, or - every N samples
(e.g., 16/32/64), depending on audible zippering

Examples: - Biquad coefficient updates for formants - Reverb damping
coefficients - Delay time interpolation parameters

## 11.2 Delay Time Changes

Avoid immediate buffer index jumps: - Use fractional delay with
interpolation OR - Slew delay time slowly and crossfade taps

Minimum v1 approach: - smooth delay time parameter (50 ms) + linear
interpolation read

------------------------------------------------------------------------

# 12. Reentrancy and Host Behavior

Hosts may call: - `setParameter` rapidly during automation -
`setStateInformation` while audio is active - `releaseResources` after
`prepareToPlay` unexpectedly

Defensive rules: - DSP modules must tolerate `reset()` at any time -
Treat all external calls as potentially concurrent with audio

------------------------------------------------------------------------

# 13. Instrumentation & Enforcement

## 13.1 Debug Build Guards

Add optional compile-time flags: - `MONKSYNTH_RT_ASSERTS=1`

In debug builds: - Track allocations via custom allocator hooks
(optional) - Track mutex usage (code review + lint) - Add assertions
ensuring buffer capacities not exceeded

## 13.2 Code Review Checklist

-   No heap alloc in audio path
-   No locks in audio path
-   Parameter reads once per block
-   Snapshots swapped atomically
-   UI telemetry via atomics only
-   Denormals handled
-   Feedback clamped

------------------------------------------------------------------------

# 14. Acceptance Criteria

A build passes this spec when: - Audio remains glitch-free under heavy
automation and MIDI input - No deadlocks when toggling MIDI learn or
loading state - CPU usage stable; no spikes when UI open/closed - No
allocations detected in `processBlock` (instrumented test or audit) -
Plugin survives repeated save/load cycles while playing

------------------------------------------------------------------------

End of Real-Time Safety & Concurrency Specification
