# MonkSynth VST3

## JUCE Architecture Blueprint

This document specifies the **C++/JUCE architecture** for a monophonic
formant-vocal VST3 instrument with modern FX and full MIDI mappability.

It is written to be handed directly to Codex (or a similar agent) so
implementation remains consistent, real-time safe, and maintainable.

------------------------------------------------------------------------

# 1. Design Goals

## 1.1 Primary Goals

-   **Monophonic vocal/formant synth** with continuous vowel morphing.
-   **Modern FX** (ensemble, ping-pong delay, reverb).
-   **All controls are parameters** (host automatable) and **MIDI
    mappable** (internal learn + CC/NRPN).
-   **Real-time safe audio thread**: no locks, no allocations, no file
    IO.
-   Cross-platform: macOS + Windows.

## 1.2 Non-Goals (v1)

-   Polyphony (may be added later).
-   Convolution reverb (optional future).
-   Complex modulation matrix (optional future).

------------------------------------------------------------------------

# 2. Repository Layout

    /plugin
      /Source
        PluginProcessor.h/.cpp
        PluginEditor.h/.cpp
        Parameters.h/.cpp
        StateSerialization.h/.cpp

    /dsp
      VoiceEngine.h/.cpp
      MonoNoteManager.h/.cpp
      Smoothers.h/.cpp

      /synth
        Excitation.h/.cpp
        FormantBank.h/.cpp
        Biquad.h/.cpp
        FOFBank.h/.cpp           (optional alternative)
        VowelTables.h/.cpp
        TiltEQ.h/.cpp
        AmpEnvelope.h/.cpp

      /fx
        Ensemble.h/.cpp
        StereoDelay.h/.cpp
        ReverbFDN.h/.cpp         (or Schroeder)
        ShimmerPitch.h/.cpp      (optional)

    /midi
      MidiRouter.h/.cpp
      MidiLearn.h/.cpp
      MidiMapModel.h/.cpp
      NRPNDecoder.h/.cpp

    /ui
      Theme.h/.cpp
      Layout.h/.cpp
      Components/
        HeaderBar.h/.cpp
        MonkView.h/.cpp
        XYPad.h/.cpp
        Knob.h/.cpp
        Panel.h/.cpp
        TonePanel.h/.cpp
        DelayPanel.h/.cpp
        ReverbPanel.h/.cpp
        EnsemblePanel.h/.cpp
        MidiLearnOverlay.h/.cpp

    /tests
      OfflineRenderTests.md
      RenderHarness.cpp          (optional)
      GoldenFiles/              (optional)

    /docs
      MonkSynth_VST3_Project_Plan_and_AI_Agent_Prompt.md
      MonkSynth_VST3_DSP_Math_Details.md
      MonkSynth_JUCE_Architecture_Blueprint.md

------------------------------------------------------------------------

# 3. Core JUCE Types & Ownership

## 3.1 Main Classes

-   `MonkSynthAudioProcessor : public juce::AudioProcessor`
-   `MonkSynthAudioProcessorEditor : public juce::AudioProcessorEditor`

## 3.2 Major Owned Subsystems (in Processor)

-   `Parameters` (APVTS + helpers)
-   `StateSerialization` (save/restore incl. MIDI maps)
-   `MidiRouter` (decode CC/PB/NRPN, MIDI learn, write-to-parameter
    policy)
-   `VoiceEngine` (monophonic synth engine)
-   `FxChain` (ensemble, delay, reverb; part of dsp module)

### Ownership rule

The processor owns all DSP/MIDI systems and provides a **thread-safe
snapshot** of parameters to the DSP at block start.

------------------------------------------------------------------------

# 4. Parameter System Architecture

## 4.1 Parameters.h/.cpp

Implement: - `juce::AudioProcessorValueTreeState apvts;` - A single
function to build the parameter layout: -
`static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();`

### Parameter IDs

All parameters must have: - Stable string ID (snake_case) - Human name -
Range, default, skew (if needed) - Smoothing time constant (documented)

### Example IDs (partial)

-   `vowel`
-   `glide_ms`
-   `voice_character`
-   `vibrato_depth`
-   `vibrato_rate_hz`
-   `tilt`
-   `ensemble_amount`
-   `delay_mode`
-   `delay_time_ms`
-   `delay_feedback`
-   `delay_damping`
-   `delay_wet`
-   `reverb_type`
-   `reverb_size`
-   `reverb_decay`
-   `reverb_predelay_ms`
-   `reverb_wet`
-   `output_gain_db`

## 4.2 Parameter Access Pattern (Real-time Safe)

**Never** call APVTS getters per-sample for DSP.

Instead: - At the start of `processBlock`, build a lightweight
`ParamSnapshot`: - floats/enums already denormal-safe - precomputed
conversions (ms → samples etc.)

### ParamSnapshot

Create a POD-like struct in `Parameters.h`:

``` cpp
struct ParamSnapshot {
  float vowel01;
  float glideSeconds;
  float voice01;
  float vibDepthSemis;
  float vibRateHz;
  float tilt01;

  // FX
  int delayMode; // 0 mono, 1 stereo, 2 pingpong
  float delayTimeSeconds;
  float delayFeedback;
  float delayDamping01;
  float delayWet;

  int reverbType;
  float reverbSize01;
  float reverbDecaySeconds;
  float reverbPreDelaySeconds;
  float reverbWet;

  float ensembleAmount;
  float outputGainLin;
};
```

Populate once per block, then pass into DSP.

------------------------------------------------------------------------

# 5. MIDI Architecture

## 5.1 MidiRouter Responsibilities

File: `/midi/MidiRouter.h/.cpp`

Responsibilities: 1. Accept `juce::MidiBuffer&` each block 2. Decode: -
Note on/off - Pitchbend - CC - Channel pressure - NRPN (14-bit) 3. Apply
mappings (fixed defaults + user learn) 4. Decide whether to: - **Set
plugin parameters** (host-notifying) OR - **Apply modulation
internally** without overwriting base parameter

### Write-to-host policy

Include a setting parameter: - `midi_writes_automation` (bool)

If enabled: - wrap updates in: - `beginChangeGesture(param)` -
`setValueNotifyingHost()` - `endChangeGesture(param)`

If disabled: - update internal modulation layer only.

## 5.2 MIDI Learn Model

File: `/midi/MidiLearn.h/.cpp` and `/midi/MidiMapModel.h/.cpp`

-   A mapping is: **Source → TargetParamID + transform**
-   Source types:
    -   CC7, PB, AT, ChPressure, NRPN14
-   Transform:
    -   min/max scaling
    -   invert
    -   curve
    -   smoothing
    -   pickup mode (jump/soft takeover)

State must be stored in plugin state tree under `midi_maps` node.

## 5.3 Note/Pitch Routing

`MidiRouter` produces: - `MonoNoteEvent` stream (note on/off,
velocity) - `PitchBend` float (-1..+1) - A `ModSnapshot` (optional) for
modulation-only paths

`VoiceEngine` consumes these.

------------------------------------------------------------------------

# 6. Voice Engine Architecture

## 6.1 VoiceEngine Responsibilities

File: `/dsp/VoiceEngine.h/.cpp`

-   Monophonic voice allocation and smoothing
-   Owns:
    -   `MonoNoteManager`
    -   `Excitation`
    -   `FormantBank` (or `FOFBank`)
    -   `AmpEnvelope`
    -   `TiltEQ`

Main methods: - `prepare(const juce::dsp::ProcessSpec&)` - `reset()` -
`render(juce::AudioBuffer<float>& out, const ParamSnapshot&, const MidiBlockEvents&)`

## 6.2 MonoNoteManager

File: `/dsp/MonoNoteManager.h/.cpp`

Responsibilities: - Note priority policy (define one): - v1 default:
**last-note priority** - Legato detection - Produces: - target frequency
f0 - note gate state - velocity scalar

Interface: -
`processMidi(const juce::MidiBuffer&, MonoNoteState& stateOut)` -
`noteOn(note, vel)` / `noteOff(note)`

## 6.3 DSP Smoothing Objects

File: `/dsp/Smoothers.h/.cpp`

Provide: - `OnePoleSmoother` (time-constant based) -
`LinearRampSmoother` (N-sample ramp) - `SmoothedValue` wrappers
(optional)

Rule: - All audible parameter changes (formants, vowel, delay time) must
be smoothed.

------------------------------------------------------------------------

# 7. Synthesis Submodules

## 7.1 Excitation

File: `/dsp/synth/Excitation.h/.cpp`

Options: - Bandlimited saw (PolyBLEP) OR - Impulse train

Outputs mono `excitationSample` per sample.

Expose: - `setFrequency(f0)` - `processSample()`

## 7.2 VowelTables

File: `/dsp/synth/VowelTables.h/.cpp`

Data model: -
`struct VowelTarget { float F[NUM_FORMANTS]; float BW[NUM_FORMANTS]; float A[NUM_FORMANTS]; };` -
Provide built-in tables (tunable) for 5 vowels: - OO, OH, AH, EH, EE
(names are UI; internally numeric)

Interpolation: - piecewise linear across vowel axis - allow future
spline if desired

## 7.3 FormantBank (Filter-bank implementation)

File: `/dsp/synth/FormantBank.h/.cpp`

-   Contains `NUM_FORMANTS` bandpass filters
-   Each filter is `Biquad` with coefficient update at **control rate**
    (once per block or every N samples)
-   Input: excitation
-   Output: sum of formant bands with per-formant gains

Key method: - `processSample(x)`

## 7.4 FOFBank (Optional alternative)

File: `/dsp/synth/FOFBank.h/.cpp`

-   Grain scheduler per formant
-   Grain triggered at pitch period
-   Grain waveform: sine(Fi) \* exp(-t/tau)

Use only if filter bank cannot match desired sound.

## 7.5 TiltEQ

File: `/dsp/synth/TiltEQ.h/.cpp`

-   Simple shelf/tilt filter
-   Coeff update at block rate

## 7.6 AmpEnvelope

File: `/dsp/synth/AmpEnvelope.h/.cpp`

-   Simple AR (attack/release) smoothing gate
-   v1: no ADSR complexity needed

------------------------------------------------------------------------

# 8. FX Chain Architecture

Create `/dsp/fx/FxChain.h/.cpp` (optional convenience) that owns: -
`Ensemble` - `StereoDelay` - `ReverbFDN`

Each has: - `prepare(spec)` - `reset()` - `processBlock(buffer, params)`

## 8.1 Ensemble

-   micro detune / short delay
-   must be stable and not chorus-y by default

## 8.2 StereoDelay

-   circular buffers L/R
-   ping-pong routing option
-   delay time smoothing (avoid zipper)
-   damping filter in feedback loop

## 8.3 ReverbFDN (or Schroeder)

-   use stable reverb topology
-   shimmer mode optional (pitch shift in feedback)
-   parameters smoothed

------------------------------------------------------------------------

# 9. Threading & Real-Time Safety

## 9.1 Audio Thread Rules

Forbidden in `processBlock`: - heap allocation - mutex locking - file
I/O - logging - UI calls - std::function allocations

## 9.2 Communication Pattern

-   UI writes to APVTS (message thread)
-   Audio thread reads APVTS values **once per block** to create
    `ParamSnapshot`
-   MIDI learn changes update state tree on message thread; audio thread
    reads an atomic pointer to an immutable mapping snapshot

### Mapping snapshot approach (recommended)

-   Store mappings in `std::shared_ptr<const MidiMapSnapshot>`
-   Swap pointer atomically when user edits mappings
-   Audio thread reads pointer once per block

------------------------------------------------------------------------

# 10. Plugin State (Save/Load)

All state must be stored in APVTS state, plus custom nodes: -
`apvts.state` contains parameter values - Additional child node:
`midi_maps` - Additional child node: `ui_state` (panel open/closed, last
preset, etc.)

Implement: - `getStateInformation()` - `setStateInformation()`

Rules: - State restore must not allocate in audio thread (JUCE calls
these on message thread typically; still keep safe).

------------------------------------------------------------------------

# 11. UI Architecture

## 11.1 Editor Composition

`PluginEditor` holds: - `HeaderBar` - Left column: - `MonkView` -
`XYPad` - `CoreKnobRow` - Right column: - `TonePanel` - `EnsemblePanel`
(collapsible) - `DelayPanel` (collapsible) - `ReverbPanel`
(collapsible) - `ModPanel` (optional future)

## 11.2 Layout Engine

Use one approach consistently: - JUCE `FlexBox` for macro layout -
Explicit rect math for micro layout inside panels - All sizes derived
from base 1200x720 and scaled

Store base rects in `/ui/Layout.h` and scale via: -
`float scale = juce::Desktop::getInstance().getGlobalScaleFactor();` (or
your own UI scale parameter)

## 11.3 XYPad Behavior

-   XY pad writes to:
    -   `vowel` (Y axis, 0..1)
    -   optional pitch offset (X axis) depending on mode
-   Provide option to **emit MIDI** from XY (optional future); if
    implemented, must not be required for core operation.

## 11.4 MIDI Learn UX

-   Right-click any control → Learn/Clear
-   Global Learn mode button in header
-   Highlight mapped controls
-   Display last-received MIDI message in overlay

------------------------------------------------------------------------

# 12. Build & CI Notes

-   Use GitHub Actions:
    -   macOS build
    -   Windows build
-   Artifacts: package VST3
-   Keep third-party dependencies minimal
-   Use `JUCE_USE_CURL=0` unless needed

------------------------------------------------------------------------

# 13. Implementation Sequence (Codex-Friendly)

Milestone order: 1. Parameters + APVTS + ParamSnapshot 2. MIDI Router
(note + pitchbend + CC basics) 3. MonoNoteManager + pitch glide +
vibrato 4. Excitation + FormantBank + VowelTables + smoothing 5. FX
chain (Ensemble → Delay → Reverb) 6. MIDI learn + NRPN + mapping
snapshot 7. UI layout + XY pad + binding 8. Stress tests + optimization

------------------------------------------------------------------------

# 14. Acceptance Criteria

A build is considered correct when: - Plugin loads in a DAW, produces
sound, no glitches - All parameters are automatable and show in host -
MIDI learn maps any control successfully - Vowel morph is smooth and
stable - Ping-pong delay and reverb operate with expected routing - No
allocations or locks in audio thread (validated via instrumentation or
code review)

------------------------------------------------------------------------

End of Architecture Blueprint
