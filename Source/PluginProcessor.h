/*
  ==============================================================================

    Alpaca VST3 Plugin
    Copyright (c) 2026 Alpaca Audio

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>

#include "Parameters.h"

struct VoiceEngine;

class AlpacaAudioProcessor : public juce::AudioProcessor
{
public:
    AlpacaAudioProcessor();
    ~AlpacaAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    bool hasEditor() const override;
    juce::AudioProcessorEditor* createEditor() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    std::unique_ptr<VoiceEngine> voiceEngine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AlpacaAudioProcessor)
};
