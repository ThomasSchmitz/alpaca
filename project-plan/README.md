# Alpaca VST3 Project Plan

**Welcome to the Alpaca project!**

This directory contains the complete, comprehensive plan for developing Alpaca - an enhanced VST3 clone of the classic Delay Lama plugin.

---

## What is Alpaca?

Alpaca is a **monophonic formant vocal synthesizer** VST3 plugin that captures the spirit of the beloved 2002 Delay Lama plugin while bringing it into the modern era with professional features, full MIDI control, and contemporary effects processing.

**Key Features:**
- Real-time vowel morphing through 5 vowel positions (OO, OH, AH, EH, EE)
- Animated alpaca character synchronized with sound
- Modern effects: Ensemble, Ping-Pong Delay, Multi-mode Reverb
- Comprehensive MIDI CC and NRPN (14-bit) mapping with learn functionality
- Professional, scalable UI
- Full DAW compatibility, especially Ableton Live
- External control surface support

---

## Project Status

**Current Phase:** Planning Complete ✅
**Next Phase:** Ready for Development Execution

---

## Directory Structure

```
/project-plan/
├── README.md (this file)
├── MASTER_PROJECT_PLAN.md          # Complete development plan
├── AI_AGENT_PROMPT.md              # Prompt for AI agent to execute project
└── /support-files/
    ├── DSP_Algorithm_Specification.md     # Detailed DSP math
    ├── Formant_Tables.md                  # Vowel formant frequency data
    ├── Parameter_Reference.md             # All parameter specifications
    └── [Additional spec files...]
```

---

## Core Documents

### 1. MASTER_PROJECT_PLAN.md
**The complete development plan.** This is your primary reference for:
- Project overview and goals
- Technical architecture
- Development phases (0-9)
- Feature specifications
- Quality assurance approach
- Build and release process
- Risk management

**Start here to understand the full scope of the project.**

### 2. AI_AGENT_PROMPT.md
**The execution prompt for an AI development agent.** This document:
- Defines the agent's role and mission
- Provides core directives and quality standards
- Explains the complete project context
- Lists all available resources
- Gives implementation guidelines
- Defines success criteria

**Use this to brief an AI agent (or yourself) on how to execute the plan.**

---

## Support Files

Located in `/support-files/`, these provide detailed specifications:

### Technical Specifications

#### DSP_Algorithm_Specification.md
Complete mathematical and algorithmic details for all DSP components:
- Pitch processing (MIDI to frequency, portamento, vibrato, pitch bend)
- Excitation generation (PolyBLEP bandlimited sawtooth)
- Formant synthesis (parallel bandpass filter bank)
- Vowel morphing algorithms
- Tone shaping (spectral tilt, breath noise)
- Ensemble effect
- Delay effect (circular buffer, multiple modes, tempo sync)
- Reverb effect (FDN architecture)
- Parameter smoothing
- Numerical stability considerations

#### Formant_Tables.md
Comprehensive vowel formant frequency data:
- Adult male formants (default voice)
- Adult female formants
- Child formants
- Bandwidth values per formant
- Amplitude scaling factors
- Implementation guide with code examples
- Based on phonetic research (Peterson & Barney, Hillenbrand et al.)

#### Parameter_Reference.md
Complete database of all 35+ parameters:
- Parameter IDs (stable, never change)
- Names, types, ranges, defaults
- Units and display formats
- Automation and MIDI mapping capabilities
- Smoothing time constants
- Categories and organization
- Default MIDI CC mappings
- Implementation notes

### Additional Resources

The `/pre-project-files/` directory contains detailed specifications from the planning phase:
- MonkSynth JUCE Architecture Blueprint
- DSP Math Details
- Parameter and MIDI Mapping Spec
- UI Component Spec
- Testing and QA Spec
- Build and CI Spec
- AudioThread RealTime Safety Spec
- Modulation Matrix Expansion Spec (future)

These provide additional depth and were used to inform the main project plan.

---

## Development Phases Summary

| Phase | Duration | Deliverable |
|-------|----------|-------------|
| **0. Project Setup** | 3 days | Empty plugin loads in DAW |
| **1. Voice Engine** | 4 days | Monophonic synth responds to MIDI |
| **2. Formant Synthesis** | 5 days | Recognizable vocal character |
| **3. Effects Chain** | 5 days | All effects functional |
| **4. MIDI Mapping** | 5 days | MIDI learn complete |
| **5. User Interface** | 8 days | Full UI implemented |
| **6. Optimization** | 5 days | Performance targets met |
| **7. Testing & QA** | 5 days | All tests pass |
| **8. Build & Package** | 3 days | Signed installer ready |
| **9. Documentation** | 5 days | Docs complete, v1.0 released |
| **Total** | **~48 days** | **Production-ready VST3** |

---

## Technology Stack

- **Framework:** JUCE 7.x
- **Language:** C++17 or later
- **Format:** VST3 (AU optional future)
- **Platform:** macOS (primary), Windows (future)
- **Build System:** CMake
- **Testing:** Catch2 or Google Test
- **CI/CD:** GitHub Actions

---

## Success Criteria

The project is complete when:

✅ Plugin loads in Ableton Live without errors
✅ All 35+ parameters are automatable
✅ MIDI notes produce correct pitched vocal sounds
✅ Vowel morphing is smooth and recognizable
✅ All effects function correctly
✅ MIDI learn can map any CC to any parameter
✅ State save/restore works reliably
✅ CPU usage <5% per instance
✅ No audio thread allocations
✅ Stable at 44.1k-96k sample rates
✅ No crashes in normal operation
✅ UI is intuitive and responsive
✅ Alpaca character animates with sound
✅ Documentation is complete
✅ Plugin is signed and notarized
✅ Version 1.0.0 released publicly

---

## Getting Started

### For Developers

1. **Read the MASTER_PROJECT_PLAN.md** to understand the full scope
2. **Review the support files** for technical details
3. **Set up your development environment** (JUCE, Xcode, etc.)
4. **Follow Phase 0** to create the initial project structure
5. **Work through phases sequentially**, testing as you go

### For AI Agents

1. **Load the AI_AGENT_PROMPT.md** as your instruction set
2. **Reference the MASTER_PROJECT_PLAN.md** for detailed requirements
3. **Use support files** for implementation specifics
4. **Begin with Phase 0** and proceed methodically
5. **Report progress** at the end of each phase

---

## Research Foundation

This project is informed by comprehensive research into the original Delay Lama plugin:

**Original Delay Lama (2002):**
- Created by AudioNerdz (Netherlands)
- Monophonic formant synthesizer simulating "singing Tibetan monk"
- Real-time vowel morphing via XY controller
- 3D animated character interface
- Built-in stereo delay
- Full MIDI integration
- Became cult classic and viral meme

**Technical Foundation:**
- Parallel formant filter synthesis
- Bandpass filter bank for vowel shaping
- Real-time parameter interpolation
- Simple but effective delay processing
- Playful yet functional UI design

**Our Enhancements:**
- Modern plugin standards (VST3)
- Enhanced synthesis (dual envelopes, formant shift)
- Extended effects (ensemble, multiple reverb types)
- Professional MIDI mapping with NRPN
- Scalable, high-DPI UI
- Real-time safe architecture
- Comprehensive preset system

---

## Legal and Ethical Considerations

### Original Work
✅ All code is original and written from scratch
✅ Alpaca character design is unique and distinct
✅ "Alpaca" name is legally distinct from "Delay Lama"
✅ DSP algorithms are based on public domain techniques
✅ Formant synthesis is a standard, well-documented approach

### What We Don't Do
❌ Copy copyrighted artwork or assets
❌ Reverse engineer or decompile Delay Lama
❌ Use proprietary code or algorithms
❌ Infringe on trademarks or trade dress

### Attribution
- JUCE framework (GPL or commercial license)
- Academic papers for formant data (cited in Formant_Tables.md)
- Standard DSP algorithms (textbook implementations)

---

## Contributing

If you're contributing to this project:

1. **Follow the plan** - deviations should be discussed
2. **Maintain code quality** - clean, documented, testable
3. **Test continuously** - don't save testing for the end
4. **Document as you go** - explain complex decisions
5. **Respect real-time safety** - zero allocations in audio thread
6. **Commit frequently** - working code in small increments

---

## Support and Questions

For questions about the plan or implementation:
1. Check the relevant specification document first
2. Review related sections in pre-project-files
3. Consult JUCE documentation for framework questions
4. Ask clarifying questions if requirements are ambiguous

---

## Timeline

**Estimated Development Time:** 8 weeks (48 working days)

This is a realistic timeline for a skilled C++ audio developer working full-time. Adjustments may be needed based on:
- Experience level with JUCE
- Familiarity with DSP algorithms
- Time spent on polish and additional features
- Testing thoroughness

---

## Version History

| Version | Date | Description |
|---------|------|-------------|
| 1.0 | 2026-03-02 | Initial complete project plan |

---

## Next Steps

**Ready to begin?**

1. Open `MASTER_PROJECT_PLAN.md`
2. Read through completely
3. Set up development environment
4. Start Phase 0: Project Setup

**Or, if using an AI agent:**

1. Load `AI_AGENT_PROMPT.md`
2. Provide access to all support files
3. Initiate Phase 0

---

## Project Vision

*"Alpaca honors the playful spirit and accessibility of the original Delay Lama while meeting modern professional standards. We're not just cloning a plugin—we're evolving a classic for a new generation of music makers."*

**Let's build something amazing. 🦙**

---

**END OF PROJECT PLAN README**
