/*
  ==============================================================================

    Alpaca VST3 Plugin
    Copyright (c) 2026 Alpaca Audio

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace alpaca::parameters
{
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
