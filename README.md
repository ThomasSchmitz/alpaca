# Alpaca VST3

Phase 0 project setup for the Alpaca vocal synth plugin.

## Building

Requirements:
- CMake 3.22+
- A C++17 compiler
- macOS or Linux build environment with audio plugin SDK prerequisites (JUCE fetches VST3 SDK)

Steps:
1. Configure: `cmake -B build -S . -DCMAKE_BUILD_TYPE=Release`
2. Build: `cmake --build build --config Release`

The CMake project downloads JUCE automatically during configuration.
