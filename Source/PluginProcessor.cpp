/*
  ==============================================================================

    Alpaca VST3 Plugin
    Copyright (c) 2026 Alpaca Audio

  ==============================================================================
*/

#include "PluginProcessor.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <juce_audio_utils/juce_audio_utils.h>

namespace
{
constexpr float kPitchBendRangeSemitones = 2.0f;
constexpr size_t kMaxHeldNotes = 32;
constexpr float kMinEnvelopeLevel = 1.0e-5f;
// Output scaling before the master gain stage keeps signal levels safe.
constexpr float kVoiceOutputScale = 0.2f;
// Tilt gain scale: maps the ±1 normalised range to approximately ±6 dB.
constexpr float kTiltGainScale = 1.5f;

struct ParamSnapshot
{
    float glideMs = 0.0f;
    float vibratoDepth = 0.0f;
    float vibratoRateHz = 0.0f;
    float attackMs = 0.0f;
    float releaseMs = 0.0f;
    float outputGainDb = 0.0f;
    int monoPriorityIndex = 0;
    int midiChannelChoice = 0;

    // Phase 2 additions
    float vowel = 0.35f;
    float voiceCharacter = 0.5f;
    float formantShiftSemitones = 0.0f;
    float tilt = 0.55f;

    static ParamSnapshot from(const juce::AudioProcessorValueTreeState& state)
    {
        const auto get = [&](const char* id) { return state.getRawParameterValue(id)->load(); };

        ParamSnapshot snapshot;
        snapshot.glideMs = get("glide_ms");
        snapshot.vibratoDepth = get("vibrato_depth");
        snapshot.vibratoRateHz = get("vibrato_rate_hz");
        snapshot.attackMs = get("amp_attack_ms");
        snapshot.releaseMs = get("amp_release_ms");
        snapshot.outputGainDb = get("output_gain_db");
        snapshot.monoPriorityIndex = static_cast<int>(state.getRawParameterValue("mono_priority")->load());
        snapshot.midiChannelChoice = static_cast<int>(state.getRawParameterValue("midi_channel")->load());
        snapshot.vowel = get("vowel");
        snapshot.voiceCharacter = get("voice_character");
        snapshot.formantShiftSemitones = get("formant_shift_semitones");
        snapshot.tilt = get("tilt");
        return snapshot;
    }

    float vibratoDepthSemitones() const { return vibratoDepth * 2.0f; }

    int midiChannel() const { return midiChannelChoice == 0 ? 0 : midiChannelChoice; }
};

// ===== Formant Synthesis (Phase 2) ===========================================

struct BiquadCoeffs
{
    float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f;
};

// Bandpass filter: constant 0 dB peak gain (Audio EQ Cookbook BPF).
// Uses b0 = alpha/a0, b1 = 0, b2 = -alpha/a0.
BiquadCoeffs calculateBandpassCoeffs(float centerFreq, float bandwidth, float sampleRate)
{
    centerFreq = std::max(centerFreq, 20.0f);
    bandwidth  = std::max(bandwidth,   1.0f);

    const float omega    = juce::MathConstants<float>::twoPi * centerFreq / sampleRate;
    const float sinOmega = std::sin(omega);
    const float cosOmega = std::cos(omega);
    const float q        = centerFreq / bandwidth;
    const float alpha    = sinOmega / (2.0f * q);
    const float a0inv    = 1.0f / (1.0f + alpha);

    BiquadCoeffs c;
    c.b0 =  alpha * a0inv;
    c.b1 =  0.0f;
    c.b2 = -alpha * a0inv;
    c.a1 = -2.0f * cosOmega * a0inv;
    c.a2 = (1.0f - alpha) * a0inv;
    return c;
}

// Direct Form I biquad filter. State variables: x1/x2 (input history),
// y1/y2 (output history).  State updates occur after computing output,
// which is correct for DF1 and ensures causality.
class Biquad
{
public:
    void setCoefficients(const BiquadCoeffs& newCoeffs) { c = newCoeffs; }

    float processSample(float input)
    {
        const float output = c.b0 * input + c.b1 * x1 + c.b2 * x2
                           - c.a1 * y1 - c.a2 * y2;
        x2 = x1; x1 = input;
        y2 = y1; y1 = output;
        return output;
    }

    void reset() { x1 = x2 = y1 = y2 = 0.0f; }

private:
    BiquadCoeffs c;
    float x1 = 0.0f, x2 = 0.0f;
    float y1 = 0.0f, y2 = 0.0f;
};

// Formant data for a single vowel (5 formants).
struct VowelFormants
{
    std::array<float, 5> frequencies{};
    std::array<float, 5> bandwidths{};
    std::array<float, 5> amplitudes{};
};

// Holds formant tables for two voice types (male tenor, female) and 5 vowels
// (OO, OH, AH, EH, EE).  Provides interpolated lookup across both dimensions.
class VowelTables
{
public:
    VowelTables() { initializeTables(); }

    // vowelParam  0..1  (OO→OH→AH→EH→EE)
    // voiceChar   0..1  (0 = male tenor, 1 = female)
    VowelFormants interpolate(float voiceChar, float vowelParam) const
    {
        voiceChar  = std::clamp(voiceChar,  0.0f, 1.0f);
        vowelParam = std::clamp(vowelParam, 0.0f, 1.0f);

        const auto mf = interpolateVowels(0, vowelParam);
        const auto ff = interpolateVowels(1, vowelParam);

        VowelFormants result;
        for (int i = 0; i < 5; ++i)
        {
            result.frequencies[i] = mf.frequencies[i]
                                   + (ff.frequencies[i] - mf.frequencies[i]) * voiceChar;
            result.bandwidths[i]  = mf.bandwidths[i]
                                   + (ff.bandwidths[i]  - mf.bandwidths[i])  * voiceChar;
            result.amplitudes[i]  = mf.amplitudes[i]
                                   + (ff.amplitudes[i]  - mf.amplitudes[i])  * voiceChar;
        }
        return result;
    }

private:
    // tables[voiceType][vowelIndex]  where voiceType: 0=male, 1=female
    //                                      vowelIndex: 0=OO 1=OH 2=AH 3=EH 4=EE
    std::array<std::array<VowelFormants, 5>, 2> tables;

    VowelFormants interpolateVowels(int v, float vowelParam) const
    {
        const float scaled   = vowelParam * 4.0f;
        const int   index    = std::clamp(static_cast<int>(scaled), 0, 3);
        const float fraction = scaled - static_cast<float>(index);

        const auto& v1 = tables[static_cast<size_t>(v)][static_cast<size_t>(index)];
        const auto& v2 = tables[static_cast<size_t>(v)][static_cast<size_t>(index) + 1];

        VowelFormants result;
        for (int i = 0; i < 5; ++i)
        {
            result.frequencies[i] = v1.frequencies[i] + (v2.frequencies[i] - v1.frequencies[i]) * fraction;
            result.bandwidths[i]  = v1.bandwidths[i]  + (v2.bandwidths[i]  - v1.bandwidths[i])  * fraction;
            result.amplitudes[i]  = v1.amplitudes[i]  + (v2.amplitudes[i]  - v1.amplitudes[i])  * fraction;
        }
        return result;
    }

    void initializeTables()
    {
        // Male tenor formants (Peterson & Barney 1952, adjusted for synthesis)
        // BW: F1=80 Hz  F2=100 Hz  F3=140 Hz  F4=180 Hz  F5=250 Hz
        // Amplitudes relative to F1 = 1.0
        auto& m = tables[0];
        // OO
        m[0].frequencies = {300.0f, 870.0f, 2240.0f, 2990.0f, 3320.0f};
        m[0].bandwidths  = { 80.0f, 100.0f,  140.0f,  180.0f,  250.0f};
        m[0].amplitudes  = {  1.0f,  0.70f,   0.45f,   0.35f,   0.25f};
        // OH
        m[1].frequencies = {400.0f,  800.0f, 2600.0f, 2900.0f, 3300.0f};
        m[1].bandwidths  = { 80.0f,  100.0f,  140.0f,  180.0f,  250.0f};
        m[1].amplitudes  = {  1.0f,   0.75f,   0.50f,   0.35f,   0.25f};
        // AH
        m[2].frequencies = {750.0f, 1220.0f, 2600.0f, 2900.0f, 3300.0f};
        m[2].bandwidths  = { 80.0f,  100.0f,  140.0f,  180.0f,  250.0f};
        m[2].amplitudes  = {  1.0f,   0.65f,   0.50f,   0.35f,   0.25f};
        // EH
        m[3].frequencies = {600.0f, 1700.0f, 2600.0f, 2900.0f, 3300.0f};
        m[3].bandwidths  = { 80.0f,  100.0f,  140.0f,  180.0f,  250.0f};
        m[3].amplitudes  = {  1.0f,   0.80f,   0.55f,   0.35f,   0.25f};
        // EE
        m[4].frequencies = {270.0f, 2290.0f, 3010.0f, 3300.0f, 3750.0f};
        m[4].bandwidths  = { 80.0f,  100.0f,  140.0f,  180.0f,  250.0f};
        m[4].amplitudes  = {  1.0f,   0.70f,   0.40f,   0.30f,   0.20f};

        // Female formants (~1.15-1.25x male for F1-F3; BW scaled by 1.1x)
        auto& f = tables[1];
        // OO
        f[0].frequencies = {370.0f,  950.0f, 2670.0f, 3350.0f, 3850.0f};
        f[0].bandwidths  = { 88.0f,  110.0f,  154.0f,  198.0f,  275.0f};
        f[0].amplitudes  = {  1.0f,   0.70f,   0.45f,   0.35f,   0.25f};
        // OH
        f[1].frequencies = {470.0f,  920.0f, 2810.0f, 3260.0f, 3850.0f};
        f[1].bandwidths  = { 88.0f,  110.0f,  154.0f,  198.0f,  275.0f};
        f[1].amplitudes  = {  1.0f,   0.75f,   0.50f,   0.35f,   0.25f};
        // AH
        f[2].frequencies = {860.0f, 1400.0f, 2810.0f, 3260.0f, 3850.0f};
        f[2].bandwidths  = { 88.0f,  110.0f,  154.0f,  198.0f,  275.0f};
        f[2].amplitudes  = {  1.0f,   0.65f,   0.50f,   0.35f,   0.25f};
        // EH
        f[3].frequencies = {690.0f, 2020.0f, 2810.0f, 3260.0f, 3850.0f};
        f[3].bandwidths  = { 88.0f,  110.0f,  154.0f,  198.0f,  275.0f};
        f[3].amplitudes  = {  1.0f,   0.80f,   0.55f,   0.35f,   0.25f};
        // EE
        f[4].frequencies = {330.0f, 2640.0f, 3350.0f, 3700.0f, 4250.0f};
        f[4].bandwidths  = { 88.0f,  110.0f,  154.0f,  198.0f,  275.0f};
        f[4].amplitudes  = {  1.0f,   0.70f,   0.40f,   0.30f,   0.20f};
    }
};

// Shift all formant frequencies by shiftSemitones (±12 semitones max).
VowelFormants applyFormantShift(const VowelFormants& base, float shiftSemitones)
{
    const float scale = std::pow(2.0f, shiftSemitones / 12.0f);
    VowelFormants shifted = base;
    for (int i = 0; i < 5; ++i)
    {
        shifted.frequencies[i] = std::max(shifted.frequencies[i] * scale, 20.0f);
        shifted.bandwidths[i]  = std::max(shifted.bandwidths[i]  * scale,  1.0f);
    }
    return shifted;
}

// Five parallel bandpass filters summed together.
class FormantBank
{
public:
    static constexpr int kNumFormants = 5;

    void prepare(double sr)
    {
        sampleRate = sr;
        reset();
    }

    void updateFormants(const VowelFormants& formants)
    {
        // Compute normalisation so all vowels produce a similar output level.
        float sumSq = 0.0f;
        for (int i = 0; i < kNumFormants; ++i)
            sumSq += formants.amplitudes[i] * formants.amplitudes[i];

        const float rms  = (sumSq > 0.0f) ? std::sqrt(sumSq / static_cast<float>(kNumFormants)) : 1.0f;
        const float norm = (rms > 0.0f) ? (0.7f / rms) : 1.0f;

        for (int i = 0; i < kNumFormants; ++i)
        {
            const auto coeffs = calculateBandpassCoeffs(formants.frequencies[i],
                                                         formants.bandwidths[i],
                                                         static_cast<float>(sampleRate));
            filters[i].setCoefficients(coeffs);
            gains[i] = formants.amplitudes[i] * norm;
        }
    }

    float processSample(float input)
    {
        float output = 0.0f;
        for (int i = 0; i < kNumFormants; ++i)
            output += filters[i].processSample(input) * gains[i];
        return output;
    }

    void reset()
    {
        for (auto& f : filters)
            f.reset();
    }

private:
    std::array<Biquad, kNumFormants> filters;
    std::array<float, kNumFormants>  gains{};
    double sampleRate = 44100.0;
};

// First-order shelving filter for spectral tilt / brightness control.
// tiltNorm 0..1 maps to dark..bright.
class TiltFilter
{
public:
    void prepare(double sr)
    {
        sampleRate = sr;
        // Cutoff at ~1 kHz, coefficient updated when tilt changes.
        const float cutoff = 1000.0f;
        coeff = 1.0f - std::exp(-juce::MathConstants<float>::twoPi
                                 * cutoff / static_cast<float>(sampleRate));
        reset();
    }

    void setTilt(float tiltNorm)
    {
        // Convert 0..1 → -1..+1; scale for a musically useful ±6 dB range.
        gain = (tiltNorm * 2.0f - 1.0f) * kTiltGainScale;
    }

    float processSample(float input)
    {
        // One-pole LPF state, then add gain * high-pass component.
        state += coeff * (input - state);
        return input + gain * (input - state);
    }

    void reset() { state = 0.0f; }

private:
    double sampleRate = 44100.0;
    float  coeff      = 0.0f;
    float  gain       = 0.0f;
    float  state      = 0.0f;
};

float midiNoteToFrequency(int noteNumber)
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(noteNumber) - 69.0f) / 12.0f);
}

float calculateGlideCoefficient(float glideTimeMs, double sampleRate)
{
    if (glideTimeMs <= 0.0f || sampleRate <= 0.0)
        return 1.0f;

    const auto tauSeconds = glideTimeMs / 1000.0f;
    const auto coeff = 1.0f - std::exp(-1.0f / (tauSeconds * static_cast<float>(sampleRate)));
    return juce::jlimit(0.0f, 1.0f, coeff);
}

float applyPitchBend(float baseFrequency, int pitchBendValue)
{
    const auto bendNorm = (static_cast<float>(pitchBendValue) - 8192.0f) / 8192.0f;
    const auto semitones = bendNorm * kPitchBendRangeSemitones;
    return baseFrequency * std::pow(2.0f, semitones / 12.0f);
}

float applyVibrato(float baseFrequency, float lfoValue, float depthSemitones)
{
    if (depthSemitones <= 0.0f)
        return baseFrequency;

    return baseFrequency * std::pow(2.0f, (depthSemitones * lfoValue) / 12.0f);
}

class MonoNoteManager
{
public:
    enum class Priority
    {
        Last,
        Low,
        High
    };

    struct NoteChange
    {
        bool hasNote = false;
        int note = -1;
        juce::uint8 velocity = 0;
        bool noteChanged = false;
        bool retriggerEnvelope = false;
    };

    void setPriority(Priority newPriority) { priority = newPriority; }

    NoteChange noteOn(int note, juce::uint8 velocity)
    {
        const bool hadNote = numActive > 0;
        const auto previousNote = activeNote;

        if (const auto idx = findIndex(note); idx >= 0)
        {
            notes[static_cast<size_t>(idx)].velocity = velocity;
            notes[static_cast<size_t>(idx)].order = ++orderCounter;
        }
        else
        {
            if (numActive < notes.size())
                notes[numActive++] = {note, velocity, ++orderCounter};
            else
                notes.back() = {note, velocity, ++orderCounter};
        }

        selectActiveNote();

        NoteChange change;
        change.hasNote = numActive > 0;
        change.note = activeNote;
        change.velocity = activeVelocity;
        change.noteChanged = activeNote != previousNote || note == previousNote;
        change.retriggerEnvelope = !hadNote || note == previousNote;
        return change;
    }

    NoteChange noteOff(int note)
    {
        const auto previousNote = activeNote;

        if (const auto idx = findIndex(note); idx >= 0)
        {
            for (size_t i = static_cast<size_t>(idx); i + 1 < numActive; ++i)
                notes[i] = notes[i + 1];

            if (numActive > 0)
                --numActive;
        }

        selectActiveNote();

        NoteChange change;
        change.hasNote = numActive > 0;
        change.note = activeNote;
        change.velocity = activeVelocity;
        change.noteChanged = activeNote != previousNote;
        change.retriggerEnvelope = false;
        return change;
    }

    void allNotesOff()
    {
        numActive = 0;
        activeNote = -1;
        activeVelocity = 0;
    }

    bool hasActiveNote() const { return numActive > 0; }
    int currentNote() const { return activeNote; }
    juce::uint8 currentVelocity() const { return activeVelocity; }

private:
    struct ActiveNote
    {
        int note = -1;
        juce::uint8 velocity = 0;
        std::uint64_t order = 0;
    };

    int findIndex(int note) const
    {
        for (size_t i = 0; i < numActive; ++i)
        {
            if (notes[i].note == note)
                return static_cast<int>(i);
        }
        return -1;
    }

    int chooseIndex() const
    {
        if (numActive == 0)
            return -1;

        size_t best = 0;

        switch (priority)
        {
            case Priority::Last:
                for (size_t i = 1; i < numActive; ++i)
                {
                    if (notes[i].order > notes[best].order)
                        best = i;
                }
                break;
            case Priority::Low:
                for (size_t i = 1; i < numActive; ++i)
                {
                    if (notes[i].note < notes[best].note)
                        best = i;
                }
                break;
            case Priority::High:
                for (size_t i = 1; i < numActive; ++i)
                {
                    if (notes[i].note > notes[best].note)
                        best = i;
                }
                break;
        }

        return static_cast<int>(best);
    }

    void selectActiveNote()
    {
        const auto idx = chooseIndex();
        if (idx < 0)
        {
            activeNote = -1;
            activeVelocity = 0;
            return;
        }

        activeNote = notes[static_cast<size_t>(idx)].note;
        activeVelocity = notes[static_cast<size_t>(idx)].velocity;
    }

    std::array<ActiveNote, kMaxHeldNotes> notes{};
    size_t numActive = 0;
    Priority priority = Priority::Last;
    std::uint64_t orderCounter = 0;
    int activeNote = -1;
    juce::uint8 activeVelocity = 0;
};

class BandlimitedSaw
{
public:
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        phase = 0.0f;
    }

    void setFrequency(float frequency)
    {
        const auto increment = frequency / static_cast<float>(sampleRate);
        phaseIncrement = std::clamp(increment, 0.0f, 0.5f);
    }

    float processSample()
    {
        const auto t = phase;
        float value = 2.0f * t - 1.0f;

        value -= polyBlep(t);
        value += polyBlep(t + phaseIncrement);

        phase += phaseIncrement;
        if (phase >= 1.0f)
            phase -= 1.0f;

        return value;
    }

    void reset() { phase = 0.0f; }

private:
    float polyBlep(float t) const
    {
        if (phaseIncrement <= 0.0f)
            return 0.0f;

        if (t < phaseIncrement)
        {
            t /= phaseIncrement;
            return t + t - t * t - 1.0f;
        }

        if (t > 1.0f - phaseIncrement)
        {
            t = (t - 1.0f) / phaseIncrement;
            return t * t + t + t + 1.0f;
        }

        return 0.0f;
    }

    double sampleRate = 44100.0;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
};

class VibratoLFO
{
public:
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        reset();
    }

    void setRateHz(float rateHz) { phaseIncrement = rateHz / static_cast<float>(sampleRate); }

    float processSample()
    {
        const auto output = std::sin(juce::MathConstants<float>::twoPi * phase);
        phase += phaseIncrement;
        if (phase >= 1.0f)
            phase -= 1.0f;

        return output;
    }

    void reset() { phase = 0.0f; }

private:
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
};

class AREnvelope
{
public:
    void prepare(double newSampleRate) { sampleRate = newSampleRate; }

    void setAttackMs(float ms) { attackCoeff = calculateGlideCoefficient(ms, sampleRate); }
    void setReleaseMs(float ms) { releaseCoeff = calculateGlideCoefficient(ms, sampleRate); }

    void noteOn(bool retrigger)
    {
        gate = true;
        if (retrigger)
        {
            state = State::Attack;
            level = 0.0f;
            return;
        }

        state = level < 1.0f ? State::Attack : State::Sustain;
    }

    void noteOff()
    {
        gate = false;
        state = State::Release;
    }

    float process()
    {
        switch (state)
        {
            case State::Idle:
                level = 0.0f;
                break;
            case State::Attack:
                level += (1.0f - level) * attackCoeff;
                if (level >= 0.999f)
                {
                    level = 1.0f;
                    state = gate ? State::Sustain : State::Release;
                }
                break;
            case State::Sustain:
                level = 1.0f;
                break;
            case State::Release:
                level += (0.0f - level) * releaseCoeff;
                if (level < kMinEnvelopeLevel)
                {
                    level = 0.0f;
                    state = State::Idle;
                }
                break;
        }

        return level;
    }

    bool isActive() const { return state != State::Idle && level > kMinEnvelopeLevel; }

    void reset()
    {
        level = 0.0f;
        state = State::Idle;
        gate = false;
    }

private:
    enum class State
    {
        Idle,
        Attack,
        Sustain,
        Release
    };

    double sampleRate = 44100.0;
    float attackCoeff = 1.0f;
    float releaseCoeff = 1.0f;
    float level = 0.0f;
    State state = State::Idle;
    bool gate = false;
};

}  // namespace

class VoiceEngine
{
public:
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        oscillator.prepare(sampleRate);
        vibrato.prepare(sampleRate);
        envelope.prepare(sampleRate);
        formantBank.prepare(sampleRate);
        tiltFilter.prepare(sampleRate);

        // Initialise with default formant snapshot (vowel=0.35, male tenor).
        const VowelFormants defaultFormants = applyFormantShift(
            vowelTables.interpolate(0.5f, 0.35f), 0.0f);
        formantBank.updateFormants(defaultFormants);
        tiltFilter.setTilt(0.55f);
    }

    void reset()
    {
        noteManager.allNotesOff();
        envelope.reset();
        vibrato.reset();
        oscillator.reset();
        formantBank.reset();
        tiltFilter.reset();
        currentFrequency = 0.0f;
        targetFrequency = 0.0f;
        pitchBendValue = 8192;
        velocityScale = 0.0f;
        std::fill(ccValues.begin(), ccValues.end(), 0);
    }

    void setPriority(MonoNoteManager::Priority priority) { noteManager.setPriority(priority); }

    void updateParameters(const ParamSnapshot& snapshot)
    {
        glideCoeff = calculateGlideCoefficient(snapshot.glideMs, sampleRate);
        vibratoDepthSemitones = snapshot.vibratoDepthSemitones();
        vibrato.setRateHz(snapshot.vibratoRateHz);
        envelope.setAttackMs(snapshot.attackMs);
        envelope.setReleaseMs(snapshot.releaseMs);
        outputGain = juce::Decibels::decibelsToGain(snapshot.outputGainDb);
        midiChannel = snapshot.midiChannel();

        // Update formant bank with interpolated vowel and voice character.
        const VowelFormants formants = applyFormantShift(
            vowelTables.interpolate(snapshot.voiceCharacter, snapshot.vowel),
            snapshot.formantShiftSemitones);
        formantBank.updateFormants(formants);

        // Update spectral tilt.
        tiltFilter.setTilt(snapshot.tilt);
    }

    void processMidiMessage(const juce::MidiMessage& message)
    {
        if (midiChannel != 0 && message.getChannel() != midiChannel)
            return;

        if (message.isAllNotesOff() || message.isAllSoundOff())
        {
            noteManager.allNotesOff();
            envelope.noteOff();
            velocityScale = 0.0f;
            return;
        }

        if (message.isPitchWheel())
        {
            pitchBendValue = message.getPitchWheelValue();
            return;
        }

        if (message.isController())
        {
            const auto controller = message.getControllerNumber();
            if (controller >= 0 && controller < static_cast<int>(ccValues.size()))
                ccValues[static_cast<size_t>(controller)] = static_cast<juce::uint8>(message.getControllerValue());
            return;
        }

        if (message.isNoteOn())
        {
            const auto change = noteManager.noteOn(message.getNoteNumber(), message.getVelocity());
            if (change.hasNote)
            {
                setTargetFrequency(change.note);
                velocityScale = static_cast<float>(change.velocity) / 127.0f;

                if (currentFrequency <= 0.0f)
                    currentFrequency = targetFrequency;

                envelope.noteOn(change.retriggerEnvelope);
            }
            return;
        }

        if (message.isNoteOff())
        {
            const auto change = noteManager.noteOff(message.getNoteNumber());

            if (change.hasNote)
            {
                setTargetFrequency(change.note);
                velocityScale = static_cast<float>(change.velocity) / 127.0f;
            }
            else
            {
                velocityScale = 0.0f;
                envelope.noteOff();
            }
        }
    }

    void render(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
    {
        if (numSamples <= 0)
            return;

        auto* left = buffer.getWritePointer(0, startSample);
        float* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1, startSample) : nullptr;

        for (int i = 0; i < numSamples; ++i)
        {
            const auto haveNote = noteManager.hasActiveNote();
            const auto envelopeActive = envelope.isActive();

            if (!haveNote && !envelopeActive)
            {
                left[i] = 0.0f;
                if (right != nullptr)
                    right[i] = 0.0f;
                continue;
            }

            currentFrequency += (targetFrequency - currentFrequency) * glideCoeff;
            const auto vibratoValue = vibrato.processSample();
            const auto frequency = applyVibrato(currentFrequency, vibratoValue, vibratoDepthSemitones);
            oscillator.setFrequency(frequency);

            // Signal chain: oscillator → formant bank → tilt → envelope → gain
            const auto osc      = oscillator.processSample();
            const auto formanted = formantBank.processSample(osc);
            const auto tilted    = tiltFilter.processSample(formanted);
            const auto env       = envelope.process();
            const auto sample    = tilted * env * velocityScale * outputGain * kVoiceOutputScale;

            left[i] = sample;
            if (right != nullptr)
                right[i] = sample;
        }
    }

private:
    void setTargetFrequency(int midiNote)
    {
        const auto baseFreq = midiNoteToFrequency(midiNote);
        targetFrequency = applyPitchBend(baseFreq, pitchBendValue);
    }

    MonoNoteManager noteManager;
    BandlimitedSaw oscillator;
    VibratoLFO vibrato;
    AREnvelope envelope;
    FormantBank formantBank;
    TiltFilter  tiltFilter;
    VowelTables vowelTables;

    std::array<juce::uint8, 128> ccValues{};
    double sampleRate = 44100.0;
    float glideCoeff = 1.0f;
    float vibratoDepthSemitones = 0.0f;
    float currentFrequency = 0.0f;
    float targetFrequency = 0.0f;
    float outputGain = 1.0f;
    float velocityScale = 0.0f;
    int pitchBendValue = 8192;
    int midiChannel = 0;
};

MonoNoteManager::Priority priorityFromIndex(int index)
{
    switch (index)
    {
        case 1:
            return MonoNoteManager::Priority::Low;
        case 2:
            return MonoNoteManager::Priority::High;
        default:
            return MonoNoteManager::Priority::Last;
    }
}

AlpacaAudioProcessor::AlpacaAudioProcessor()
    : juce::AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "Parameters", alpaca::parameters::createParameterLayout())
{
    voiceEngine = std::make_unique<VoiceEngine>();
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
    juce::ignoreUnused(samplesPerBlock);
    if (voiceEngine != nullptr)
    {
        voiceEngine->prepare(sampleRate);
        voiceEngine->reset();
    }
}

void AlpacaAudioProcessor::releaseResources()
{
    if (voiceEngine != nullptr)
        voiceEngine->reset();
}

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
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    const auto snapshot = ParamSnapshot::from(parameters);
    if (voiceEngine != nullptr)
    {
        voiceEngine->setPriority(priorityFromIndex(snapshot.monoPriorityIndex));
        voiceEngine->updateParameters(snapshot);

        int currentSample = 0;
        for (const auto metadata : midiMessages)
        {
            const auto eventSample = juce::jlimit(0, buffer.getNumSamples(), metadata.samplePosition);
            const auto samplesToProcess = eventSample - currentSample;
            if (samplesToProcess > 0)
                voiceEngine->render(buffer, currentSample, samplesToProcess);

            voiceEngine->processMidiMessage(metadata.getMessage());
            currentSample = eventSample;
        }

        const auto remaining = buffer.getNumSamples() - currentSample;
        if (remaining > 0)
            voiceEngine->render(buffer, currentSample, remaining);
    }

    for (int channel = 2; channel < buffer.getNumChannels(); ++channel)
        buffer.clear(channel, 0, buffer.getNumSamples());
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
