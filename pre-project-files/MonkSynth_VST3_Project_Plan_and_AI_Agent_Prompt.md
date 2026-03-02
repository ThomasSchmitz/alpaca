# MonkSynth VST3 Project

## Complete AI Agent Prompt + Full Development Plan

------------------------------------------------------------------------

# Part 1: Complete AI Agent Prompt (For Codex or Autonomous Development Agent)

You are a senior C++ DSP engineer building a professional VST3
instrument using JUCE.

Your goal is to build a monophonic singing-formant synthesizer inspired
by classic chanting vocal synths, but legally distinct, modernized, and
production-ready.

## Technical Constraints

-   Framework: JUCE (latest stable)
-   Format: VST3 (primary), AU optional
-   Language: C++17 or later
-   Must be cross-platform (macOS + Windows)
-   All parameters must be exposed as automatable VST3 parameters
-   All parameters must be MIDI mappable
-   Code must be modular and testable
-   DSP must be sample-rate independent
-   No third-party proprietary DSP libraries

------------------------------------------------------------------------

## Core Feature Requirements

### 1. Voice Architecture

-   Monophonic
-   MIDI note controlled pitch
-   Portamento (glide time adjustable)
-   Vibrato (rate + depth, MIDI controllable)
-   Pitchbend support
-   Optional MPE support (future-ready)

### 2. Synthesis Engine

Use a Formant-Based Architecture.

Preferred approach: - FOF-style formant grain bank OR - Parallel
bandpass filter bank fed by bandlimited excitation

Minimum: - 3--5 formants - Continuous vowel morphing - Formant
interpolation between vowel targets - Formant shift scaling (voice size
control) - Spectral tilt / brightness control - Breath/noise component
(optional)

### 3. Vowel Morphing

Implement continuous vowel morphing: - Store formant frequency,
bandwidth, amplitude per vowel - Interpolate smoothly - Parameter
smoothing to avoid zipper noise

### 4. Effects Section

Include:

Delay - Mono - Stereo - Ping-pong - Time (ms + sync) - Feedback -
Damping - Wet

Reverb - Hall - Cathedral - Plate - Shimmer (optional) - Size - Decay -
Pre-delay - Wet

Ensemble - Micro detune - Stereo width - Spread

Signal Chain Order: Synth → Tone → Ensemble → Delay → Reverb → Output

------------------------------------------------------------------------

## MIDI System Requirements

-   All parameters exposed as VST3 automatable parameters
-   Internal MIDI learn system
-   Support:
    -   CC (7-bit)
    -   14-bit CC where possible
    -   NRPN (14-bit)
    -   Pitchbend
    -   Channel pressure
-   Optional: MPE

Mapping System Must Support: - Range scaling - Inversion - Curves
(linear, exponential) - Smoothing - Pickup modes

------------------------------------------------------------------------

## UI Requirements

-   1200x720 base layout
-   Scalable UI
-   Left column: Performance (Monk + XY pad)
-   Right column: Tone + FX panels (collapsible)
-   Modern flat design
-   Minimal skeuomorphism
-   No copyrighted likeness

------------------------------------------------------------------------

## Performance Targets

-   CPU usage under 5% per instance on modern laptop
-   Stable at 44.1k, 48k, 96k
-   No denormals
-   No dynamic allocations in audio thread

------------------------------------------------------------------------

# Part 2: Complete Development Plan

------------------------------------------------------------------------

# Phase 1 -- Project Setup

1.  Create JUCE project
2.  Enable VST3
3.  Set up AudioProcessorValueTreeState
4.  Define all parameters
5.  Set up GitHub repository
6.  Configure CI builds

Deliverable: - Empty synth that compiles and loads

------------------------------------------------------------------------

# Phase 2 -- Monophonic Engine

1.  MIDI handling
2.  Note priority logic
3.  Portamento
4.  Vibrato LFO
5.  Parameter smoothing

Deliverable: - Stable monophonic oscillator responding to MIDI

------------------------------------------------------------------------

# Phase 3 -- Formant Engine

Option A (Recommended): - FOF grain scheduler - Per-formant envelope
shaping

Option B: - Parallel bandpass filter bank

Steps: 1. Implement 3 formants 2. Add vowel tables 3. Interpolation
logic 4. Add formant shift 5. Add tilt shaping

Deliverable: - Recognizable vocal timbre

------------------------------------------------------------------------

# Phase 4 -- Effects Implementation

Delay - Circular buffer - Feedback loop - Ping-pong routing - Sync
option

Reverb - Algorithmic reverb (Schroeder or FDN) - Optional shimmer via
pitch shifter in feedback

Ensemble - Micro pitch modulation - Stereo spread

Deliverable: - Full FX chain functional

------------------------------------------------------------------------

# Phase 5 -- MIDI Mapping System

1.  Create mapping structure
2.  Implement MIDI learn
3.  Add transform options
4.  Store mappings in state
5.  Ensure automation compatibility

Deliverable: - All parameters MIDI mappable

------------------------------------------------------------------------

# Phase 6 -- UI Implementation

1.  Layout scaffolding
2.  Monk animation system
3.  XY pad interaction
4.  Collapsible FX panels
5.  MIDI learn visual feedback
6.  High DPI scaling

Deliverable: - Fully interactive UI

------------------------------------------------------------------------

# Phase 7 -- Optimization & QA

1.  Profile CPU
2.  Remove audio-thread allocations
3.  Stress test automation
4.  Test across sample rates
5.  Cross-platform validation

Deliverable: - Release Candidate

------------------------------------------------------------------------

# Phase 8 -- Future Enhancements (Post v1)

-   MPE full support
-   Modulation matrix
-   Granular mode
-   Convolution reverb
-   Advanced modulation routing

------------------------------------------------------------------------

# Repository Structure

/plugin /dsp /ui /midi /tests /assets /docs

------------------------------------------------------------------------

# Milestone Timeline (Suggested)

Week 1: Project + Mono Engine\
Week 2: Formant Engine\
Week 3: FX\
Week 4: MIDI System\
Week 5: UI\
Week 6: Optimization + Beta

------------------------------------------------------------------------

# Final Goal

A modern chanting formant instrument that: - Feels expressive - Is fully
MIDI controllable - Has modern spatial FX - Is stable and professional -
Is legally distinct

------------------------------------------------------------------------

End of Document
