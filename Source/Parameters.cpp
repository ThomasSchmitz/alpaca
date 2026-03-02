/*
  ==============================================================================

    Alpaca VST3 Plugin
    Copyright (c) 2026 Alpaca Audio

  ==============================================================================
*/

#include "Parameters.h"

namespace
{
using Parameter = std::unique_ptr<juce::RangedAudioParameter>;

Parameter makeFloat(const juce::String& id,
                    const juce::String& name,
                    const juce::NormalisableRange<float>& range,
                    float defaultValue)
{
    return std::make_unique<juce::AudioParameterFloat>(id, name, range, defaultValue);
}

Parameter makeBool(const juce::String& id, const juce::String& name, bool defaultValue)
{
    return std::make_unique<juce::AudioParameterBool>(id, name, defaultValue);
}

Parameter makeChoice(const juce::String& id,
                     const juce::String& name,
                     const juce::StringArray& choices,
                     int defaultIndex)
{
    return std::make_unique<juce::AudioParameterChoice>(id, name, choices, defaultIndex);
}

juce::StringArray midiChannelChoices()
{
    juce::StringArray channels;
    channels.add("omni");
    for (int channel = 1; channel <= 16; ++channel)
        channels.add(juce::String(channel));

    return channels;
}
}  // namespace

juce::AudioProcessorValueTreeState::ParameterLayout alpaca::parameters::createParameterLayout()
{
    std::vector<Parameter> params;
    params.reserve(40);

    // Performance
    params.push_back(makeFloat("vowel", "Vowel", {0.0f, 1.0f}, 0.35f));
    params.push_back(makeFloat("glide_ms", "Glide", {0.0f, 2000.0f}, 50.0f));
    params.push_back(makeFloat("voice_character", "Voice Character", {0.0f, 1.0f}, 0.50f));
    params.push_back(makeFloat("vibrato_depth", "Vibrato Depth", {0.0f, 1.0f}, 0.0f));
    params.push_back(makeFloat("vibrato_rate_hz", "Vibrato Rate", {0.1f, 12.0f}, 5.5f));

    // Envelope
    params.push_back(makeFloat("amp_attack_ms", "Amp Attack", {0.0f, 200.0f}, 5.0f));
    params.push_back(makeFloat("amp_release_ms", "Amp Release", {5.0f, 2000.0f}, 120.0f));

    // Tone
    params.push_back(makeFloat("formant_shift_semitones", "Formant Shift", {-12.0f, 12.0f}, 0.0f));
    params.push_back(makeFloat("tilt", "Brightness", {0.0f, 1.0f}, 0.55f));
    params.push_back(makeFloat("breath_noise", "Breath/Noise", {0.0f, 1.0f}, 0.0f));
    params.push_back(makeFloat("voice_size", "Voice Size", {0.0f, 1.0f}, 0.50f));

    // Ensemble
    params.push_back(makeBool("ensemble_enable", "Ensemble Enable", true));
    params.push_back(makeFloat("ensemble_amount", "Ensemble Amount", {0.0f, 1.0f}, 0.15f));
    params.push_back(makeFloat("ensemble_width", "Ensemble Width", {0.0f, 1.0f}, 0.65f));
    params.push_back(makeFloat("ensemble_detune_cents", "Ensemble Detune", {0.0f, 20.0f}, 6.0f));
    params.push_back(makeChoice("ensemble_mode",
                                "Ensemble Mode",
                                juce::StringArray({"mono", "stereo", "choir"}),
                                1));

    // Delay
    params.push_back(makeBool("delay_enable", "Delay Enable", true));
    params.push_back(makeChoice("delay_mode",
                                "Delay Mode",
                                juce::StringArray({"mono", "stereo", "ping_pong"}),
                                2));
    params.push_back(makeBool("delay_sync", "Delay Sync", false));
    params.push_back(makeFloat("delay_time_ms", "Delay Time", {1.0f, 2000.0f}, 320.0f));
    params.push_back(makeChoice("delay_time_div",
                                "Delay Division",
                                juce::StringArray({"1/1",
                                                   "1/2",
                                                   "1/4",
                                                   "1/8",
                                                   "1/16",
                                                   "1/8T",
                                                   "1/16T",
                                                   "1/8D",
                                                   "1/16D"}),
                                3));
    params.push_back(makeFloat("delay_feedback",
                               "Delay Feedback",
                               juce::NormalisableRange<float>(0.0f, 0.95f, 0.0f, 0.7f),
                               0.35f));
    params.push_back(makeFloat("delay_damping", "Delay Damping", {0.0f, 1.0f}, 0.35f));
    params.push_back(makeFloat("delay_wet", "Delay Wet", {0.0f, 1.0f}, 0.25f));

    // Reverb
    params.push_back(makeBool("reverb_enable", "Reverb Enable", true));
    params.push_back(makeChoice("reverb_type",
                                "Reverb Type",
                                juce::StringArray({"hall", "cathedral", "plate", "shimmer"}),
                                0));
    params.push_back(makeFloat("reverb_size", "Reverb Size", {0.0f, 1.0f}, 0.60f));
    params.push_back(makeFloat("reverb_decay_s",
                               "Reverb Decay",
                               juce::NormalisableRange<float>(0.1f, 20.0f, 0.0f, 0.5f),
                               4.5f));
    params.push_back(makeFloat("reverb_predelay_ms", "Reverb Pre-delay", {0.0f, 250.0f}, 18.0f));
    params.push_back(makeFloat("reverb_damping", "Reverb Damping", {0.0f, 1.0f}, 0.40f));
    params.push_back(makeFloat("reverb_wet", "Reverb Wet", {0.0f, 1.0f}, 0.22f));

    // Global
    params.push_back(makeFloat("output_gain_db", "Output Gain", {-24.0f, 12.0f}, 0.0f));

    // MIDI
    params.push_back(makeBool("midi_learn_enable", "MIDI Learn Enable", false));
    params.push_back(makeBool("midi_writes_automation", "MIDI Writes Automation", false));
    params.push_back(makeChoice("midi_mapping_mode",
                                "MIDI Mapping Mode",
                                juce::StringArray({"set", "modulate"}),
                                0));
    params.push_back(makeChoice("midi_channel", "MIDI Channel", midiChannelChoices(), 0));
    params.push_back(makeChoice("midi_hi_res",
                                "MIDI Hi-Res Mode",
                                juce::StringArray({"cc7", "nrpn14"}),
                                1));

    // Advanced
    params.push_back(makeChoice("pitch_control_mode",
                                "Pitch Control Mode",
                                juce::StringArray({"midi_note", "xy_offset", "xy_absolute"}),
                                0));
    params.push_back(
        makeFloat("xy_pitch_range_semitones", "XY Pitch Range", {0.0f, 24.0f}, 12.0f));
    params.push_back(makeChoice("mono_priority",
                                "Mono Priority",
                                juce::StringArray({"last", "low", "high"}),
                                0));

    return {params.begin(), params.end()};
}
