# MonkSynth VST3

## Parameter & MIDI Mapping Specification

This document defines the **complete parameter inventory** (IDs, ranges,
defaults, units, smoothing) and the **MIDI mapping system** (defaults,
NRPN plan, learn model, conflict policies).

Goals: - Every user-facing control is a **VST3 automatable parameter** -
Every parameter is **MIDI mappable** (fixed defaults + internal learn) -
MIDI can operate in two modes: **Set** (writes parameter) or
**Modulate** (adds modulation without overwriting base) -
High-resolution MIDI supported via **NRPN 14-bit**

------------------------------------------------------------------------

# 1. Conventions

## 1.1 Parameter ID Rules

-   IDs are stable `snake_case` strings (never change once released).
-   IDs never include spaces.
-   Enumerations are integer-backed but exposed as choice parameters.

## 1.2 Normalization

Internally, continuous parameters should be normalized to **0..1** for
modulation, then mapped to real units.

## 1.3 Smoothing

Smoothing is applied **in DSP**, not by querying APVTS per sample.

Recommended default smoothing time constants: - **Fast**: 5--10 ms
(vibrato depth, vibrato rate) - **Medium**: 15--30 ms (vowel,
voice/formant shift) - **Slow**: 30--60 ms (delay time, feedback, reverb
decay)

------------------------------------------------------------------------

# 2. Parameter Inventory

Unless stated otherwise: - Automatable: Yes - MIDI-mappable: Yes - Unit
conversion happens in `ParamSnapshot` creation.

## 2.1 Performance / Core

  --------------------------------------------------------------------------------------
  ID                  Name             Type       Range     Default Unit       Smoothing
  ------------------- --------- ----------- ----------- ----------- -------- -----------
  `vowel`             Vowel           float        0..1        0.35 norm           25 ms

  `glide_ms`          Glide           float     0..2000          50 ms            0 ms\*

  `voice_character`   Voice           float        0..1        0.50 norm           25 ms

  `vibrato_depth`     Vibrato         float        0..1        0.00 norm           10 ms
                      Depth                                                  

  `vibrato_rate_hz`   Vibrato         float     0.1..12         5.5 Hz             10 ms
                      Rate                                                   

  `amp_attack_ms`     Amp             float      0..200           5 ms              0 ms
                      Attack                                                 

  `amp_release_ms`    Amp             float     5..2000         120 ms              0 ms
                      Release                                                

  `output_gain_db`    Output          float    -24..+12           0 dB             10 ms
                      Gain                                                   
  --------------------------------------------------------------------------------------

\* Pitch glide is implemented as part of the pitch smoother; the glide
parameter itself does not need smoothing beyond block-rate stability.

## 2.2 Pitch / Play Modes (Optional but recommended)

  -----------------------------------------------------------------------------------------------
  ID                           Name        Type        Choices / Range Default       Notes
  ---------------------------- ----------- ----------- --------------- ------------- ------------
  `pitch_control_mode`         Pitch       choice      `midi_note`,    `midi_note`   Determines
                               Control                 `xy_offset`,                  how XY
                               Mode                    `xy_absolute`                 X-axis
                                                                                     affects
                                                                                     pitch

  `xy_pitch_range_semitones`   XY Pitch    float       0..24           12            semis
                               Range                                                 

  `mono_priority`              Mono        choice      `last`, `low`,  `last`        v1 default
                               Priority                `high`                        is last-note
  -----------------------------------------------------------------------------------------------

## 2.3 Tone / Vocal Shaping

  ------------------------------------------------------------------------------------------------------
  ID                          Name                     Type       Range     Default Unit       Smoothing
  --------------------------- ----------------- ----------- ----------- ----------- -------- -----------
  `formant_shift_semitones`   Formant Shift           float    -12..+12           0 semis          25 ms

  `tilt`                      Brightness/Tilt         float        0..1        0.55 norm           20 ms

  `breath_noise`              Breath/Noise            float        0..1        0.00 norm           20 ms

  `voice_size`                Tract Size              float        0..1        0.50 norm           25 ms
  ------------------------------------------------------------------------------------------------------

Notes: - `voice_character` is a musical macro; `voice_size` can be a
more explicit formant-scaling control if you want both.

## 2.4 Ensemble

  ---------------------------------------------------------------------------------------------
  ID                        Name              Type Range /         Default Unit       Smoothing
                                                   Choices                          
  ------------------------- ---------- ----------- ----------- ----------- -------- -----------
  `ensemble_enable`         Ensemble          bool off/on               on ---              ---
                            Enable                                                  

  `ensemble_amount`         Ensemble         float 0..1               0.15 norm           30 ms
                            Amount                                                  

  `ensemble_width`          Ensemble         float 0..1               0.65 norm           30 ms
                            Width                                                   

  `ensemble_detune_cents`   Ensemble         float 0..20                 6 cents          30 ms
                            Detune                                                  

  `ensemble_mode`           Ensemble        choice `mono`,        `stereo` ---             0 ms
                            Mode                   `stereo`,                        
                                                   `choir`                          
  ---------------------------------------------------------------------------------------------

## 2.5 Delay

  ---------------------------------------------------------------------------------------------------------------------------
  ID                 Name              Type Range / Choices                                      Default Unit       Smoothing
  ------------------ ---------- ----------- ---------------------------------------------- ------------- -------- -----------
  `delay_enable`     Delay             bool off/on                                                    on ---              ---
                     Enable                                                                                       

  `delay_mode`       Delay Mode      choice `mono`, `stereo`, `ping_pong`                    `ping_pong` ---             0 ms

  `delay_sync`       Delay Sync        bool off/on                                                   off ---              ---

  `delay_time_ms`    Delay Time       float 1..2000                                                  320 ms             50 ms

  `delay_time_div`   Delay           choice `1/1,1/2,1/4,1/8,1/16,1/8T,1/16T,1/8D,1/16D`           `1/8` ---             0 ms
                     Division                                                                                     

  `delay_feedback`   Delay            float 0..0.95                                                 0.35 norm           40 ms
                     Feedback                                                                                     

  `delay_damping`    Delay            float 0..1                                                    0.35 norm           40 ms
                     Damping                                                                                      

  `delay_wet`        Delay Wet        float 0..1                                                    0.25 norm           20 ms
  ---------------------------------------------------------------------------------------------------------------------------

Notes: - If `delay_sync=on`, compute time in seconds from host BPM and
`delay_time_div`.

## 2.6 Reverb

  ----------------------------------------------------------------------------------------------
  ID                     Name               Type Range /            Default Unit       Smoothing
                                                 Choices                             
  ---------------------- ----------- ----------- -------------- ----------- -------- -----------
  `reverb_enable`        Reverb             bool off/on                  on ---              ---
                         Enable                                                      

  `reverb_type`          Reverb Type      choice `hall`,             `hall` ---             0 ms
                                                 `cathedral`,                        
                                                 `plate`,                            
                                                 `shimmer`                           

  `reverb_size`          Reverb Size       float 0..1                  0.60 norm           50 ms

  `reverb_decay_s`       Reverb            float 0.1..20                4.5 s              80 ms
                         Decay                                                       

  `reverb_predelay_ms`   Reverb            float 0..250                  18 ms             50 ms
                         Pre-delay                                                   

  `reverb_damping`       Reverb            float 0..1                  0.40 norm           60 ms
                         Damping                                                     

  `reverb_wet`           Reverb Wet        float 0..1                  0.22 norm           40 ms
  ----------------------------------------------------------------------------------------------

## 2.7 MIDI / Learn / Global Behavior

  ----------------------------------------------------------------------------------------------
  ID                         Name         Type        Range /        Default     Notes
                                                      Choices                    
  -------------------------- ------------ ----------- -------------- ----------- ---------------
  `midi_learn_enable`        MIDI Learn   bool        off/on         off         UI-only toggle;
                             Enable                                              not required to
                                                                                 automate

  `midi_writes_automation`   MIDI Writes  bool        off/on         off         If on, MIDI
                             Automation                                          sets params via
                                                                                 host
                                                                                 notifications

  `midi_mapping_mode`        MIDI Mapping choice      `set`,         `set`       `set`
                             Mode                     `modulate`                 overwrites base
                                                                                 value;
                                                                                 `modulate` adds

  `midi_channel`             MIDI Channel choice      `omni,1..16`   `omni`      Applies to
                                                                                 learn + fixed
                                                                                 mappings

  `midi_hi_res`              MIDI Hi-Res  choice      `cc7`,         `nrpn14`    Determines
                             Mode                     `nrpn14`                   preferred
                                                                                 high-res path
  ----------------------------------------------------------------------------------------------

------------------------------------------------------------------------

# 3. Default MIDI Mappings

These are the "out-of-the-box" assignments. Users can override via MIDI
Learn.

## 3.1 Core Defaults (Compatibility-inspired)

-   Pitchbend → `vowel` (high-res)
-   CC1 (Mod Wheel) → `vibrato_depth`
-   CC5 → `glide_ms`
-   CC12 → `delay_wet`
-   CC13 → `voice_character`

## 3.2 Suggested Additional Defaults (Modern usability)

-   CC2 (Breath) → `breath_noise` (or `vowel` if you want breath
    controller = vowel)
-   CC74 (Timbre) → `vowel` (if MPE-like controllers are used without
    full MPE)
-   CC11 (Expression) → `output_gain_db` (scaled -inf..0 style, or to
    -24..0 dB)

## 3.3 FX Defaults (Optional)

-   CC14 → `reverb_wet`
-   CC15 → `delay_feedback`
-   CC16 → `ensemble_amount`

If you want to keep CC space minimal, skip these and rely on Learn.

------------------------------------------------------------------------

# 4. NRPN 14-bit Mapping Plan

## 4.1 Why NRPN

-   7-bit CC causes audible stepping for vowel and delay-time sweeps.
-   NRPN provides 14-bit resolution (0..16383).

## 4.2 NRPN Message Format

Standard NRPN uses CC messages: - CC99: NRPN MSB - CC98: NRPN LSB -
CC06: Data Entry MSB - CC38: Data Entry LSB (optional but recommended)
Optional increment/decrement: - CC96/97

## 4.3 NRPN Address Map (Recommended)

Pick a dedicated MSB bank for the plugin, e.g. MSB=7.

-   NRPN MSB = 7
-   NRPN LSB = parameter number (0..127)

Proposed LSB assignments: - 0: `vowel` - 1: `voice_character` - 2:
`glide_ms` - 3: `vibrato_depth` - 4: `vibrato_rate_hz` - 5:
`delay_wet` - 6: `delay_time_ms` - 7: `delay_feedback` - 8:
`reverb_wet` - 9: `reverb_decay_s` - 10: `tilt` - 11:
`ensemble_amount` - 12: `formant_shift_semitones` - 13: `breath_noise` -
14: `voice_size`

Rule: - NRPN value 0..16383 maps to parameter range linearly unless a
curve is specified.

------------------------------------------------------------------------

# 5. MIDI Learn Specification

## 5.1 Mapping Model

A mapping entry stores:

-   Source:
    -   type: `cc7 | pitchbend | aftertouch | ch_pressure | nrpn14`
    -   number: CC number or NRPN LSB, PB is implicit
    -   channel: 1..16 or omni
-   Target:
    -   parameter ID string
-   Transform:
    -   min, max in target units (or normalized 0..1)
    -   invert (bool)
    -   curve: `linear | exp | log | s_curve`
    -   smoothing_ms (0..200)
    -   pickup: `jump | soft_takeover`

## 5.2 Learn Workflow

1.  User enters learn mode
2.  User selects a target control
3.  Next received MIDI message becomes the source
4.  A default transform is applied:
    -   range = full parameter range
    -   curve = linear
    -   smoothing = parameter default
    -   pickup = soft_takeover
5.  Mapping is stored and instantly active

## 5.3 Soft Takeover Rule

When enabled: - Ignore incoming MIDI until it crosses current parameter
value within a tolerance. - Then latch and start applying.

------------------------------------------------------------------------

# 6. MIDI vs Automation Conflict Policy

Provide a global choice (parameter or hidden config):

## 6.1 Policy A (Recommended Default): Automation Wins

-   If host automation is changing a parameter this block, ignore MIDI
    for that parameter.

Implementation note: - Track "automation touched" by comparing the base
parameter value to previous block with a threshold OR use host gesture
events if available.

## 6.2 Policy B: Last-Touched Wins

-   Last source (MIDI or automation) that changed parameter becomes
    authoritative for N seconds.

Not required v1; can be added later.

------------------------------------------------------------------------

# 7. Set vs Modulate Mapping Mode

## 7.1 Set Mode

-   Incoming MIDI sets the parameter value directly.
-   Optionally can write automation if `midi_writes_automation=on`.

## 7.2 Modulate Mode (Recommended as optional)

Maintain: - Base parameter value (from host) - Mod value (from MIDI
mapping, normalized -1..+1 or 0..1 depending on target) Final value =
clamp(base + depth\*mod)

Notes: - For unipolar parameters, use 0..1 modulation with offset. - For
bipolar, use -1..+1.

------------------------------------------------------------------------

# 8. Smoothing Rules for MIDI Control

-   Apply smoothing **after** mapping/transform, in DSP space.
-   For stepping sources (CC7), smoothing defaults to 25--60 ms for
    vowel/delay time.
-   For NRPN14, smoothing can be lower (10--30 ms).

------------------------------------------------------------------------

# 9. State Serialization Requirements

Store in APVTS state: - Parameter values (automatic) - `midi_maps` child
node containing: - list of mappings - global MIDI settings (channel,
hi-res mode, mapping mode, write automation)

On restore: - rebuild immutable `MidiMapSnapshot` - atomically swap into
audio thread

------------------------------------------------------------------------

# 10. Acceptance Tests (Mapping)

A build passes mapping spec when: - All parameters appear in host
automation list - MIDI learn can map CC and NRPN to any parameter - NRPN
sweeps vowel smoothly without stepping - Soft takeover works (no
jumps) - MIDI mapping survives save/reload - When automation is active,
policy behaves as specified

------------------------------------------------------------------------

End of Parameter & MIDI Mapping Specification
