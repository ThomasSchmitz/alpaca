# MonkSynth VST3

## Testing & Quality Assurance Specification

This document defines the complete testing strategy for MonkSynth,
including DSP validation, automation stress testing, MIDI validation,
performance profiling, state integrity checks, and release criteria.

The goal is to ensure the plugin is: - Sonically correct - Stable under
extreme conditions - Real-time safe - Deterministic across sample
rates - Host-compatible

------------------------------------------------------------------------

# 1. Testing Philosophy

MonkSynth must be tested in three layers:

1.  DSP Unit-Level Validation
2.  Integration & Automation Stress Tests
3.  Host & Real-World Behavioral Tests

Testing must include both automated (offline harness) and manual
validation passes.

------------------------------------------------------------------------

# 2. Offline DSP Render Harness

Create a standalone console test target (not plugin) that links DSP
modules directly.

File: /tests/RenderHarness.cpp

Purpose: - Render deterministic audio to buffers - Compare against
reference outputs - Analyze spectra and envelopes - Measure performance
metrics

------------------------------------------------------------------------

# 3. DSP Unit Tests

## 3.1 Excitation Stability Test

Test: - Generate 10 seconds at 44.1kHz and 96kHz - Sweep pitch from 50
Hz to 2 kHz - Confirm: - No NaNs - No infinities - No denormal stalls -
No alias bursts beyond expected behavior

Acceptance: - RMS within expected range - No sample \> \|1.5\| unless
intentionally saturated

------------------------------------------------------------------------

## 3.2 Formant Bank Verification

Test Cases: - Fixed vowel positions (0.0, 0.25, 0.5, 0.75, 1.0) -
Measure FFT peaks

Acceptance: - Major spectral peaks align approximately with target
formant frequencies - Peak positions scale correctly with
`formant_shift_semitones` - Interpolation between vowels produces
continuous movement (no jumps)

------------------------------------------------------------------------

## 3.3 Glide Test

Test: - Rapid note changes with glide enabled - Log pitch trajectory

Acceptance: - Exponential or linear ramp matches expected curve - No
discontinuous pitch jumps - No audible clicks

------------------------------------------------------------------------

## 3.4 Vibrato Test

Test: - Fixed pitch with vibrato depth 0.5 - Measure instantaneous
frequency

Acceptance: - Modulation depth matches expected semitone depth ±
tolerance - LFO frequency within 1% of target

------------------------------------------------------------------------

## 3.5 Delay Stability Test

Test: - Feedback set to 0.95 - Impulse input

Acceptance: - Signal decays - No infinite growth - No instability at any
delay time

------------------------------------------------------------------------

## 3.6 Reverb Stability Test

Test: - Impulse response - Long decay (15--20s)

Acceptance: - Energy decays smoothly - No runaway oscillation - No
denormals after decay

------------------------------------------------------------------------

# 4. Sample Rate Matrix Testing

Test at minimum:

-   44.1 kHz
-   48 kHz
-   88.2 kHz
-   96 kHz

If feasible: - 192 kHz

Acceptance: - Identical musical behavior - No parameter scaling errors -
No delay/reverb timing drift

------------------------------------------------------------------------

# 5. Automation Stress Tests

## 5.1 Rapid Automation Sweep

Automate: - Vowel - Delay time - Reverb decay - Formant shift

Sweep 0 → 1 repeatedly at high speed.

Acceptance: - No zippering beyond smoothing tolerance - No CPU spikes -
No audio dropouts

------------------------------------------------------------------------

## 5.2 Automation + MIDI Conflict

Simultaneously: - Automate `vowel` - Send MIDI CC controlling `vowel`

Acceptance: - Behavior matches defined policy (Automation Wins or
Last-Touched) - No flicker or parameter instability

------------------------------------------------------------------------

# 6. MIDI System Tests

## 6.1 CC Mapping

Test: - Map CC to every parameter - Move controller rapidly

Acceptance: - Correct range mapping - Soft takeover works - Invert
works - No stepping when smoothing enabled

------------------------------------------------------------------------

## 6.2 NRPN High-Resolution Sweep

Test: - Sweep 14-bit NRPN across full range - Target `vowel` and
`delay_time_ms`

Acceptance: - Smooth transitions - No 7-bit stepping artifacts

------------------------------------------------------------------------

## 6.3 State Persistence

Test: - Create 10 custom mappings - Save project - Reload DAW

Acceptance: - All mappings restored correctly - Behavior identical

------------------------------------------------------------------------

# 7. UI Behavioral Tests

## 7.1 Scaling Test

Test: - UI scale 0.75, 1.0, 1.5

Acceptance: - No overlapping controls - Text readable - No pixel
misalignment

------------------------------------------------------------------------

## 7.2 MIDI Learn UX

Test: - Enter learn mode - Map control - Cancel learn - Clear mapping

Acceptance: - Correct highlight state - Overlay appears/disappears
properly - No UI freeze

------------------------------------------------------------------------

# 8. Performance Profiling

## 8.1 CPU Baseline

Measure CPU usage at: - Idle (no notes) - Single sustained note - Rapid
MIDI input - Max FX settings

Target: - \< 5% on modern laptop at 44.1kHz (single instance)

------------------------------------------------------------------------

## 8.2 Automation Worst Case

Automate 8 parameters simultaneously.

Acceptance: - No dropouts - CPU stable

------------------------------------------------------------------------

# 9. Memory & Allocation Audit

## 9.1 Allocation Detection

In debug: - Instrument allocator or use tooling (e.g., Xcode Instruments
/ Visual Studio profiler) - Confirm no allocations during `processBlock`

Acceptance: - Zero heap allocations in audio callback

------------------------------------------------------------------------

# 10. Denormal Test

Procedure: - Feed silence for 30 seconds after reverb tail - Monitor CPU

Acceptance: - No CPU spike - No creeping performance degradation

------------------------------------------------------------------------

# 11. Host Compatibility Matrix

Test minimum:

-   Ableton Live
-   Logic Pro
-   Reaper
-   Cubase

For each host: - Load plugin - Save project - Reload - Automate
parameters - MIDI learn - Change sample rate during session

Acceptance: - No crashes - State preserved - Automation visible and
functional

------------------------------------------------------------------------

# 12. Preset Validation

Test: - Save preset - Load preset - Compare parameter values

Acceptance: - Bit-identical parameter restoration

------------------------------------------------------------------------

# 13. Regression Testing Strategy

For each major milestone:

1.  Render reference audio file (golden file)
2.  Store in /tests/GoldenFiles
3.  After changes, re-render
4.  Compare:
    -   RMS
    -   Peak
    -   Optional spectral fingerprint

Small tolerance allowed due to floating-point drift.

------------------------------------------------------------------------

# 14. Release Criteria

Plugin is ready for release when:

-   All DSP tests pass
-   No RT violations found
-   CPU within target
-   No crashes across test hosts
-   No memory leaks
-   No UI scaling defects
-   State save/load verified repeatedly
-   Manual musical play test confirms expressiveness and stability

------------------------------------------------------------------------

# 15. Beta Test Checklist

Before public release:

-   5--10 external beta testers
-   Different OS versions
-   Different buffer sizes (64--1024 samples)
-   At least 3 DAWs per platform

Collect: - CPU reports - Crash logs - MIDI controller compatibility
feedback

------------------------------------------------------------------------

End of Testing & QA Specification
