cmake_minimum_required(VERSION 3.22)

project(Alpaca VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 7.0.12
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(
  Alpaca
  COMPANY_NAME "Alpaca Audio"
  BUNDLE_ID "com.alpacaaudio.alpaca"
  IS_SYNTH TRUE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  PLUGIN_MANUFACTURER_CODE ALPA
  PLUGIN_CODE ALPC
  FORMATS VST3
  PRODUCT_NAME "Alpaca"
)

target_sources(
  Alpaca
  PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/Parameters.cpp
    Source/Parameters.h
)

target_compile_definitions(
  Alpaca
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(
  Alpaca
  PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_dsp
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

juce_generate_juce_header(Alpaca)
