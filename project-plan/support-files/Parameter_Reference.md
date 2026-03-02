# Alpaca VST3 - Complete Parameter Reference

**Document Version:** 1.0
**Last Updated:** 2026-03-02
**Purpose:** Comprehensive database of all plugin parameters

---

## Parameter Database

This document serves as the single source of truth for all parameters in the Alpaca VST3 plugin.

### Legend

- **ID:** Stable string identifier (never changes)
- **Name:** Display name in UI
- **Type:** float, int, bool, choice
- **Range:** Min..Max or list of choices
- **Default:** Initial value
- **Unit:** Display unit (Hz, ms, dB, %, etc.)
- **Automatable:** Can be automated by host
- **MIDI:** Can be MIDI mapped
- **Smoothing:** Time constant in milliseconds
- **Category:** Grouping for organization

---

## Performance Parameters

### vowel
- **ID:** `vowel`
- **Name:** Vowel
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.35
- **Unit:** normalized (0=OO, 0.25=OH, 0.5=AH, 0.75=EH, 1.0=EE)
- **Automatable:** Yes
- **MIDI:** Yes (default: Pitch Bend)
- **Smoothing:** 25 ms
- **Category:** Performance
- **Description:** Controls vowel morphing through 5 vowel positions

### glide_ms
- **ID:** `glide_ms`
- **Name:** Glide
- **Type:** float
- **Range:** 0.0 to 2000.0
- **Default:** 50.0
- **Unit:** milliseconds
- **Automatable:** Yes
- **MIDI:** Yes (default: CC5)
- **Smoothing:** 0 ms (applied in pitch smoother)
- **Category:** Performance
- **Description:** Portamento/glide time between notes

### voice_character
- **ID:** `voice_character`
- **Name:** Voice Character
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.50
- **Unit:** normalized (0=dark/male, 1=bright/female)
- **Automatable:** Yes
- **MIDI:** Yes (default: CC13)
- **Smoothing:** 25 ms
- **Category:** Performance
- **Description:** Overall voice character and timbre

### vibrato_depth
- **ID:** `vibrato_depth`
- **Name:** Vibrato Depth
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.0
- **Unit:** normalized (maps to 0-2 semitones internally)
- **Automatable:** Yes
- **MIDI:** Yes (default: CC1 Mod Wheel)
- **Smoothing:** 10 ms
- **Category:** Performance
- **Description:** Vibrato depth/amount

### vibrato_rate_hz
- **ID:** `vibrato_rate_hz`
- **Name:** Vibrato Rate
- **Type:** float
- **Range:** 0.1 to 12.0
- **Default:** 5.5
- **Unit:** Hz
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 10 ms
- **Category:** Performance
- **Description:** Vibrato speed/rate

---

## Envelope Parameters

### amp_attack_ms
- **ID:** `amp_attack_ms`
- **Name:** Amp Attack
- **Type:** float
- **Range:** 0.0 to 200.0
- **Default:** 5.0
- **Unit:** milliseconds
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 0 ms
- **Category:** Envelope
- **Description:** Amplitude envelope attack time

### amp_release_ms
- **ID:** `amp_release_ms`
- **Name:** Amp Release
- **Type:** float
- **Range:** 5.0 to 2000.0
- **Default:** 120.0
- **Unit:** milliseconds
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 0 ms
- **Category:** Envelope
- **Description:** Amplitude envelope release time

---

## Tone Shaping Parameters

### formant_shift_semitones
- **ID:** `formant_shift_semitones`
- **Name:** Formant Shift
- **Type:** float
- **Range:** -12.0 to +12.0
- **Default:** 0.0
- **Unit:** semitones
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 25 ms
- **Category:** Tone
- **Description:** Shifts all formant frequencies (voice size control)

### tilt
- **ID:** `tilt`
- **Name:** Brightness
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.55
- **Unit:** normalized (0=dark, 1=bright)
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 20 ms
- **Category:** Tone
- **Description:** Spectral tilt/brightness control

### breath_noise
- **ID:** `breath_noise`
- **Name:** Breath/Noise
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.0
- **Unit:** normalized
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 20 ms
- **Category:** Tone
- **Description:** Amount of breath noise for realism

---

## Ensemble Parameters

### ensemble_enable
- **ID:** `ensemble_enable`
- **Name:** Ensemble Enable
- **Type:** bool
- **Range:** off/on
- **Default:** on
- **Unit:** boolean
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** N/A
- **Category:** Ensemble
- **Description:** Enable/disable ensemble effect

### ensemble_amount
- **ID:** `ensemble_amount`
- **Name:** Ensemble Amount
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.15
- **Unit:** normalized
- **Automatable:** Yes
- **MIDI:** Yes (suggested: CC16)
- **Smoothing:** 30 ms
- **Category:** Ensemble
- **Description:** Ensemble effect intensity

### ensemble_width
- **ID:** `ensemble_width`
- **Name:** Ensemble Width
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.65
- **Unit:** normalized (0=mono, 1=wide stereo)
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 30 ms
- **Category:** Ensemble
- **Description:** Stereo width of ensemble effect

### ensemble_detune_cents
- **ID:** `ensemble_detune_cents`
- **Name:** Ensemble Detune
- **Type:** float
- **Range:** 0.0 to 20.0
- **Default:** 6.0
- **Unit:** cents
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 30 ms
- **Category:** Ensemble
- **Description:** Micro-detune amount

### ensemble_mode
- **ID:** `ensemble_mode`
- **Name:** Ensemble Mode
- **Type:** choice
- **Range:** mono, stereo, choir
- **Default:** stereo
- **Unit:** enumeration
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 0 ms
- **Category:** Ensemble
- **Description:** Ensemble processing mode

---

## Delay Parameters

### delay_enable
- **ID:** `delay_enable`
- **Name:** Delay Enable
- **Type:** bool
- **Range:** off/on
- **Default:** on
- **Unit:** boolean
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** N/A
- **Category:** Delay
- **Description:** Enable/disable delay effect

### delay_mode
- **ID:** `delay_mode`
- **Name:** Delay Mode
- **Type:** choice
- **Range:** mono, stereo, ping_pong
- **Default:** ping_pong
- **Unit:** enumeration
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 0 ms
- **Category:** Delay
- **Description:** Delay routing mode

### delay_sync
- **ID:** `delay_sync`
- **Name:** Delay Sync
- **Type:** bool
- **Range:** off/on
- **Default:** off
- **Unit:** boolean
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** N/A
- **Category:** Delay
- **Description:** Enable tempo synchronization

### delay_time_ms
- **ID:** `delay_time_ms`
- **Name:** Delay Time
- **Type:** float
- **Range:** 1.0 to 2000.0
- **Default:** 320.0
- **Unit:** milliseconds
- **Automatable:** Yes
- **MIDI:** Yes (default: CC12 for wet, could map to time)
- **Smoothing:** 50 ms
- **Category:** Delay
- **Description:** Delay time in milliseconds (when sync is off)

### delay_time_div
- **ID:** `delay_time_div`
- **Name:** Delay Division
- **Type:** choice
- **Range:** 1/1, 1/2, 1/4, 1/8, 1/16, 1/8T, 1/16T, 1/8D, 1/16D
- **Default:** 1/8
- **Unit:** musical division
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 0 ms
- **Category:** Delay
- **Description:** Delay time division when synced

### delay_feedback
- **ID:** `delay_feedback`
- **Name:** Delay Feedback
- **Type:** float
- **Range:** 0.0 to 0.95
- **Default:** 0.35
- **Unit:** normalized
- **Automatable:** Yes
- **MIDI:** Yes (suggested: CC15)
- **Smoothing:** 40 ms
- **Category:** Delay
- **Description:** Delay feedback amount

### delay_damping
- **ID:** `delay_damping`
- **Name:** Delay Damping
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.35
- **Unit:** normalized (0=no damping, 1=heavy)
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 40 ms
- **Category:** Delay
- **Description:** High-frequency damping in feedback loop

### delay_wet
- **ID:** `delay_wet`
- **Name:** Delay Wet
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.25
- **Unit:** normalized
- **Automatable:** Yes
- **MIDI:** Yes (default: CC12)
- **Smoothing:** 20 ms
- **Category:** Delay
- **Description:** Delay wet/dry mix

---

## Reverb Parameters

### reverb_enable
- **ID:** `reverb_enable`
- **Name:** Reverb Enable
- **Type:** bool
- **Range:** off/on
- **Default:** on
- **Unit:** boolean
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** N/A
- **Category:** Reverb
- **Description:** Enable/disable reverb effect

### reverb_type
- **ID:** `reverb_type`
- **Name:** Reverb Type
- **Type:** choice
- **Range:** hall, cathedral, plate, shimmer
- **Default:** hall
- **Unit:** enumeration
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 0 ms
- **Category:** Reverb
- **Description:** Reverb algorithm type

### reverb_size
- **ID:** `reverb_size`
- **Name:** Reverb Size
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.60
- **Unit:** normalized
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 50 ms
- **Category:** Reverb
- **Description:** Reverb room size

### reverb_decay_s
- **ID:** `reverb_decay_s`
- **Name:** Reverb Decay
- **Type:** float
- **Range:** 0.1 to 20.0
- **Default:** 4.5
- **Unit:** seconds
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 80 ms
- **Category:** Reverb
- **Description:** Reverb decay time

### reverb_predelay_ms
- **ID:** `reverb_predelay_ms`
- **Name:** Reverb Pre-delay
- **Type:** float
- **Range:** 0.0 to 250.0
- **Default:** 18.0
- **Unit:** milliseconds
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 50 ms
- **Category:** Reverb
- **Description:** Pre-delay before reverb onset

### reverb_damping
- **ID:** `reverb_damping`
- **Name:** Reverb Damping
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.40
- **Unit:** normalized
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 60 ms
- **Category:** Reverb
- **Description:** High-frequency damping/absorption

### reverb_wet
- **ID:** `reverb_wet`
- **Name:** Reverb Wet
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.22
- **Unit:** normalized
- **Automatable:** Yes
- **MIDI:** Yes (suggested: CC14)
- **Smoothing:** 40 ms
- **Category:** Reverb
- **Description:** Reverb wet/dry mix

---

## Global Parameters

### output_gain_db
- **ID:** `output_gain_db`
- **Name:** Output Gain
- **Type:** float
- **Range:** -24.0 to +12.0
- **Default:** 0.0
- **Unit:** dB
- **Automatable:** Yes
- **MIDI:** Yes (suggested: CC11 Expression)
- **Smoothing:** 10 ms
- **Category:** Global
- **Description:** Master output gain

---

## MIDI System Parameters

### midi_learn_enable
- **ID:** `midi_learn_enable`
- **Name:** MIDI Learn Enable
- **Type:** bool
- **Range:** off/on
- **Default:** off
- **Unit:** boolean
- **Automatable:** No
- **MIDI:** No
- **Smoothing:** N/A
- **Category:** MIDI
- **Description:** Toggle MIDI learn mode (UI only)

### midi_writes_automation
- **ID:** `midi_writes_automation`
- **Name:** MIDI Writes Automation
- **Type:** bool
- **Range:** off/on
- **Default:** off
- **Unit:** boolean
- **Automatable:** No
- **MIDI:** No
- **Smoothing:** N/A
- **Category:** MIDI
- **Description:** If on, MIDI sets params via host notifications

### midi_mapping_mode
- **ID:** `midi_mapping_mode`
- **Name:** MIDI Mapping Mode
- **Type:** choice
- **Range:** set, modulate
- **Default:** set
- **Unit:** enumeration
- **Automatable:** No
- **MIDI:** No
- **Smoothing:** N/A
- **Category:** MIDI
- **Description:** Set overwrites parameter, modulate adds to it

### midi_channel
- **ID:** `midi_channel`
- **Name:** MIDI Channel
- **Type:** choice
- **Range:** omni, 1, 2, 3, ..., 16
- **Default:** omni
- **Unit:** enumeration
- **Automatable:** No
- **MIDI:** No
- **Smoothing:** N/A
- **Category:** MIDI
- **Description:** MIDI input channel filter

### midi_hi_res
- **ID:** `midi_hi_res`
- **Name:** MIDI Hi-Res Mode
- **Type:** choice
- **Range:** cc7, nrpn14
- **Default:** nrpn14
- **Unit:** enumeration
- **Automatable:** No
- **MIDI:** No
- **Smoothing:** N/A
- **Category:** MIDI
- **Description:** Preferred high-resolution MIDI mode

---

## Optional/Advanced Parameters (Future)

### pitch_control_mode
- **ID:** `pitch_control_mode`
- **Name:** Pitch Control Mode
- **Type:** choice
- **Range:** midi_note, xy_offset, xy_absolute
- **Default:** midi_note
- **Unit:** enumeration
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 0 ms
- **Category:** Advanced
- **Description:** How XY pad X-axis affects pitch

### xy_pitch_range_semitones
- **ID:** `xy_pitch_range_semitones`
- **Name:** XY Pitch Range
- **Type:** float
- **Range:** 0.0 to 24.0
- **Default:** 12.0
- **Unit:** semitones
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 0 ms
- **Category:** Advanced
- **Description:** Pitch range for XY pad X-axis

### mono_priority
- **ID:** `mono_priority`
- **Name:** Mono Priority
- **Type:** choice
- **Range:** last, low, high
- **Default:** last
- **Unit:** enumeration
- **Automatable:** No
- **MIDI:** No
- **Smoothing:** N/A
- **Category:** Advanced
- **Description:** Monophonic note priority mode

### voice_size
- **ID:** `voice_size`
- **Name:** Voice Size
- **Type:** float
- **Range:** 0.0 to 1.0
- **Default:** 0.50
- **Unit:** normalized
- **Automatable:** Yes
- **MIDI:** Yes
- **Smoothing:** 25 ms
- **Category:** Tone
- **Description:** Vocal tract size (alternative to formant shift)

---

## Parameter Summary

**Total Parameters:** 35+ (core set)

**By Category:**
- Performance: 5
- Envelope: 2
- Tone: 3 (5 with optional)
- Ensemble: 5
- Delay: 8
- Reverb: 7
- Global: 1
- MIDI: 5
- Advanced: 3 (optional)

**Automatable Parameters:** 32+
**MIDI-Mappable Parameters:** 30+

---

## Implementation Notes

### Parameter ID Stability

Parameter IDs must **never change** once released. They are used for:
- Host automation lanes
- Preset files
- MIDI mapping persistence
- API compatibility

If a parameter needs to be renamed, deprecated, or changed:
1. Keep the old ID
2. Add a new parameter if needed
3. Maintain backward compatibility in state loading

### Default MIDI CC Mappings

These CCs are mapped by default (user can override via MIDI learn):
- CC1 (Mod Wheel) → `vibrato_depth`
- CC5 → `glide_ms`
- CC12 → `delay_wet`
- CC13 → `voice_character`
- CC74 → `vowel` (alternative)
- Pitch Bend → `vowel` (primary)

### Parameter Smoothing Strategy

Smoothing is applied in DSP, not by polling APVTS per-sample:
1. Create `ParamSnapshot` at block start
2. Smoother objects process per-sample
3. Use one-pole filter with time constant

### Value Ranges and Conversion

**Normalized Parameters (0..1):**
- Stored internally as 0..1
- Converted to real units in `ParamSnapshot`
- Display conversion in UI

**Skewed Parameters:**
Some parameters may benefit from skew factor:
- `reverb_decay_s`: logarithmic feel (skew 0.5)
- `delay_feedback`: slightly logarithmic (skew 0.7)

**Boolean Parameters:**
- Stored as float (0.0 or 1.0)
- Display as toggle in UI

**Choice Parameters:**
- Stored as int index
- String list for display

---

## JUCE Parameter Layout Example

```cpp
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Performance
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "vowel",
        "Vowel",
        juce::NormalisableRange<float>(0.0f, 1.0f),
        0.35f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "glide_ms",
        "Glide",
        juce::NormalisableRange<float>(0.0f, 2000.0f),
        50.0f
    ));

    // ... (continue for all parameters)

    return { params.begin(), params.end() };
}
```

---

**END OF PARAMETER REFERENCE**
