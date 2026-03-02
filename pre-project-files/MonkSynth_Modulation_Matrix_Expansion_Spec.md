# MonkSynth VST3

## Modulation Matrix Expansion Specification (v1.5)

This document defines the complete architecture, DSP behavior, UI
structure, real-time safety rules, and testing requirements for the
MonkSynth v1.5 Modulation Matrix expansion.

Purpose: Transform MonkSynth from a performance-focused vocal instrument
into a modern expressive sound design tool while preserving real-time
safety and stability.

------------------------------------------------------------------------

# 1. Design Goals

## 1.1 Primary Goals

-   Add flexible internal modulation routing.
-   Support many-to-many routing (sources → targets).
-   Maintain strict real-time safety.
-   Avoid rewriting or overwriting base parameters.
-   Keep CPU overhead under 2% per instance (target).

## 1.2 Non-Goals (v1.5)

-   Full modular synth routing.
-   Unlimited slots.
-   Audio-rate modulation of every parameter.
-   Complex scripting environment.

------------------------------------------------------------------------

# 2. High-Level Architecture

Existing v1 architecture:

Base Parameter → ParamSnapshot → DSP

v1.5 architecture:

Base Parameter ↓ Mod Matrix (accumulate modulation) ↓ Final
ParamSnapshot ↓ DSP

All modulation is applied **after parameter smoothing targets are
updated** but before DSP processing begins.

------------------------------------------------------------------------

# 3. Slot Configuration

## 3.1 Slot Count

Recommended: - Minimum: 4 slots - Ideal: 8 slots - Hard max: 8 (fixed
compile-time constant)

``` cpp
constexpr int MAX_MOD_SLOTS = 8;
```

------------------------------------------------------------------------

## 3.2 Slot Structure

Each slot stores:

``` cpp
struct ModSlot {
    bool enabled;
    ModSource source;
    int targetParamIndex;
    float depth;           // -1.0 to +1.0
    bool bipolar;          // true = -1..+1, false = 0..1
};
```

All fields must be POD and RT-safe.

------------------------------------------------------------------------

# 4. Modulation Sources

## 4.1 Source Enum

``` cpp
enum class ModSource {
    LFO1,
    LFO2,
    EnvFollower,
    RandomSlow,
    KeyTrack,
    Velocity,
    ModWheel,
    Aftertouch,
    ChannelPressure,
    PitchBend,
    Macro1,
    Macro2
};
```

Keep enum stable for preset compatibility.

------------------------------------------------------------------------

## 4.2 Source Definitions

### LFO1 / LFO2

Parameters: - Rate - Depth (global multiplier) - Waveform (sine,
triangle, random) - Sync (optional)

Implementation: - Run per-sample or per-block with interpolation. - Must
be extremely lightweight.

------------------------------------------------------------------------

### Envelope Follower

Based on output signal RMS or absolute value smoothing.

Block-based acceptable.

------------------------------------------------------------------------

### RandomSlow

Low-frequency random walk or sample-and-hold updated every N ms.

------------------------------------------------------------------------

### KeyTrack

Normalized pitch tracking: 0 = lowest MIDI note supported 1 = highest
MIDI note supported

------------------------------------------------------------------------

### Velocity

Note-on velocity normalized 0..1.

------------------------------------------------------------------------

### MIDI Sources

Use decoded values from MidiRouter. These are already normalized 0..1 or
-1..+1.

------------------------------------------------------------------------

# 5. Modulation Targets

Targets are parameter indices (integer lookup into ParamSnapshot).

Valid targets:

Performance: - vowel - voice_character - formant_shift_semitones -
tilt - vibrato_depth

FX: - delay_time_ms - delay_feedback - delay_wet - reverb_size -
reverb_decay_s - reverb_wet - ensemble_amount

Do NOT allow modulation of: - mono_priority - mapping mode - MIDI
channel - UI-only settings

------------------------------------------------------------------------

# 6. Modulation Processing Model

## 6.1 Per-Block Accumulation

At start of processBlock():

1.  Build base ParamSnapshot.
2.  Clear modulation buffer:

``` cpp
float paramMod[MAX_PARAMS] = {0};
```

3.  For each enabled slot:

``` cpp
float sourceVal = getSourceValue(slot.source);
float modVal = sourceVal * slot.depth;
paramMod[slot.targetParamIndex] += modVal;
```

4.  Apply to base:

``` cpp
finalValue = clamp(baseValue + paramMod[i], min, max);
```

5.  Feed final snapshot to DSP.

------------------------------------------------------------------------

# 7. Performance Constraints

-   No dynamic memory in audio thread.
-   No string lookups in audio thread.
-   Pre-resolve targetParamIndex when slot is created.
-   O(slots) per block cost only.
-   No nested loops across all parameters.

------------------------------------------------------------------------

# 8. Snapshot Swapping (Concurrency)

Mod matrix configuration changes happen on message thread.

Procedure:

1.  UI builds new immutable ModMatrixSnapshot.
2.  Atomically swap pointer into audio thread.
3.  Audio thread reads snapshot once per block.

Use same pattern as MIDI mapping snapshot.

------------------------------------------------------------------------

# 9. LFO Implementation Details

## 9.1 Phase Accumulation

``` cpp
phase += rate / sampleRate;
if (phase >= 1.0f) phase -= 1.0f;
```

Waveform generation must avoid expensive trig calls where possible.
Precompute sine table OR use std::sin if CPU budget allows.

------------------------------------------------------------------------

## 9.2 Sync Mode

If sync enabled: - Rate derived from BPM and division. - BPM retrieved
from host at block start.

------------------------------------------------------------------------

# 10. UI Specification

Add collapsible "Modulation" panel.

Each slot row contains:

-   Enable toggle
-   Source dropdown
-   Target dropdown
-   Depth knob (bipolar)
-   Optional small polarity toggle icon

Compact layout required (no grid matrix).

------------------------------------------------------------------------

# 11. Interaction with MIDI Mapping

MIDI mapping: - External → parameter (Set or Modulate mode)

Mod matrix: - Internal → parameter (always additive)

Order of operations:

Base parameter (host) → MIDI set (if in Set mode) → Mod matrix
accumulation → DSP

In Modulate mapping mode: - MIDI mapping feeds into mod source instead
of overwriting base.

------------------------------------------------------------------------

# 12. Real-Time Safety

Strictly follow:

-   No locks
-   No allocations
-   No vector resizing
-   No pointer invalidation mid-block

ModMatrixSnapshot must be immutable and atomically swapped.

------------------------------------------------------------------------

# 13. Testing Requirements

## 13.1 Basic Routing Test

-   Route LFO1 → vowel
-   Confirm modulation depth correct

## 13.2 Multi-Slot Test

-   4 slots targeting same param
-   Confirm accumulation without clipping

## 13.3 Automation + Modulation

-   Automate base parameter
-   Apply LFO
-   Confirm base moves while modulation remains additive

## 13.4 Performance Test

-   8 slots active
-   All LFOs running
-   CPU overhead \< 2%

------------------------------------------------------------------------

# 14. Preset Serialization

Mod matrix must serialize: - Slot enable state - Source enum - Target
index (store param ID string for safety) - Depth - Bipolar flag

State stored under:

mod_matrix slot_0 slot_1 ...

------------------------------------------------------------------------

# 15. Acceptance Criteria

v1.5 modulation matrix is complete when:

-   Up to 8 slots functional
-   No RT violations
-   Stable under heavy automation
-   No parameter jumps
-   Presets recall correctly
-   CPU impact minimal
-   No host crashes

------------------------------------------------------------------------

End of Modulation Matrix Expansion Specification
