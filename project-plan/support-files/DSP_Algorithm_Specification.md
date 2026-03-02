# Alpaca VST3 - DSP Algorithm Specification

**Document Version:** 1.0
**Last Updated:** 2026-03-02
**Purpose:** Detailed mathematical and algorithmic specifications for all DSP components

---

## Table of Contents

1. [Overview](#overview)
2. [Pitch Processing](#pitch-processing)
3. [Excitation Generation](#excitation-generation)
4. [Formant Synthesis](#formant-synthesis)
5. [Vowel Morphing](#vowel-morphing)
6. [Tone Shaping](#tone-shaping)
7. [Ensemble Effect](#ensemble-effect)
8. [Delay Effect](#delay-effect)
9. [Reverb Effect](#reverb-effect)
10. [Parameter Smoothing](#parameter-smoothing)
11. [Numerical Considerations](#numerical-considerations)

---

## 1. Overview

### 1.1 Signal Flow

```
MIDI Input → Note Manager → Pitch Processing
                                ↓
                          Excitation Generator
                                ↓
                          Formant Filter Bank
                                ↓
                          Amplitude Envelope
                                ↓
                          Spectral Tilt EQ
                                ↓
                          Ensemble Effect
                                ↓
                          Stereo Delay
                                ↓
                          Reverb
                                ↓
                          Output Gain → Audio Output
```

### 1.2 Design Principles

- **Sample-rate independence:** All algorithms adapt to sample rate
- **Real-time safe:** No allocations, no locks in audio thread
- **Numerically stable:** Proper coefficient limiting and denormal prevention
- **Efficient:** Minimize coefficient updates, use control-rate where appropriate

### 1.3 Processing Rates

- **Audio Rate:** Per-sample processing (filter state updates)
- **Control Rate:** Per-block or every N samples (coefficient updates)
- **Parameter Rate:** When user changes parameter (smoothed over time)

---

## 2. Pitch Processing

### 2.1 MIDI Note to Frequency

Convert MIDI note number `n` to frequency in Hz:

```
f₀ = 440 × 2^((n - 69) / 12)
```

Where:
- `n` = MIDI note number (0-127)
- `69` = MIDI note number for A4 (440 Hz)
- `f₀` = fundamental frequency in Hz

**Implementation:**
```cpp
float midiNoteToFrequency(int note, float pitchBend = 0.0f) {
    float noteFloat = static_cast<float>(note) + pitchBend;
    return 440.0f * std::pow(2.0f, (noteFloat - 69.0f) / 12.0f);
}
```

### 2.2 Portamento (Glide)

Exponential smoothing for pitch transitions:

```
f(n) = f_target + (f_prev - f_target) × e^(-1 / (τ × Fs))
```

Where:
- `τ` = glide time in seconds
- `Fs` = sample rate in Hz
- `f_target` = target frequency
- `f_prev` = previous frequency

**Coefficient calculation:**
```cpp
float calculateGlideCoeff(float glideTimeMs, float sampleRate) {
    if (glideTimeMs <= 0.0f) return 1.0f;
    float tau = glideTimeMs / 1000.0f;
    return 1.0f - std::exp(-1.0f / (tau * sampleRate));
}
```

**Per-sample update:**
```cpp
currentFreq = targetFreq + (currentFreq - targetFreq) * (1.0f - glideCoeff);
```

### 2.3 Vibrato

Low-frequency sinusoidal modulation of pitch:

```
LFO(n) = sin(2π × fLFO × n / Fs)
fmod(n) = f₀ × 2^(depth × LFO(n) / 12)
```

Where:
- `fLFO` = vibrato rate in Hz (0.1-12 Hz)
- `depth` = vibrato depth in semitones (0-2 typical)
- `n` = sample index

**Implementation:**
```cpp
class VibratoLFO {
    float phase = 0.0f;
    float phaseIncrement = 0.0f;

public:
    void setFrequency(float hz, float sampleRate) {
        phaseIncrement = hz / sampleRate;
    }

    float processSample() {
        float output = std::sin(2.0f * M_PI * phase);
        phase += phaseIncrement;
        if (phase >= 1.0f) phase -= 1.0f;
        return output;
    }
};

// Apply vibrato
float applyVibrato(float baseFreq, float lfoValue, float depthSemitones) {
    return baseFreq * std::pow(2.0f, (depthSemitones * lfoValue) / 12.0f);
}
```

### 2.4 Pitch Bend

MIDI pitch bend (14-bit value 0-16383, center 8192):

```
bendSemitones = (bendValue - 8192) / 8192 × bendRange
fBent = f₀ × 2^(bendSemitones / 12)
```

**Implementation:**
```cpp
float applyPitchBend(float baseFreq, int bendValue, float bendRangeSemitones) {
    float bendNorm = (static_cast<float>(bendValue) - 8192.0f) / 8192.0f;
    float semitones = bendNorm * bendRangeSemitones;
    return baseFreq * std::pow(2.0f, semitones / 12.0f);
}
```

---

## 3. Excitation Generation

### 3.1 Bandlimited Sawtooth (PolyBLEP)

PolyBLEP (Polynomial Band-Limited Step) reduces aliasing:

**Basic sawtooth:**
```
saw(t) = 2 × phase - 1
```

**PolyBLEP correction function:**
```cpp
float polyBlep(float t) {
    if (t < 1.0f) {
        return t + t - t * t - 1.0f;
    } else if (t > 1.0f) {
        t = t - 1.0f;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}
```

**Bandlimited sawtooth:**
```cpp
class BandlimitedSaw {
    float phase = 0.0f;
    float phaseIncrement = 0.0f;

public:
    void setFrequency(float freq, float sampleRate) {
        phaseIncrement = freq / sampleRate;
    }

    float processSample() {
        float value = 2.0f * phase - 1.0f;

        // Apply PolyBLEP at discontinuity
        value -= polyBlep(phase / phaseIncrement);

        phase += phaseIncrement;
        while (phase >= 1.0f) phase -= 1.0f;

        return value;
    }

private:
    float polyBlep(float t) {
        if (t < 1.0f) {
            t = t + t - t * t - 1.0f;
            return -t;
        } else if (t > 1.0f - phaseIncrement) {
            t = (t - 1.0f) / phaseIncrement;
            return t * t + t + t + 1.0f;
        }
        return 0.0f;
    }
};
```

### 3.2 Alternative: Impulse Train

Simpler excitation for formant synthesis:

```cpp
class ImpulseTrain {
    float phase = 0.0f;
    float phaseIncrement = 0.0f;

public:
    void setFrequency(float freq, float sampleRate) {
        phaseIncrement = freq / sampleRate;
    }

    float processSample() {
        float output = (phase < phaseIncrement) ? 1.0f : 0.0f;
        phase += phaseIncrement;
        if (phase >= 1.0f) phase -= 1.0f;
        return output;
    }
};
```

---

## 4. Formant Synthesis

### 4.1 Formant Filter Bank Architecture

Parallel bandpass filters, each tuned to a formant frequency:

```
y(n) = Σ(i=1 to N) Ai × BPFi(x(n))
```

Where:
- `N` = number of formants (3-5)
- `Ai` = amplitude/gain for formant i
- `BPFi` = bandpass filter for formant i

### 4.2 Biquad Bandpass Filter

Transfer function:
```
H(z) = (b₀ + b₁z⁻¹ + b₂z⁻²) / (1 + a₁z⁻¹ + a₂z⁻²)
```

**Coefficient calculation (Audio EQ Cookbook):**

For bandpass filter with center frequency `F` and bandwidth `BW`:

```cpp
struct BiquadCoeffs {
    float b0, b1, b2, a1, a2;
};

BiquadCoeffs calculateBandpassCoeffs(float centerFreq, float bandwidth,
                                      float sampleRate) {
    // Prewarp frequencies
    float omega = 2.0f * M_PI * centerFreq / sampleRate;
    float bw = 2.0f * M_PI * bandwidth / sampleRate;

    float Q = omega / bw;  // Q factor

    float alpha = std::sin(omega) / (2.0f * Q);
    float cos_omega = std::cos(omega);

    BiquadCoeffs c;
    float a0 = 1.0f + alpha;

    c.b0 = alpha / a0;
    c.b1 = 0.0f;
    c.b2 = -alpha / a0;
    c.a1 = -2.0f * cos_omega / a0;
    c.a2 = (1.0f - alpha) / a0;

    return c;
}
```

**Filter processing:**
```cpp
class Biquad {
    float x1 = 0.0f, x2 = 0.0f;  // Input history
    float y1 = 0.0f, y2 = 0.0f;  // Output history
    BiquadCoeffs c;

public:
    void setCoefficients(const BiquadCoeffs& coeffs) {
        c = coeffs;
    }

    float processSample(float input) {
        float output = c.b0 * input + c.b1 * x1 + c.b2 * x2
                     - c.a1 * y1 - c.a2 * y2;

        // Update history
        x2 = x1; x1 = input;
        y2 = y1; y1 = output;

        // Denormal prevention
        if (std::abs(output) < 1e-15f) output = 0.0f;

        return output;
    }

    void reset() {
        x1 = x2 = y1 = y2 = 0.0f;
    }
};
```

### 4.3 Formant Bank Implementation

```cpp
class FormantBank {
    static constexpr int NUM_FORMANTS = 5;
    std::array<Biquad, NUM_FORMANTS> filters;
    std::array<float, NUM_FORMANTS> gains;

public:
    void updateFormants(const float* frequencies, const float* bandwidths,
                        const float* amplitudes, float sampleRate) {
        for (int i = 0; i < NUM_FORMANTS; ++i) {
            auto coeffs = calculateBandpassCoeffs(frequencies[i],
                                                   bandwidths[i],
                                                   sampleRate);
            filters[i].setCoefficients(coeffs);
            gains[i] = amplitudes[i];
        }
    }

    float processSample(float input) {
        float output = 0.0f;
        for (int i = 0; i < NUM_FORMANTS; ++i) {
            output += filters[i].processSample(input) * gains[i];
        }
        return output;
    }

    void reset() {
        for (auto& filter : filters) {
            filter.reset();
        }
    }
};
```

### 4.4 Formant Frequency Tables

Research-based formant values for vowels (adult male voice):

| Vowel | F1 (Hz) | F2 (Hz) | F3 (Hz) | F4 (Hz) | F5 (Hz) |
|-------|---------|---------|---------|---------|---------|
| OO (boot) | 300 | 870 | 2240 | 2990 | 3320 |
| OH (boat) | 400 | 800 | 2600 | 2900 | 3300 |
| AH (bot) | 750 | 1220 | 2600 | 2900 | 3300 |
| EH (bet) | 600 | 1700 | 2600 | 2900 | 3300 |
| EE (beat) | 270 | 2290 | 3010 | 3300 | 3750 |

**Bandwidth values (typical):**
- F1: 60-100 Hz
- F2: 70-120 Hz
- F3: 110-170 Hz
- F4: 140-200 Hz
- F5: 200-300 Hz

**Amplitude values (relative):**
- F1: 1.0 (reference)
- F2: 0.6-0.8
- F3: 0.4-0.6
- F4: 0.3-0.5
- F5: 0.2-0.4

---

## 5. Vowel Morphing

### 5.1 Linear Interpolation

For vowel parameter `v` ∈ [0, 1], interpolate between vowel targets:

```
Fi(v) = Fi(V1) + (Fi(V2) - Fi(V1)) × v
BWi(v) = BWi(V1) + (BWi(V2) - BWi(V1)) × v
Ai(v) = Ai(V1) + (Ai(V2) - Ai(V1)) × v
```

**Multi-vowel interpolation:**
Map vowel parameter to 5 vowels:
- v ∈ [0.0, 0.25]: OO → OH
- v ∈ [0.25, 0.5]: OH → AH
- v ∈ [0.5, 0.75]: AH → EH
- v ∈ [0.75, 1.0]: EH → EE

```cpp
struct VowelFormants {
    float F[5], BW[5], A[5];
};

VowelFormants interpolateVowels(float vowelParam,
                                  const std::array<VowelFormants, 5>& vowels) {
    float scaledParam = vowelParam * 4.0f;  // Map to [0, 4]
    int index = static_cast<int>(scaledParam);
    index = std::clamp(index, 0, 3);  // Ensure valid range

    float fraction = scaledParam - static_cast<float>(index);

    VowelFormants result;
    for (int i = 0; i < 5; ++i) {
        result.F[i] = vowels[index].F[i] +
                      (vowels[index+1].F[i] - vowels[index].F[i]) * fraction;
        result.BW[i] = vowels[index].BW[i] +
                       (vowels[index+1].BW[i] - vowels[index].BW[i]) * fraction;
        result.A[i] = vowels[index].A[i] +
                      (vowels[index+1].A[i] - vowels[index].A[i]) * fraction;
    }
    return result;
}
```

### 5.2 Formant Shift

Scale all formant frequencies to change voice character:

```
Fi_shifted = Fi × 2^(shift_semitones / 12)
```

**Male to female shift:** +4 to +6 semitones
**Female to male shift:** -4 to -6 semitones
**Child voice:** +6 to +12 semitones

```cpp
VowelFormants applyFormantShift(const VowelFormants& input, float shiftSemitones) {
    float scale = std::pow(2.0f, shiftSemitones / 12.0f);
    VowelFormants result = input;
    for (int i = 0; i < 5; ++i) {
        result.F[i] *= scale;
        // Bandwidth can scale proportionally or stay fixed
        result.BW[i] *= scale;  // Proportional scaling
    }
    return result;
}
```

---

## 6. Tone Shaping

### 6.1 Spectral Tilt Filter

First-order shelving filter for brightness control:

**High shelf boost/cut:**
```
y[n] = x[n] + g × (x[n] - LPF(x[n]))
```

Where `g` controls brightness (>0 brightens, <0 darkens).

**Implementation (simple one-pole):**
```cpp
class TiltFilter {
    float state = 0.0f;
    float coeff = 0.0f;
    float gain = 0.0f;

public:
    void setParameters(float tiltAmount, float sampleRate) {
        // Tilt amount: -1 to +1
        gain = tiltAmount * 2.0f;

        // Cutoff around 1 kHz
        float cutoff = 1000.0f;
        coeff = 1.0f - std::exp(-2.0f * M_PI * cutoff / sampleRate);
    }

    float processSample(float input) {
        // One-pole LPF
        state = state + coeff * (input - state);

        // High-pass = input - LPF
        float highpass = input - state;

        // Apply tilt
        return input + gain * highpass;
    }
};
```

### 6.2 Breath Noise Component

Optional filtered noise for realism:

```cpp
class BreathNoise {
    float noiseLevel = 0.0f;
    Biquad hpf;  // High-pass to remove low-frequency rumble

public:
    void setAmount(float amount, float sampleRate) {
        noiseLevel = amount * 0.15f;  // Scale to reasonable level

        // HPF at 500 Hz
        auto coeffs = calculateHighpassCoeffs(500.0f, 0.707f, sampleRate);
        hpf.setCoefficients(coeffs);
    }

    float processSample() {
        // White noise
        float noise = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;

        // Filter and scale
        return hpf.processSample(noise) * noiseLevel;
    }
};
```

---

## 7. Ensemble Effect

### 7.1 Micro-Detune Algorithm

Create subtle stereo width using pitch modulation and delay:

```cpp
class Ensemble {
    std::array<float, 2048> delayBuffer;
    int writePos = 0;
    float lfoPhase = 0.0f;
    float lfoRate = 0.0f;
    float amount = 0.0f;
    float width = 0.0f;
    float detuneCents = 0.0f;
    float sampleRate = 48000.0f;

public:
    void setParameters(float amt, float stereoWidth, float detune, float sr) {
        amount = amt;
        width = stereoWidth;
        detuneCents = detune;
        sampleRate = sr;
        lfoRate = 0.25f / sr;  // Slow LFO for natural movement
    }

    void processStereo(float* left, float* right, int numSamples) {
        for (int i = 0; i < numSamples; ++i) {
            float input = (*left + *right) * 0.5f;

            // Write to delay buffer
            delayBuffer[writePos] = input;

            // LFO for delay modulation
            float lfo = std::sin(2.0f * M_PI * lfoPhase);
            lfoPhase += lfoRate;
            if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;

            // Modulated delay time (5-15 ms)
            float baseDelay = 10.0f * sampleRate / 1000.0f;
            float modDepth = 5.0f * sampleRate / 1000.0f;

            float delayTimeL = baseDelay + modDepth * lfo * amount;
            float delayTimeR = baseDelay - modDepth * lfo * amount;

            // Read from delay buffer with interpolation
            float delayedL = readDelayBufferLinear(delayTimeL);
            float delayedR = readDelayBufferLinear(delayTimeR);

            // Mix dry and wet
            *left = input + delayedL * amount * width;
            *right = input + delayedR * amount * width;

            writePos = (writePos + 1) % delayBuffer.size();
            left++; right++;
        }
    }

private:
    float readDelayBufferLinear(float delaySamples) {
        float readPos = writePos - delaySamples;
        while (readPos < 0.0f) readPos += delayBuffer.size();

        int pos0 = static_cast<int>(readPos);
        int pos1 = (pos0 + 1) % delayBuffer.size();
        float frac = readPos - pos0;

        return delayBuffer[pos0] + frac * (delayBuffer[pos1] - delayBuffer[pos0]);
    }
};
```

---

## 8. Delay Effect

### 8.1 Circular Buffer Implementation

```cpp
class CircularBuffer {
    std::vector<float> buffer;
    int writePos = 0;
    int size = 0;

public:
    void setSize(int samples) {
        size = samples;
        buffer.resize(size, 0.0f);
        writePos = 0;
    }

    void write(float sample) {
        buffer[writePos] = sample;
        writePos = (writePos + 1) % size;
    }

    float read(float delaySamples) const {
        float readPos = writePos - delaySamples;
        while (readPos < 0.0f) readPos += size;
        while (readPos >= size) readPos -= size;

        // Linear interpolation
        int pos0 = static_cast<int>(readPos);
        int pos1 = (pos0 + 1) % size;
        float frac = readPos - pos0;

        return buffer[pos0] + frac * (buffer[pos1] - buffer[pos0]);
    }

    void clear() {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
    }
};
```

### 8.2 Delay Modes

**Mono Delay:**
```
L_out = L_in + feedback × L_delayed
R_out = R_in + feedback × R_delayed
```

**Stereo Delay:**
```
L_out = L_in + feedback × L_delayed
R_out = R_in + feedback × R_delayed
(Independent left and right)
```

**Ping-Pong Delay:**
```
L_out = L_in + feedback × R_delayed
R_out = R_in + feedback × L_delayed
(Cross-feedback)
```

### 8.3 Damping Filter

Low-pass filter in feedback path:

```cpp
class DampingFilter {
    float state = 0.0f;
    float coeff = 0.0f;

public:
    void setDamping(float amount, float sampleRate) {
        // amount: 0 = no damping, 1 = heavy damping
        float cutoff = 20000.0f * (1.0f - amount * 0.9f);
        coeff = 1.0f - std::exp(-2.0f * M_PI * cutoff / sampleRate);
    }

    float processSample(float input) {
        state = state + coeff * (input - state);
        return state;
    }
};
```

### 8.4 Tempo Sync

Convert musical divisions to delay time in samples:

```cpp
float tempoSyncToSamples(float bpm, float division, float sampleRate) {
    // division: 1.0 = quarter note, 0.5 = eighth, etc.
    float beatsPerSecond = bpm / 60.0f;
    float secondsPerBeat = 1.0f / beatsPerSecond;
    float delaySeconds = secondsPerBeat * division;
    return delaySeconds * sampleRate;
}
```

---

## 9. Reverb Effect

### 9.1 Feedback Delay Network (FDN)

Four delay lines with feedback matrix:

```
[y1]   [d1]   [a11 a12 a13 a14]   [y1]
[y2] = [d2] + [a21 a22 a23 a24] × [y2]
[y3]   [d3]   [a31 a32 a33 a34]   [y3]
[y4]   [d4]   [a41 a42 a43 a44]   [y4]
```

**Householder feedback matrix (diffusion):**
```
A = I - 2vvᵀ
where v = [0.5, 0.5, 0.5, 0.5]
```

Results in:
```
A = [-0.5  0.5  0.5  0.5]
    [ 0.5 -0.5  0.5  0.5]
    [ 0.5  0.5 -0.5  0.5]
    [ 0.5  0.5  0.5 -0.5]
```

### 9.2 Delay Line Lengths

Prime number lengths for dense, non-periodic response:

```cpp
std::array<int, 4> calculateDelayLengths(float size, float sampleRate) {
    // Base lengths for size = 0.5
    std::array<int, 4> baseLengths = {1637, 1811, 1987, 2203};

    // Scale by size parameter
    float scale = 0.5f + size * 1.5f;  // 0.5 to 2.0

    std::array<int, 4> lengths;
    for (int i = 0; i < 4; ++i) {
        lengths[i] = static_cast<int>(baseLengths[i] * scale * sampleRate / 48000.0f);
    }
    return lengths;
}
```

### 9.3 Decay Control

Feedback gain controls decay time:

```
RT60 = -3 × delayLength / (sampleRate × log10(g))
```

Solve for `g`:
```
g = 10^(-3 × delayLength / (sampleRate × RT60))
```

```cpp
float calculateReverbGain(float decaySeconds, int delayLength, float sampleRate) {
    float rt60 = decaySeconds;
    float exponent = -3.0f * delayLength / (sampleRate * rt60);
    float gain = std::pow(10.0f, exponent);
    return std::clamp(gain, 0.0f, 0.999f);
}
```

---

## 10. Parameter Smoothing

### 10.1 One-Pole Smoothing

Exponential approach to target:

```
y[n] = y[n-1] + α × (target - y[n-1])
```

Where:
```
α = 1 - e^(-1 / (τ × Fs))
```

**Implementation:**
```cpp
class ParameterSmoother {
    float current = 0.0f;
    float target = 0.0f;
    float coeff = 0.0f;

public:
    void setTimeConstant(float timeMs, float sampleRate) {
        float tau = timeMs / 1000.0f;
        coeff = 1.0f - std::exp(-1.0f / (tau * sampleRate));
    }

    void setTarget(float newTarget) {
        target = newTarget;
    }

    float getNext() {
        current = current + coeff * (target - current);
        return current;
    }

    void reset(float value) {
        current = target = value;
    }
};
```

### 10.2 Linear Ramp

Constant-rate change over N samples:

```cpp
class LinearRamp {
    float current = 0.0f;
    float target = 0.0f;
    float increment = 0.0f;
    int samplesRemaining = 0;

public:
    void setTarget(float newTarget, int rampSamples) {
        target = newTarget;
        samplesRemaining = rampSamples;
        if (rampSamples > 0) {
            increment = (target - current) / rampSamples;
        } else {
            current = target;
            increment = 0.0f;
        }
    }

    float getNext() {
        if (samplesRemaining > 0) {
            current += increment;
            samplesRemaining--;
        }
        return current;
    }
};
```

---

## 11. Numerical Considerations

### 11.1 Denormal Prevention

Flush very small values to zero:

```cpp
inline float flushDenormal(float value) {
    if (std::abs(value) < 1e-15f) return 0.0f;
    return value;
}
```

Or use CPU flush-to-zero mode:
```cpp
#include <xmmintrin.h>
_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
```

### 11.2 Feedback Limiting

Prevent instability in feedback loops:

```cpp
float safeFeedback = std::clamp(userFeedback, 0.0f, 0.98f);
```

### 11.3 Filter Stability

Limit Q factor to prevent resonance explosion:

```cpp
float safeQ = std::clamp(Q, 0.1f, 50.0f);
```

Check pole locations are inside unit circle (for IIR filters).

### 11.4 Coefficient Update Strategy

Update filter coefficients at control rate, not audio rate:

```cpp
void processBlock(float* buffer, int numSamples) {
    // Update coefficients once per block
    if (parametersChanged) {
        updateFilterCoefficients();
        parametersChanged = false;
    }

    // Process samples with stable coefficients
    for (int i = 0; i < numSamples; ++i) {
        buffer[i] = filter.processSample(buffer[i]);
    }
}
```

---

## Conclusion

This specification provides the mathematical foundation and implementation details for all DSP components of the Alpaca plugin. All algorithms are designed for real-time performance, numerical stability, and high audio quality.

**Key Takeaways:**
- Sample-rate independent design throughout
- Proper parameter smoothing prevents audio artifacts
- Numerically stable filter implementations
- Efficient coefficient update strategies
- Clear signal flow from input to output

For implementation details and code integration, refer to the JUCE Architecture Blueprint and the main project plan.
