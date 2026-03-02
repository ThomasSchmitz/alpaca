/*
  ==============================================================================

    Alpaca VST3 Plugin
    Copyright (c) 2026 Alpaca Audio

  ==============================================================================
*/

#include "PluginProcessor.h"

#include <juce_audio_utils/juce_audio_utils.h>

AlpacaAudioProcessor::AlpacaAudioProcessor()
    : juce::AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "Parameters", alpaca::parameters::createParameterLayout())
{
}

const juce::String AlpacaAudioProcessor::getName() const
{
    return "Alpaca";
}

bool AlpacaAudioProcessor::acceptsMidi() const
{
    return true;
}

bool AlpacaAudioProcessor::producesMidi() const
{
    return false;
}

bool AlpacaAudioProcessor::isMidiEffect() const
{
    return false;
}

double AlpacaAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AlpacaAudioProcessor::getNumPrograms()
{
    return 1;
}

int AlpacaAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AlpacaAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AlpacaAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AlpacaAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void AlpacaAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void AlpacaAudioProcessor::releaseResources() {}

bool AlpacaAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled()
           || layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}

void AlpacaAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                        juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();
}

bool AlpacaAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AlpacaAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

void AlpacaAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    juce::MemoryOutputStream stream(destData, true);
    state.writeToStream(stream);
}

void AlpacaAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto state =
            juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));
        state.isValid())
        parameters.replaceState(state);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AlpacaAudioProcessor();
}
