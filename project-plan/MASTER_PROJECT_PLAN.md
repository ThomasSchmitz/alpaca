# Alpaca VST3 Project - Master Development Plan

**Project Name:** Alpaca - Enhanced Delay Lama VST3 Clone
**Target Platform:** macOS (primary), Windows (future)
**Format:** VST3, AU (optional)
**Framework:** JUCE 7.x or later
**Language:** C++17 or later
**Status:** Planning Phase
**Estimated Timeline:** 6-8 weeks development + 2 weeks testing

---

## Executive Summary

This document provides a comprehensive start-to-finish work plan for producing and publishing a VST3 enhanced-clone of the classic Delay Lama plugin for macOS. The project will be named "Alpaca" to create a legally distinct identity while honoring the spirit of the original.

**Core Objective:** Create a professional, modern VST3 plugin that captures the essence and functionality of the original Delay Lama while adding contemporary features, full MIDI compatibility, and robust DAW integration.

**Key Differentiation:** While inspired by Delay Lama's formant synthesis and playful interface, Alpaca will be:
- Legally distinct with original artwork and character design
- Enhanced with modern plugin features and workflow improvements
- Fully compatible with modern DAWs including Ableton Live
- MIDI mappable for external control surfaces
- Built with professional audio engineering standards

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Technical Architecture](#technical-architecture)
3. [Development Phases](#development-phases)
4. [Feature Specifications](#feature-specifications)
5. [Quality Assurance](#quality-assurance)
6. [Build & Release](#build--release)
7. [Documentation](#documentation)
8. [Success Criteria](#success-criteria)
9. [Risk Management](#risk-management)
10. [Resources & References](#resources--references)

---

## 1. Project Overview

### 1.1 Project Goals

**Primary Goals:**
1. Create a fully functional VST3 plugin replicating Delay Lama's core functionality
2. Add modern plugin features and workflow enhancements
3. Ensure compatibility with all major DAWs, especially Ableton Live
4. Implement comprehensive MIDI control for external surfaces
5. Build with professional audio engineering standards (real-time safe, efficient)

**Secondary Goals:**
1. Create an engaging, original visual design
2. Provide excellent user experience and documentation
3. Build foundation for future enhancements (AU format, polyphony, etc.)
4. Establish a clean, maintainable codebase for long-term development

### 1.2 Original Delay Lama Analysis

**Core Features to Preserve:**
- Monophonic formant-based vocal synthesis
- Real-time vowel morphing (ooh-ow-ah-ayh-eeh)
- XY pad for intuitive vowel/pitch control
- Built-in stereo delay effect
- Animated visual feedback synchronized with sound
- Playful, accessible interface design
- Full MIDI integration

**Technical Foundation:**
- Parallel formant filter synthesis
- Bandpass/resonant filters for vowel shaping
- Real-time parameter interpolation
- Simple but effective delay line processing
- Monophonic voice architecture

**Cultural Significance:**
- Instant recognizability and nostalgic value
- Balance of serious DSP with playful presentation
- Low barrier to entry, high creative potential

### 1.3 Enhanced Features for Alpaca

**Modern Plugin Features:**
1. **Enhanced Synthesis:**
   - Dual ADSR envelopes (amplitude + filter)
   - Multimode resonant filter section (LPF/HPF/BPF)
   - LFO modulation for vibrato and filter sweeps
   - Adjustable formant shift for voice character changes
   - Breath/noise component for realism

2. **Extended Effects Chain:**
   - Ensemble/chorus effect for width and richness
   - Multiple delay modes (mono/stereo/ping-pong)
   - Tempo-synced delay with division options
   - Multiple reverb types (hall/cathedral/plate/shimmer)
   - Spectral tilt/brightness control

3. **MIDI & Control:**
   - Full MIDI CC mapping with learn functionality
   - NRPN 14-bit support for high-resolution control
   - External control surface integration
   - MPE compatibility (future-ready)
   - Pitch bend, mod wheel, aftertouch support

4. **User Experience:**
   - Preset management system
   - A/B comparison
   - Undo/redo
   - Scalable UI (DPI-aware)
   - Context-sensitive help
   - CPU meter and performance monitoring

5. **Professional Features:**
   - Host automation support for all parameters
   - State save/recall
   - MIDI mapping persistence
   - Oversampling options
   - Multiple sample rate support (44.1k-96k)

---

## 2. Technical Architecture

### 2.1 Technology Stack

**Framework:** JUCE 7.x
- Cross-platform plugin development framework
- Mature, well-documented, industry-standard
- Built-in VST3/AU wrappers
- Excellent UI toolkit with modern rendering

**Language:** C++17 or later
- Performance-critical audio processing
- Real-time safe programming patterns
- Modern C++ features for clean code

**Build System:** CMake
- Cross-platform build configuration
- Easy integration with CI/CD
- Dependency management via CPM or FetchContent

**Version Control:** Git + GitHub
- Source code management
- Issue tracking and project management
- CI/CD via GitHub Actions

**Testing:** Catch2 or Google Test
- Unit testing for DSP algorithms
- Integration testing for plugin functionality
- Automated test suite in CI pipeline

### 2.2 Plugin Architecture

**High-Level Structure:**

```
AlpacaAudioProcessor (juce::AudioProcessor)
├── Parameters (AudioProcessorValueTreeState)
├── State Serialization
├── MIDI Router
│   ├── MIDI Learn System
│   ├── MIDI Map Model
│   └── NRPN Decoder
├── Voice Engine
│   ├── Mono Note Manager
│   ├── Excitation Generator
│   ├── Formant Bank
│   ├── Vowel Tables
│   └── Envelope Generators
└── FX Chain
    ├── Ensemble
    ├── Stereo Delay
    └── Reverb

AlpacaAudioProcessorEditor (juce::AudioProcessorEditor)
├── Header Bar
├── Performance Column
│   ├── Alpaca Character View
│   ├── XY Pad
│   └── Core Knob Row
├── FX Column
│   ├── Tone Panel
│   ├── Ensemble Panel
│   ├── Delay Panel
│   └── Reverb Panel
└── MIDI Learn Overlay
```

**Key Architectural Principles:**
1. **Real-time Safety:** No allocations, locks, or blocking operations in audio thread
2. **Parameter Management:** Single source of truth via APVTS, snapshot pattern for audio thread
3. **Modularity:** Clear separation of DSP, MIDI, and UI concerns
4. **Testability:** Pure functions for DSP, dependency injection for testing
5. **Maintainability:** Clear naming, comprehensive documentation, consistent patterns

### 2.3 Repository Structure

```
/alpaca
├── README.md
├── LICENSE
├── CMakeLists.txt
├── .gitignore
├── .github/
│   └── workflows/
│       ├── build-macos.yml
│       └── test.yml
├── /plugin
│   ├── /Source
│   │   ├── PluginProcessor.h/.cpp
│   │   ├── PluginEditor.h/.cpp
│   │   ├── Parameters.h/.cpp
│   │   └── StateSerialization.h/.cpp
│   └── /Resources
│       ├── alpaca_logo.png
│       ├── alpaca_character.svg
│       └── presets/
├── /dsp
│   ├── VoiceEngine.h/.cpp
│   ├── MonoNoteManager.h/.cpp
│   ├── Smoothers.h/.cpp
│   ├── /synth
│   │   ├── Excitation.h/.cpp
│   │   ├── FormantBank.h/.cpp
│   │   ├── Biquad.h/.cpp
│   │   ├── VowelTables.h/.cpp
│   │   ├── TiltEQ.h/.cpp
│   │   └── AmpEnvelope.h/.cpp
│   └── /fx
│       ├── Ensemble.h/.cpp
│       ├── StereoDelay.h/.cpp
│       └── ReverbFDN.h/.cpp
├── /midi
│   ├── MidiRouter.h/.cpp
│   ├── MidiLearn.h/.cpp
│   ├── MidiMapModel.h/.cpp
│   └── NRPNDecoder.h/.cpp
├── /ui
│   ├── Theme.h/.cpp
│   ├── Layout.h/.cpp
│   └── /Components
│       ├── HeaderBar.h/.cpp
│       ├── AlpacaView.h/.cpp
│       ├── XYPad.h/.cpp
│       ├── Knob.h/.cpp
│       ├── Panel.h/.cpp
│       ├── TonePanel.h/.cpp
│       ├── DelayPanel.h/.cpp
│       ├── ReverbPanel.h/.cpp
│       ├── EnsemblePanel.h/.cpp
│       └── MidiLearnOverlay.h/.cpp
├── /tests
│   ├── /unit
│   │   ├── TestFormantBank.cpp
│   │   ├── TestBiquad.cpp
│   │   ├── TestVowelTables.cpp
│   │   └── TestMidiRouter.cpp
│   ├── /integration
│   │   └── TestPluginLifecycle.cpp
│   └── /validation
│       ├── RenderHarness.cpp
│       └── GoldenFiles/
├── /docs
│   ├── UserGuide.md
│   ├── DeveloperGuide.md
│   ├── DSP_Documentation.md
│   └── MIDI_Implementation.md
└── /scripts
    ├── build.sh
    ├── test.sh
    └── package.sh
```

---

## 3. Development Phases

### Phase 0: Project Setup (Week 1, Days 1-3)

**Objectives:**
- Set up development environment
- Create JUCE project structure
- Configure build system
- Set up version control and CI
- Define all plugin parameters

**Tasks:**
1. Install JUCE framework and dependencies
2. Create plugin project with Projucer or CMake
3. Configure VST3 plugin format
4. Set up GitHub repository
5. Configure GitHub Actions for macOS builds
6. Define parameter inventory (IDs, ranges, defaults)
7. Implement basic AudioProcessorValueTreeState
8. Verify empty plugin compiles and loads in test DAW

**Deliverables:**
- Buildable, loadable empty plugin
- CI pipeline running
- Parameter definitions documented
- Repository structure established

**Acceptance Criteria:**
- Plugin loads in Ableton Live without errors
- All parameters appear in host automation list
- CI builds succeed on macOS

---

### Phase 1: Monophonic Voice Engine (Week 1-2, Days 4-7)

**Objectives:**
- Implement core MIDI handling
- Create monophonic voice allocation
- Add pitch processing (portamento, vibrato, pitch bend)
- Generate stable tone with basic excitation

**Tasks:**

**1.1 MIDI Processing Foundation:**
- Implement MidiRouter basic structure
- Parse MIDI note on/off messages
- Handle pitch bend
- Process CC messages (store for later use)
- Route MIDI events to voice engine

**1.2 Monophonic Note Management:**
- Implement MonoNoteManager class
- Last-note priority logic
- Note on/off handling with velocity
- Legato detection
- Convert MIDI note to frequency (A440 standard)

**1.3 Pitch Control:**
- Exponential portamento/glide smoothing
- Vibrato LFO (sine wave)
- Pitch bend range (±2 semitones default, configurable)
- Pitch modulation application
- Parameter smoothing for glide time

**1.4 Basic Excitation:**
- Implement bandlimited sawtooth (PolyBLEP) or impulse train
- Phase accumulation with proper wrapping
- Frequency tracking
- Output scaling

**1.5 Envelope:**
- Simple AR (attack/release) envelope
- Gate-based triggering
- Exponential curves
- Apply to excitation output

**Testing:**
- MIDI note input produces audible tone
- Pitch tracking is accurate
- Portamento glides smoothly
- Vibrato modulates pitch correctly
- No clicking or popping on note on/off

**Deliverables:**
- Functional monophonic synthesizer
- Responds to MIDI input
- Smooth pitch control
- Basic amplitude envelope

**Acceptance Criteria:**
- Play a melody via MIDI and hear correct pitches
- Glide parameter creates smooth pitch transitions
- Vibrato is audible and controllable
- No audio artifacts or glitches

---

### Phase 2: Formant Synthesis Engine (Week 2, Days 8-12)

**Objectives:**
- Implement formant-based vocal synthesis
- Create vowel tables and morphing
- Achieve recognizable vocal character
- Implement formant shift and voice character controls

**Tasks:**

**2.1 Vowel Formant Tables:**
- Research and implement formant frequency data for 5 vowels:
  - OO (as in "boot")
  - OH (as in "boat")
  - AH (as in "bot")
  - EH (as in "bet")
  - EE (as in "beat")
- Define 3-5 formants per vowel (F1, F2, F3, F4, F5)
- Store frequency, bandwidth, and amplitude for each formant
- Create VowelTables class with interpolation methods

**2.2 Formant Filter Bank:**
- Implement parallel bandpass filter bank
- Use biquad filters for each formant
- Calculate filter coefficients from F, BW, Q
- Update coefficients at control rate (not per-sample)
- Sum filter outputs with appropriate gains
- Implement denormal prevention

**2.3 Vowel Morphing:**
- Linear interpolation between vowel targets
- Smooth parameter changes (one-pole filter)
- Map vowel parameter (0..1) to vowel positions
- Real-time coefficient updates
- Zipper noise prevention

**2.4 Voice Character Controls:**
- Formant shift (scales all formant frequencies)
- Voice size/tract size control
- Spectral tilt/brightness filter
- Breath noise component (optional filtered noise)

**2.5 Integration:**
- Connect excitation to formant bank
- Apply envelopes
- Mix breath noise if enabled
- Normalize output levels

**Testing:**
- Sweep vowel parameter and hear distinct vowel transitions
- Formants sound natural and recognizable
- No stability issues or resonance explosions
- Formant shift changes voice character predictably

**Deliverables:**
- Formant synthesis engine
- Recognizable vowel sounds
- Smooth vowel morphing
- Voice character controls

**Acceptance Criteria:**
- Vowel transitions are smooth and continuous
- Each vowel position is recognizable
- Formant shift creates male/female/child character changes
- CPU usage is reasonable (<5% single instance)

---

### Phase 3: Effects Chain Implementation (Week 3, Days 13-17)

**Objectives:**
- Implement ensemble effect
- Create flexible delay (mono/stereo/ping-pong)
- Build algorithmic reverb
- Proper signal routing and parameter control

**Tasks:**

**3.1 Ensemble Effect:**
- Micro-detune/short delay algorithm
- Stereo width control
- LFO modulation for chorus-like movement
- Subtle by default (not cheesy chorus)
- Enable/disable with smooth bypass

**3.2 Stereo Delay:**
- Circular buffer implementation (pre-allocated)
- Three modes: mono, stereo, ping-pong
- Tempo sync with musical divisions (1/4, 1/8, 1/16, triplets, dotted)
- Feedback with clamping (<0.98 for stability)
- Damping filter in feedback loop (low-pass)
- Wet/dry mix control
- Smooth delay time changes (avoid clicks)

**3.3 Reverb:**
- Implement FDN (Feedback Delay Network) or Schroeder reverb
- Four types: hall, cathedral, plate, shimmer
- Parameters: size, decay time, pre-delay, damping, wet/dry
- Shimmer mode: pitch shifter in feedback (+12 semitones)
- Stability testing at extreme settings
- Stereo output

**3.4 Signal Chain Integration:**
- Correct processing order: Synth → Tilt → Ensemble → Delay → Reverb → Output
- Effect enable/disable with smooth transitions
- Parallel/series routing as appropriate
- Output gain stage with soft clipping
- Master wet/dry balance (optional)

**Testing:**
- Each effect sounds good in isolation
- Effects chain sounds good combined
- No feedback instability
- Tempo sync works correctly with host
- All effect modes function properly

**Deliverables:**
- Fully functional FX chain
- Multiple delay modes
- Multiple reverb types
- Smooth parameter control

**Acceptance Criteria:**
- Ensemble adds subtle width without overwhelming sound
- Delay creates clear echoes with musical timing
- Ping-pong delay bounces between left and right
- Reverb creates believable space
- Shimmer reverb produces upward-shifting echoes
- No artifacts when changing effect parameters
- Tempo sync locks to DAW BPM

---

### Phase 4: MIDI Mapping & Learn System (Week 3-4, Days 18-22)

**Objectives:**
- Implement comprehensive MIDI mapping system
- Add MIDI learn functionality
- Support high-resolution NRPN
- Create default CC mappings
- Ensure compatibility with control surfaces

**Tasks:**

**4.1 MIDI Mapping Infrastructure:**
- Create MidiMapModel class
- Define mapping structure: Source → Target → Transform
- Support multiple MIDI source types:
  - CC (7-bit)
  - NRPN (14-bit)
  - Pitch Bend
  - Channel Pressure
  - Poly Aftertouch
- Implement transform options:
  - Min/max range scaling
  - Inversion
  - Curve types (linear, exponential, logarithmic)
  - Smoothing time
  - Pickup mode (jump/soft takeover)

**4.2 NRPN Decoder:**
- Implement NRPN message parsing (CC99/98/6/38)
- Maintain NRPN state machine
- Convert 14-bit values to normalized parameters
- Define NRPN address map for plugin parameters

**4.3 MIDI Learn System:**
- Implement learn mode toggle
- Arm target parameter for learning
- Capture next incoming MIDI message
- Create mapping with sensible defaults
- Store mapping in plugin state
- Visual feedback for learned mappings

**4.4 Default CC Mappings:**
- Map common CCs to key parameters:
  - CC1 (Mod Wheel) → Vibrato Depth
  - CC5 → Glide Time
  - CC74 → Vowel
  - CC12 → Delay Wet
  - CC13 → Voice Character
  - Pitch Bend → Vowel (alternative)
- User can override via MIDI learn

**4.5 Thread Safety:**
- Lock-free mapping snapshot pattern
- Atomic pointer swap for mapping updates
- Audio thread reads immutable mapping snapshot
- UI thread updates mappings safely

**4.6 State Persistence:**
- Serialize MIDI mappings to plugin state
- Store in ValueTree under "midi_maps" node
- Restore mappings on plugin load
- Handle legacy state gracefully

**Testing:**
- MIDI learn captures various message types
- Mappings control parameters correctly
- NRPN provides smooth, high-resolution control
- Default mappings work out of the box
- Mappings survive save/load cycle
- No thread safety issues or crashes

**Deliverables:**
- Complete MIDI mapping system
- MIDI learn functionality
- NRPN support
- Default CC mappings
- Mapping persistence

**Acceptance Criteria:**
- User can map any MIDI CC to any parameter via learn mode
- NRPN messages provide 14-bit resolution for smooth sweeps
- Default mappings work with common MIDI controllers
- All mappings are saved with plugin state
- Control surfaces (e.g., MIDI Fighter Twister) work correctly
- Soft takeover prevents parameter jumps

---

### Phase 5: User Interface Implementation (Week 4-5, Days 23-30)

**Objectives:**
- Create professional, visually appealing UI
- Implement alpaca character animation
- Build XY pad for vowel control
- Create collapsible FX panels
- Add MIDI learn visual feedback
- Ensure scalable, high-DPI rendering

**Tasks:**

**5.1 UI Architecture Setup:**
- Define Theme.h with colors, fonts, styles
- Create Layout.h with base rectangles and scaling
- Implement base Panel component with collapsible behavior
- Set up APVTS attachments pattern

**5.2 Header Bar:**
- Preset management UI (dropdown/browser)
- A/B comparison buttons
- MIDI Learn toggle button
- Settings menu
- CPU meter display
- Plugin name/logo

**5.3 Alpaca Character View:**
- Design original alpaca character (legally distinct)
- Implement mouth animation synchronized with vowel
- Visual feedback for amplitude (mouth opening)
- Subtle head movement for pitch/vibrato
- 30-60 fps animation timer
- Thread-safe telemetry from audio thread

**5.4 XY Pad:**
- Touch-drag interaction
- Y-axis → Vowel parameter (0..1)
- X-axis → Pitch offset (optional, configurable)
- Visual crosshair and trail
- Vowel labels on Y-axis (OO, OH, AH, EH, EE)
- Gesture handling (begin/end parameter changes)
- Right-click context menu for MIDI learn
- Smooth rendering with reduced CPU usage

**5.5 Core Knob Row:**
- Custom Knob component
- Arc-style value indicator
- Label below knob
- Double-click to reset
- Shift-drag for fine control
- Right-click context menu (learn, clear, range)
- Visual indicator for MIDI mapping
- Implement for: Glide, Voice Character, Vibrato Depth/Rate

**5.6 Effect Panels:**
- Collapsible panel base class
- Tone Panel: Formant Shift, Brightness, Breath Noise
- Ensemble Panel: Amount, Width, Detune, Mode
- Delay Panel: Mode, Time/Sync, Feedback, Damping, Wet
- Reverb Panel: Type, Size, Decay, Pre-delay, Damping, Wet
- Consistent layout and spacing
- Enable/disable toggle per effect

**5.7 MIDI Learn Overlay:**
- Semi-transparent overlay when in learn mode
- "Learning..." status message
- Last received MIDI message display
- Highlight target control
- List of current mappings (optional expandable)

**5.8 Scaling & DPI:**
- Implement UI scale factor (0.75x to 1.5x)
- Detect display DPI and scale appropriately
- Test on retina/high-DPI displays
- All graphics render crisp at any scale
- Text remains readable

**5.9 Polish:**
- Smooth animations for panel collapse
- Hover states for interactive elements
- Tooltips for advanced controls
- Keyboard shortcuts (tab navigation optional)
- Consistent spacing and alignment

**Testing:**
- UI looks good at various scales
- All controls are responsive and smooth
- Character animation is entertaining and informative
- XY pad feels natural and precise
- MIDI learn provides clear visual feedback
- No UI lag or stuttering during playback
- UI thread does not impact audio thread

**Deliverables:**
- Complete, professional UI
- Animated alpaca character
- Fully functional XY pad
- All parameter controls
- MIDI learn visual system
- Scalable, high-DPI rendering

**Acceptance Criteria:**
- UI looks professional and polished
- Alpaca character animates in sync with sound
- XY pad provides smooth, intuitive vowel control
- All parameters are accessible and clearly labeled
- MIDI learn mode is easy to use and understand
- UI scales cleanly from 0.75x to 1.5x
- No visual glitches or artifacts
- UI is responsive even under heavy CPU load

---

### Phase 6: Optimization & Performance (Week 5-6, Days 31-35)

**Objectives:**
- Profile CPU usage and optimize hot paths
- Eliminate audio thread allocations
- Ensure real-time safety
- Test across sample rates
- Stress test with automation and MIDI

**Tasks:**

**6.1 Performance Profiling:**
- Use Instruments (macOS) or similar profiler
- Identify CPU hotspots in audio thread
- Measure CPU usage with multiple instances
- Profile at different sample rates (44.1k, 48k, 96k)
- Test with complex automation scenarios

**6.2 DSP Optimization:**
- Optimize formant bank filter calculations
- Use SIMD where appropriate (JUCE dsp module)
- Minimize coefficient recalculation
- Optimize parameter smoothing
- Denormal prevention strategies
- Reduce branching in audio thread

**6.3 Memory Optimization:**
- Pre-allocate all buffers in prepare()
- Eliminate dynamic allocations in processBlock()
- Use fixed-size containers where possible
- Profile memory usage
- Verify no allocations with AddressSanitizer

**6.4 Real-Time Safety Audit:**
- Review all audio thread code paths
- Remove any locks in audio thread
- Verify no file I/O or system calls
- Check for hidden allocations (std::function, etc.)
- Use lock-free structures for cross-thread communication
- Verify with real-time safety tools

**6.5 Sample Rate Testing:**
- Test at 44.1 kHz, 48 kHz, 88.2 kHz, 96 kHz
- Verify filter stability at all rates
- Check oversampling if implemented
- Ensure no aliasing or digital artifacts
- Validate buffer size handling (64 to 2048 samples)

**6.6 Stress Testing:**
- Multiple plugin instances in a project
- Rapid parameter automation
- Dense MIDI input
- Long sustained notes
- Extreme parameter values
- Parameter changes while processing

**Testing:**
- CPU usage under 5% per instance on modern laptop
- No allocations in audio thread (verified with tools)
- Stable at all tested sample rates
- No glitches under stress conditions
- Meets real-time deadlines consistently

**Deliverables:**
- Optimized DSP code
- Verified real-time safety
- Performance documentation
- Optimization report

**Acceptance Criteria:**
- CPU usage <5% per instance at 48 kHz
- 10+ instances run simultaneously without issues
- No allocations detected in audio thread
- Stable at 44.1k through 96k sample rates
- Fast automation does not cause glitches
- No denormal performance penalties

---

### Phase 7: Testing & Quality Assurance (Week 6, Days 36-40)

**Objectives:**
- Comprehensive functional testing
- DAW compatibility validation
- Bug fixing and polish
- User testing (if possible)
- Documentation verification

**Tasks:**

**7.1 Functional Testing:**
- Test all parameters and their ranges
- Verify all MIDI CC mappings
- Test MIDI learn workflow
- Verify preset save/load
- Test plugin state persistence
- Validate automation recording/playback
- Test bypass functionality
- Verify multi-channel support (if applicable)

**7.2 DAW Compatibility Testing:**
- **Primary: Ableton Live** (11 and 12 if available)
  - Load plugin
  - Test all features
  - Record automation
  - Save and reload projects
  - Test in freeze/flatten workflow
- **Secondary: Logic Pro**
  - Basic functionality test
  - AU format if implemented
- **Tertiary: Reaper, Studio One, Cubase** (if time permits)
  - Basic load and functionality test

**7.3 Edge Case Testing:**
- Extreme parameter values
- Rapid parameter changes
- Very short/long notes
- High note density MIDI input
- Buffer size changes
- Sample rate changes mid-session
- Plugin GUI closed during operation
- Multiple instances with different states

**7.4 Audio Quality Testing:**
- Null test against reference (if available)
- Listen for artifacts or glitches
- Frequency response analysis
- THD+N measurement
- Test output clipping protection
- Validate stereo imaging
- Check phase relationships

**7.5 Bug Fixing:**
- Triage discovered issues
- Fix critical bugs (crashes, audio glitches)
- Fix high-priority bugs (incorrect behavior)
- Document known issues for future resolution
- Regression testing after fixes

**7.6 User Acceptance Testing:**
- If possible, get feedback from beta users
- Collect usability feedback
- Identify confusing UI elements
- Validate documentation clarity
- Incorporate feedback where feasible

**Testing:**
- All test cases pass
- No critical or high-priority bugs remain
- Plugin works in all tested DAWs
- Audio quality meets expectations
- User feedback is positive

**Deliverables:**
- Test results documentation
- Bug fixes implemented
- Known issues documented
- Compatibility matrix

**Acceptance Criteria:**
- Plugin loads and runs in Ableton Live without errors
- All parameters function correctly
- MIDI control works as expected
- No crashes or hangs in normal use
- Audio output is clean and artifact-free
- State persistence is reliable
- Performance meets targets

---

### Phase 8: Build System & Packaging (Week 7, Days 41-43)

**Objectives:**
- Automate build process
- Create installer package
- Code signing and notarization (macOS)
- Prepare distribution files
- Version management

**Tasks:**

**8.1 Build Automation:**
- Finalize CMake build configuration
- Create build scripts (build.sh)
- Configure Release build optimization flags
- Set up version numbering system
- Automate asset inclusion

**8.2 macOS Packaging:**
- Create VST3 bundle correctly structured
- Include all necessary resources
- Set bundle identifier
- Set version and build numbers
- Create Info.plist with correct metadata

**8.3 Code Signing:**
- Obtain Apple Developer ID certificate
- Sign VST3 bundle
- Sign installer package
- Verify signature with codesign tool

**8.4 Notarization:**
- Submit package to Apple for notarization
- Handle notarization responses
- Staple notarization ticket
- Verify notarization with spctl tool

**8.5 Installer Creation:**
- Create PKG installer (macOS)
- Set install location (/Library/Audio/Plug-Ins/VST3)
- Include readme and license
- Add uninstaller (optional)
- Test installer on clean system

**8.6 Distribution Package:**
- Create disk image (DMG) or ZIP
- Include:
  - VST3 plugin
  - User manual (PDF)
  - Readme
  - License
  - Preset library
  - Demo project (optional)
- Version naming convention

**8.7 CI/CD Integration:**
- GitHub Actions workflow for release builds
- Automated builds on tagged commits
- Upload artifacts to GitHub Releases
- Build numbering from git tags

**Testing:**
- Installer works on clean macOS system
- Plugin installs to correct location
- DAWs discover and load plugin correctly
- Code signing and notarization verified
- Version info displayed correctly

**Deliverables:**
- Signed and notarized VST3 bundle
- Installer package
- Distribution DMG/ZIP
- Automated build pipeline

**Acceptance Criteria:**
- Plugin installs correctly via installer
- macOS Gatekeeper allows execution without warnings
- Plugin appears in Ableton Live plugin list
- All resources load correctly
- Version and copyright info correct

---

### Phase 9: Documentation & Release (Week 7-8, Days 44-48)

**Objectives:**
- Complete user documentation
- Create developer documentation
- Prepare marketing materials
- Release version 1.0
- Establish feedback channels

**Tasks:**

**9.1 User Documentation:**
- User Guide:
  - Installation instructions
  - Quick start guide
  - Feature overview
  - Parameter reference
  - MIDI implementation details
  - Troubleshooting guide
  - FAQ
- Format: PDF and web (markdown)
- Screenshots and examples
- Video tutorials (optional)

**9.2 Developer Documentation:**
- Architecture overview
- Build instructions
- Testing procedures
- Contributing guidelines
- Code style guide
- API documentation (Doxygen)
- Roadmap for future development

**9.3 MIDI Implementation Chart:**
- Standard MIDI implementation chart format
- List all CC mappings
- NRPN address map
- Special message handling
- Compatibility notes

**9.4 Preset Library:**
- Create 20-30 presets covering:
  - Classic vowel sounds
  - Expressive performance presets
  - Effect showcases
  - Pads and atmospheres
  - Rhythmic/sequenced patches
- Organize by category
- Include preset descriptions

**9.5 Release Preparation:**
- Create GitHub release
- Write release notes
- Tag version 1.0.0
- Upload distribution packages
- Update website (if applicable)

**9.6 Communication:**
- Announce on relevant forums (KVR, VI-Control, etc.)
- Social media announcement
- Email announcement (if mailing list exists)
- Create demo videos/audio
- Reach out to reviewers/YouTubers

**9.7 Feedback Channels:**
- Set up GitHub Issues for bug reports
- Create discussion forum or Discord
- Monitor feedback and respond
- Plan point releases for bug fixes

**Testing:**
- Documentation is clear and accurate
- All links work
- Screenshots match current version
- Presets load and sound good

**Deliverables:**
- Complete user documentation
- Developer documentation
- Preset library
- Release package
- Marketing materials

**Acceptance Criteria:**
- User can install and use plugin following documentation
- All features are documented
- Developer can build from source following guide
- Presets demonstrate plugin capabilities
- Release is publicly available

---

## 4. Feature Specifications

### 4.1 Synthesis Engine

**Voice Architecture:**
- Monophonic voice allocation
- Last-note priority (configurable to low/high note in future)
- Legato mode detection
- MIDI velocity response
- Pitch bend support (±2 semitones, configurable)

**Formant Synthesis:**
- 3-5 formants per vowel
- Parallel bandpass filter bank (biquad)
- Five vowel positions: OO, OH, AH, EH, EE
- Continuous morphing via linear interpolation
- Formant frequency tables based on research
- Per-formant amplitude and bandwidth control

**Pitch Processing:**
- MIDI note to frequency conversion (A440 standard)
- Exponential portamento/glide (0-2000ms)
- Vibrato with depth and rate controls
- Pitch bend modulation
- MPE-ready architecture (future)

**Tone Shaping:**
- Formant shift (±12 semitones)
- Voice character control (tract size)
- Spectral tilt/brightness filter
- Breath/noise component (optional)

**Envelopes:**
- Amplitude envelope: Attack, Release
- Filter envelope (future): for vowel animation

**Excitation:**
- Bandlimited sawtooth (PolyBLEP)
- Anti-aliased for all pitches

### 4.2 Effects Chain

**Ensemble:**
- Micro-detune/chorus effect
- Amount (0-100%)
- Width (mono to wide stereo)
- Detune amount (cents)
- Mode: mono/stereo/choir
- Enable/bypass toggle

**Delay:**
- Three modes: mono, stereo, ping-pong
- Delay time: 1-2000ms or tempo-synced
- Tempo sync divisions: 1/1, 1/2, 1/4, 1/8, 1/16, triplets, dotted
- Feedback: 0-95%
- Damping: high-frequency rolloff in feedback
- Wet/dry mix
- Enable/bypass toggle

**Reverb:**
- Four types: Hall, Cathedral, Plate, Shimmer
- Size: 0-100%
- Decay time: 0.1-20 seconds
- Pre-delay: 0-250ms
- Damping: high-frequency absorption
- Wet/dry mix
- Enable/bypass toggle

**Signal Chain:**
Synth → Tilt EQ → Ensemble → Delay → Reverb → Output Gain

### 4.3 MIDI Implementation

**MIDI Input:**
- Note On/Off (all velocities)
- Pitch Bend
- Channel Pressure (Aftertouch)
- CC messages (all 128 controllers)
- NRPN (14-bit high-resolution)

**Default CC Mappings:**
- CC1 (Mod Wheel) → Vibrato Depth
- CC5 → Glide Time
- CC12 → Delay Wet
- CC13 → Voice Character
- CC74 → Vowel (alternative to pitch bend)
- Pitch Bend → Vowel (default)

**MIDI Learn:**
- Learn mode toggle
- Arm any parameter for learning
- Capture any MIDI source type
- Transform options: range, invert, curve, smoothing
- Soft takeover/pickup mode
- Mapping persistence in plugin state

**NRPN Address Map:**
- MSB = 7 (plugin bank)
- LSB = parameter number
- 14-bit resolution for smooth control
- Predefined mappings for key parameters

**MIDI Channel:**
- Omni mode or specific channel (1-16)
- Configurable in settings

### 4.4 User Interface

**Layout:**
- Base size: 1200×720 pixels
- Scalable UI: 0.75x to 1.5x
- High-DPI rendering

**Sections:**
1. Header Bar
   - Preset browser
   - A/B comparison
   - MIDI Learn toggle
   - Settings menu
   - CPU meter

2. Performance Column (Left)
   - Alpaca character animation
   - XY Pad (vowel Y-axis)
   - Core knobs: Glide, Voice, Vibrato

3. FX Column (Right)
   - Tone Panel (collapsible)
   - Ensemble Panel (collapsible)
   - Delay Panel (collapsible)
   - Reverb Panel (collapsible)

**Visual Design:**
- Modern flat design
- Minimal skeuomorphism
- Original alpaca character artwork
- Smooth animations
- Clear visual hierarchy
- Consistent spacing (8px grid)

**Interaction:**
- Knob: drag to adjust, double-click reset, shift-drag fine
- XY Pad: drag for continuous control
- Right-click context menus for MIDI learn
- Keyboard shortcuts (undo/redo)

**Feedback:**
- Alpaca mouth syncs with vowel
- MIDI mapped controls show indicator
- Learning mode highlights target
- Tooltips for advanced parameters

### 4.5 Parameters (Complete List)

**Performance:**
- Vowel (0..1, default 0.35)
- Glide Time (0..2000ms, default 50ms)
- Voice Character (0..1, default 0.5)
- Vibrato Depth (0..1, default 0.0)
- Vibrato Rate (0.1..12 Hz, default 5.5Hz)

**Envelope:**
- Amp Attack (0..200ms, default 5ms)
- Amp Release (5..2000ms, default 120ms)

**Tone:**
- Formant Shift (-12..+12 semitones, default 0)
- Brightness/Tilt (0..1, default 0.55)
- Breath Noise (0..1, default 0.0)

**Ensemble:**
- Enable (on/off, default on)
- Amount (0..1, default 0.15)
- Width (0..1, default 0.65)
- Detune (0..20 cents, default 6)
- Mode (mono/stereo/choir, default stereo)

**Delay:**
- Enable (on/off, default on)
- Mode (mono/stereo/ping-pong, default ping-pong)
- Sync (on/off, default off)
- Time (1..2000ms, default 320ms)
- Division (1/1..1/16D, default 1/8)
- Feedback (0..0.95, default 0.35)
- Damping (0..1, default 0.35)
- Wet (0..1, default 0.25)

**Reverb:**
- Enable (on/off, default on)
- Type (hall/cathedral/plate/shimmer, default hall)
- Size (0..1, default 0.60)
- Decay (0.1..20s, default 4.5s)
- Pre-delay (0..250ms, default 18ms)
- Damping (0..1, default 0.40)
- Wet (0..1, default 0.22)

**Global:**
- Output Gain (-24..+12 dB, default 0dB)
- MIDI Learn Enable (on/off, default off)
- MIDI Write Automation (on/off, default off)
- MIDI Channel (omni/1-16, default omni)

**Total: ~35 parameters, all automatable and MIDI-mappable**

---

## 5. Quality Assurance

### 5.1 Testing Strategy

**Unit Testing:**
- DSP algorithms (formant bank, filters, envelopes)
- MIDI parsing and routing
- Parameter mapping and transformation
- Vowel interpolation
- Effect algorithms

**Integration Testing:**
- Plugin lifecycle (load/unload)
- State save/restore
- MIDI mapping persistence
- Parameter automation
- Multi-instance operation

**Manual Testing:**
- Functional testing of all features
- DAW compatibility testing
- UI/UX testing
- Audio quality validation
- Performance testing

**Automated Testing:**
- CI pipeline runs unit tests on every commit
- Memory leak detection (AddressSanitizer)
- Thread safety validation
- Performance regression tests

### 5.2 Quality Metrics

**Performance:**
- CPU usage <5% per instance (48kHz, 512 samples, M1 Mac)
- No audio thread allocations
- No locks in audio thread
- Stable at all sample rates (44.1k-96k)

**Reliability:**
- No crashes in normal operation
- Graceful handling of edge cases
- Stable with extreme parameter values
- Robust state persistence

**Compatibility:**
- Works in Ableton Live 11+ (primary target)
- Works in Logic Pro X (secondary)
- VST3 specification compliant
- macOS 10.13+ (or latest 3 versions)

**Audio Quality:**
- No audible artifacts under normal use
- Smooth parameter changes (no zipper noise)
- Clean audio path (low noise floor)
- Proper headroom and clipping protection

**Usability:**
- Intuitive interface
- Clear parameter labeling
- Helpful tooltips
- Responsive controls
- Good visual feedback

### 5.3 Bug Tracking

**Priority Levels:**
- **Critical:** Crashes, data loss, audio corruption
- **High:** Major feature broken, severe audio issues
- **Medium:** Minor feature issue, usability problem
- **Low:** Cosmetic issue, enhancement request

**Workflow:**
1. Bug reported via GitHub Issues
2. Triage and prioritize
3. Assign to milestone
4. Fix and test
5. Verify fixed in next build
6. Close issue

**Bug Fix Policy:**
- Critical bugs: fix immediately
- High priority: fix in next point release
- Medium priority: fix when feasible
- Low priority: backlog for future

---

## 6. Build & Release

### 6.1 Version Numbering

**Semantic Versioning:** MAJOR.MINOR.PATCH
- MAJOR: Breaking changes, major new features
- MINOR: New features, backward compatible
- PATCH: Bug fixes, no new features

**Version 1.0.0 Goals:**
- All core features implemented
- Stable and tested
- Ready for public release

**Future Versions:**
- 1.0.x: Bug fixes
- 1.1.0: Minor features (AU format, additional presets)
- 1.2.0: Polyphony, modulation matrix
- 2.0.0: Major overhaul or breaking changes

### 6.2 Build Process

**Development Builds:**
- Frequent builds during development
- Debug symbols included
- Logging enabled
- Build number from git commit hash

**Release Builds:**
- Optimized for performance
- No debug symbols or logging
- Signed and notarized
- Versioned build number

**Build Automation:**
- GitHub Actions workflows
- Triggered by git tags (v1.0.0)
- Builds on macOS runner
- Runs tests before packaging
- Uploads artifacts to release

### 6.3 Distribution

**Release Package Contents:**
- Alpaca-1.0.0-macOS.dmg containing:
  - Alpaca.vst3 (signed and notarized)
  - Alpaca_User_Guide.pdf
  - README.txt
  - LICENSE.txt
  - Presets folder
  - Demo_Project folder (optional)

**Distribution Channels:**
- GitHub Releases (primary)
- Personal website (if available)
- Plugin marketplaces (KVR, Plugin Boutique, etc.)
- Direct distribution

**Licensing:**
- Choose appropriate license (e.g., proprietary, GPL, MIT)
- Include license file
- Respect JUCE licensing (GPL or paid license)

### 6.4 Update Mechanism

**Version 1.0:**
- Manual updates (user downloads new version)
- Release notes explain what's new
- Backward compatible state files

**Future:**
- Automatic update check (optional)
- In-app update notification
- Changelog viewer

---

## 7. Documentation

### 7.1 User Documentation

**User Guide Contents:**
1. Introduction
   - What is Alpaca?
   - Key features
   - System requirements
2. Installation
   - macOS installation steps
   - Verifying installation
3. Quick Start
   - Loading the plugin
   - Playing your first sound
   - Exploring presets
4. Interface Overview
   - Main sections
   - Navigation
5. Sound Design
   - Vowel morphing
   - Voice character
   - Vibrato and expression
6. Effects
   - Ensemble
   - Delay
   - Reverb
7. MIDI Control
   - Default mappings
   - MIDI learn
   - Control surfaces
8. Presets
   - Loading and saving
   - Preset browser
9. Automation
   - Recording automation
   - Editing automation
10. Tips and Tricks
11. Troubleshooting
12. FAQ
13. MIDI Implementation Chart
14. Credits and License

**Format:** PDF with bookmarks, web version (HTML/markdown)

### 7.2 Developer Documentation

**Developer Guide Contents:**
1. Architecture Overview
2. Building from Source
   - Prerequisites
   - Clone and build
   - IDE setup
3. Testing
   - Running tests
   - Writing new tests
4. Contributing
   - Code style
   - Pull request process
5. DSP Documentation
   - Formant synthesis algorithm
   - Filter design
   - Effect algorithms
6. MIDI Implementation
   - Routing architecture
   - NRPN implementation
7. UI Architecture
   - Component tree
   - Layout system
   - Parameter binding
8. Roadmap
9. Known Issues

**Format:** Markdown in repository, HTML via GitHub Pages

### 7.3 Code Documentation

- Doxygen comments for all public APIs
- README files in each major directory
- Inline comments for complex algorithms
- Design rationale documented where non-obvious

---

## 8. Success Criteria

### 8.1 Technical Success Criteria

✓ Plugin compiles and builds without errors or warnings
✓ Plugin loads in Ableton Live without errors
✓ All 35+ parameters are automatable via host
✓ MIDI notes produce sound with correct pitch
✓ Vowel morphing is smooth and recognizable
✓ All effects function correctly
✓ MIDI learn system works reliably
✓ State save/restore is reliable
✓ CPU usage <5% per instance
✓ No audio thread allocations
✓ Stable at 44.1k, 48k, 96k sample rates
✓ No crashes in normal operation
✓ Plugin is signed and notarized for macOS
✓ All unit tests pass
✓ All integration tests pass

### 8.2 User Experience Success Criteria

✓ UI is intuitive and easy to navigate
✓ Alpaca character animation is engaging
✓ XY pad provides natural vowel control
✓ MIDI learn is discoverable and easy to use
✓ Presets load instantly and sound good
✓ Documentation is clear and comprehensive
✓ Installation is straightforward
✓ Plugin responds to control immediately (low latency)

### 8.3 Audio Quality Success Criteria

✓ Vowel sounds are recognizable and musical
✓ No audible artifacts (clicks, pops, aliasing)
✓ Parameter changes are smooth (no zipper noise)
✓ Effects sound professional and musical
✓ Output is clean with good headroom
✓ Stereo imaging is proper and balanced

### 8.4 Compatibility Success Criteria

✓ Works in Ableton Live 11 and 12
✓ Works in Logic Pro X (bonus)
✓ Compatible with macOS 10.13+
✓ Works with common MIDI controllers
✓ VST3 specification compliant

### 8.5 Release Success Criteria

✓ Version 1.0.0 released publicly
✓ Available on GitHub Releases
✓ User documentation complete
✓ Demo videos/audio available
✓ Announced on relevant forums
✓ Positive initial user feedback

---

## 9. Risk Management

### 9.1 Technical Risks

**Risk: Formant synthesis doesn't sound good**
- Mitigation: Research formant tables extensively, reference academic papers
- Mitigation: Allow manual formant table editing for fine-tuning
- Contingency: Implement alternative FOF synthesis approach

**Risk: CPU usage too high**
- Mitigation: Profile early and often
- Mitigation: Optimize hot paths with SIMD
- Contingency: Reduce formant count or effect quality

**Risk: Real-time safety violations**
- Mitigation: Strict code review, use analyzer tools
- Mitigation: Lock-free patterns, pre-allocation
- Contingency: Refactor violating code

**Risk: DAW compatibility issues**
- Mitigation: Test in multiple DAWs frequently
- Mitigation: Follow VST3 specification strictly
- Contingency: Document known issues, work with DAW vendors

**Risk: JUCE licensing costs**
- Mitigation: Understand JUCE license options early
- Mitigation: Budget for JUCE Indie license if needed
- Contingency: Consider GPL release if appropriate

### 9.2 Schedule Risks

**Risk: Development takes longer than estimated**
- Mitigation: Break work into small, measurable tasks
- Mitigation: Track progress weekly, adjust scope if needed
- Contingency: Release v1.0 with reduced feature set, add features in v1.1

**Risk: Blocking bugs discovered late**
- Mitigation: Test continuously, not just at end
- Mitigation: Alpha testing with friendly users
- Contingency: Delay release, fix critical bugs first

**Risk: Key dependency issues (JUCE update, macOS changes)**
- Mitigation: Pin dependency versions
- Mitigation: Test on multiple OS versions
- Contingency: Maintain compatibility with older versions

### 9.3 Legal/IP Risks

**Risk: Copyright infringement claims (Delay Lama similarity)**
- Mitigation: Original artwork, no copied code or assets
- Mitigation: Different name, distinct visual identity
- Mitigation: Consult with legal expert if needed
- Contingency: Cease distribution, modify design

**Risk: JUCE licensing violation**
- Mitigation: Understand and comply with JUCE license
- Mitigation: Purchase commercial license if distributing non-GPL
- Contingency: Release under GPL or cease distribution

**Risk: Sample/preset copyright issues**
- Mitigation: Create all presets from scratch
- Mitigation: No sampled content, purely synthetic
- Contingency: Remove infringing content immediately

### 9.4 Quality Risks

**Risk: Insufficient testing leads to buggy release**
- Mitigation: Comprehensive test plan, manual and automated
- Mitigation: Beta testing period
- Contingency: Rapid point release with fixes

**Risk: Poor documentation leads to user confusion**
- Mitigation: Write documentation throughout development
- Mitigation: User feedback on documentation
- Contingency: Tutorial videos, FAQ updates

**Risk: Performance issues on older hardware**
- Mitigation: Test on range of hardware
- Mitigation: Quality/performance presets
- Contingency: Provide "low CPU" mode

---

## 10. Resources & References

### 10.1 Technical References

**Formant Synthesis:**
- "Speech Synthesis and Recognition" by John Holmes
- "Digital Processing of Speech Signals" by Rabiner & Schafer
- Formant frequency tables from phonetic research
- FOF synthesis papers (Rodet et al.)

**DSP Algorithms:**
- "Designing Audio Effect Plugins in C++" by Will Pirkle
- "The Audio Programming Book" by Boulanger & Lazzarini
- DAFX - Digital Audio Effects (Zölzer)
- musicdsp.org archive

**JUCE Framework:**
- JUCE documentation and tutorials
- JUCE forum
- "Audio Plugin Development" course materials
- Example plugins in JUCE repository

**VST3 Specification:**
- Steinberg VST3 SDK documentation
- VST3 Audio Plug-Ins SDK
- VST3 implementation examples

### 10.2 Tools & Software

**Development:**
- Xcode (macOS)
- CLion or VS Code
- CMake 3.15+
- JUCE 7.x
- Git

**Audio Analysis:**
- iZotope RX (spectral analysis)
- FabFilter Pro-Q3 (frequency analysis)
- Oscilloscope plugins
- Audio file editor (Audacity, Fission)

**Profiling:**
- Instruments (Xcode)
- Time Profiler
- Allocations instrument
- AddressSanitizer

**Testing:**
- Plugin validation tools (pluginval)
- DAWs for testing (Ableton, Logic)
- MIDI monitor tools
- Virtual MIDI controllers

### 10.3 Design Resources

**Visual Design:**
- Figma or Sketch for UI mockups
- Vector graphics editor (Illustrator, Affinity Designer)
- Icon libraries
- Color palette tools

**Character Design:**
- Original alpaca character artwork
- Animation references
- Sprite/asset creation tools

### 10.4 Community Resources

**Forums:**
- KVR Audio Developer Forum
- JUCE Forum
- VI-Control
- Reddit r/audioengineering, r/synthrecipes

**Learning:**
- YouTube channels: The Audio Programmer, Dan Worrall
- Online courses on DSP and plugin development
- Academic papers and theses

### 10.5 Support Files Reference

This project plan is supported by detailed specification files located in `/project-plan/support-files/`:

1. **DSP_Algorithm_Specification.md** - Detailed math and algorithms
2. **MIDI_Implementation_Detailed.md** - Complete MIDI specification
3. **UI_Design_Specification.md** - UI mockups and interaction details
4. **Parameter_Reference.md** - Complete parameter database
5. **Formant_Tables.md** - Vowel formant frequency data
6. **Testing_Plan.md** - Comprehensive testing procedures
7. **Build_Instructions.md** - Step-by-step build guide
8. **Code_Style_Guide.md** - Coding standards
9. **Architecture_Diagrams.md** - System architecture visuals
10. **Preset_Library_Plan.md** - Preset creation guidelines

---

## Appendices

### Appendix A: Glossary

**APVTS:** AudioProcessorValueTreeState - JUCE's parameter management system
**AU:** Audio Unit - Apple's audio plugin format
**Biquad:** Second-order IIR filter with two poles and two zeros
**CC:** Control Change - MIDI message type for continuous controllers
**DAW:** Digital Audio Workstation
**Formant:** Resonant frequency of the vocal tract
**FOF:** Formant Wave Function - granular formant synthesis technique
**JUCE:** Jules' Utility Class Extensions - C++ framework for audio applications
**MIDI:** Musical Instrument Digital Interface
**MPE:** MIDI Polyphonic Expression
**NRPN:** Non-Registered Parameter Number - 14-bit MIDI messages
**Portamento:** Glide or slide between pitches
**Real-time Safe:** Code that meets real-time deadlines without allocations or blocking
**VST3:** Virtual Studio Technology 3 - Steinberg's plugin format
**XY Pad:** Two-dimensional continuous controller

### Appendix B: Abbreviations

**BPF:** Band-Pass Filter
**CPU:** Central Processing Unit
**DSP:** Digital Signal Processing
**FDN:** Feedback Delay Network
**HPF:** High-Pass Filter
**LFO:** Low-Frequency Oscillator
**LPF:** Low-Pass Filter
**MIDI:** Musical Instrument Digital Interface
**UI:** User Interface
**UX:** User Experience
**VST:** Virtual Studio Technology

### Appendix C: Project Timeline Summary

| Phase | Duration | Milestone |
|-------|----------|-----------|
| 0. Project Setup | 3 days | Empty plugin loads |
| 1. Voice Engine | 4 days | Monophonic synth works |
| 2. Formant Synthesis | 5 days | Vocal character achieved |
| 3. Effects Chain | 5 days | FX fully functional |
| 4. MIDI Mapping | 5 days | MIDI learn complete |
| 5. User Interface | 8 days | UI fully implemented |
| 6. Optimization | 5 days | Performance targets met |
| 7. Testing & QA | 5 days | All tests pass |
| 8. Build & Package | 3 days | Signed installer ready |
| 9. Documentation | 5 days | Docs complete, released |
| **Total** | **48 days** | **~8 weeks** |

### Appendix D: Contact & Support

**Developer:** [Your Name/Organization]
**Email:** [support@alpacaplugin.com]
**Website:** [www.alpacaplugin.com]
**GitHub:** [github.com/yourname/alpaca]
**Discord:** [discord.gg/alpacaplugin]

**Bug Reports:** GitHub Issues
**Feature Requests:** GitHub Discussions
**General Support:** Discord community

---

## Document Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2026-03-02 | AI Agent | Initial comprehensive project plan |

---

**END OF MASTER PROJECT PLAN**

This document serves as the complete roadmap for developing the Alpaca VST3 plugin from conception to public release. All team members and AI agents should reference this plan throughout the development process.
