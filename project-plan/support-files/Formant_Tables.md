# Alpaca VST3 - Formant Frequency Tables

**Document Version:** 1.0
**Last Updated:** 2026-03-02
**Purpose:** Comprehensive formant data for vowel synthesis

---

## Table of Contents

1. [Overview](#overview)
2. [Adult Male Formants](#adult-male-formants)
3. [Adult Female Formants](#adult-female-formants)
4. [Child Formants](#child-formants)
5. [Bandwidth Values](#bandwidth-values)
6. [Amplitude Scaling](#amplitude-scaling)
7. [Implementation Guide](#implementation-guide)

---

## 1. Overview

### 1.1 Formant Theory

**Formants** are resonant frequencies of the vocal tract that give vowels their characteristic quality. The first three formants (F1, F2, F3) are most important for vowel recognition, while F4 and F5 add naturalness and timbre.

**Formant Characteristics:**
- **F1 (First Formant):** 250-850 Hz
  - Related to vowel height (tongue position)
  - Higher F1 = more open vowel (AH)
  - Lower F1 = more closed vowel (EE, OO)

- **F2 (Second Formant):** 800-2500 Hz
  - Related to vowel backness (tongue front/back)
  - Higher F2 = front vowel (EE)
  - Lower F2 = back vowel (OO)

- **F3 (Third Formant):** 2000-3500 Hz
  - Adds voice quality and naturalness
  - Less variable than F1/F2

- **F4, F5 (Higher Formants):** 3000-4500 Hz
  - Contribute to overall timbre
  - More stable across vowels

### 1.2 Vowel Set

The plugin implements 5 cardinal vowels:

1. **OO** (as in "boot") - /u/
2. **OH** (as in "boat") - /o/
3. **AH** (as in "bot") - /ɑ/
4. **EH** (as in "bet") - /ɛ/
5. **EE** (as in "beat") - /i/

These provide good coverage of the vowel space and smooth morphing characteristics.

---

## 2. Adult Male Formants

### 2.1 Default Voice (Tenor/Baritone)

| Vowel | F1 (Hz) | F2 (Hz) | F3 (Hz) | F4 (Hz) | F5 (Hz) |
|-------|---------|---------|---------|---------|---------|
| OO    | 300     | 870     | 2240    | 2990    | 3320    |
| OH    | 400     | 800     | 2600    | 2900    | 3300    |
| AH    | 750     | 1220    | 2600    | 2900    | 3300    |
| EH    | 600     | 1700    | 2600    | 2900    | 3300    |
| EE    | 270     | 2290    | 3010    | 3300    | 3750    |

**Sources:**
- Peterson & Barney (1952) - Classic formant measurements
- Hillenbrand et al. (1995) - Modern updated measurements
- Adjusted for musical synthesis context

### 2.2 Bass Voice (Optional)

Lower formants for deeper voice:

| Vowel | F1 (Hz) | F2 (Hz) | F3 (Hz) | F4 (Hz) | F5 (Hz) |
|-------|---------|---------|---------|---------|---------|
| OO    | 250     | 750     | 2050    | 2750    | 3050    |
| OH    | 350     | 700     | 2400    | 2700    | 3050    |
| AH    | 650     | 1100    | 2400    | 2700    | 3050    |
| EH    | 550     | 1500    | 2400    | 2700    | 3050    |
| EE    | 240     | 2100    | 2800    | 3100    | 3500    |

---

## 3. Adult Female Formants

### 3.1 Soprano/Alto Voice

Higher formants for female voice character:

| Vowel | F1 (Hz) | F2 (Hz) | F3 (Hz) | F4 (Hz) | F5 (Hz) |
|-------|---------|---------|---------|---------|---------|
| OO    | 370     | 950     | 2670    | 3350    | 3850    |
| OH    | 470     | 920     | 2810    | 3260    | 3850    |
| AH    | 860     | 1400    | 2810    | 3260    | 3850    |
| EH    | 690     | 2020    | 2810    | 3260    | 3850    |
| EE    | 330     | 2640    | 3350    | 3700    | 4250    |

**Scaling Factor from Male:**
Approximately 1.15x to 1.25x for F1-F3
Higher formants scale less dramatically

---

## 4. Child Formants

### 4.1 Child Voice (6-10 years)

Significantly higher formants:

| Vowel | F1 (Hz) | F2 (Hz) | F3 (Hz) | F4 (Hz) | F5 (Hz) |
|-------|---------|---------|---------|---------|---------|
| OO    | 450     | 1200    | 3200    | 4200    | 4800    |
| OH    | 550     | 1100    | 3500    | 4100    | 4800    |
| AH    | 1000    | 1600    | 3500    | 4100    | 4800    |
| EH    | 800     | 2200    | 3500    | 4100    | 4800    |
| EE    | 400     | 3000    | 4000    | 4500    | 5200    |

**Scaling Factor from Male:**
Approximately 1.5x to 1.7x for all formants

---

## 5. Bandwidth Values

### 5.1 Formant Bandwidths

Bandwidth determines the "sharpness" of each formant resonance. Wider bandwidth = less sharp peak.

**Default Bandwidths (Adult Male):**

| Formant | Bandwidth (Hz) | Q Factor* |
|---------|---------------|-----------|
| F1      | 80            | F1/80     |
| F2      | 100           | F2/100    |
| F3      | 140           | F3/140    |
| F4      | 180           | F4/180    |
| F5      | 250           | F5/250    |

*Q = Center Frequency / Bandwidth

### 5.2 Bandwidth Scaling Rules

**For different voice types:**
- Female: BW × 1.1
- Child: BW × 1.2
- Breathy voice: BW × 1.5 (more damped)
- Tense voice: BW × 0.8 (sharper resonances)

**Per-formant variation:**
```cpp
struct FormantBandwidths {
    float bw1 = 80.0f;
    float bw2 = 100.0f;
    float bw3 = 140.0f;
    float bw4 = 180.0f;
    float bw5 = 250.0f;

    void scale(float factor) {
        bw1 *= factor;
        bw2 *= factor;
        bw3 *= factor;
        bw4 *= factor;
        bw5 *= factor;
    }
};
```

---

## 6. Amplitude Scaling

### 6.1 Relative Amplitudes

Formants have different natural amplitudes. F1 is typically strongest.

**Default Amplitude Ratios (relative to F1 = 1.0):**

| Vowel | A1   | A2   | A3   | A4   | A5   |
|-------|------|------|------|------|------|
| OO    | 1.00 | 0.70 | 0.45 | 0.35 | 0.25 |
| OH    | 1.00 | 0.75 | 0.50 | 0.35 | 0.25 |
| AH    | 1.00 | 0.65 | 0.50 | 0.35 | 0.25 |
| EH    | 1.00 | 0.80 | 0.55 | 0.35 | 0.25 |
| EE    | 1.00 | 0.70 | 0.40 | 0.30 | 0.20 |

**Notes:**
- F2 is prominent in front vowels (EE, EH)
- F1 dominates in open vowels (AH)
- Higher formants (F4, F5) are always quieter

### 6.2 Amplitude Adjustment Formula

Apply spectral tilt and voice character:

```cpp
float adjustAmplitude(float baseAmp, int formantIndex, float tilt) {
    // Tilt: -1 (dark) to +1 (bright)
    // Higher formants are boosted/cut more
    float tiltAmount = tilt * formantIndex * 0.15f;
    return baseAmp * std::pow(10.0f, tiltAmount / 20.0f);  // dB to linear
}
```

### 6.3 Normalization

Prevent output clipping by normalizing total amplitude:

```cpp
float calculateNormalization(const float amplitudes[5]) {
    float sumSquared = 0.0f;
    for (int i = 0; i < 5; ++i) {
        sumSquared += amplitudes[i] * amplitudes[i];
    }
    float rms = std::sqrt(sumSquared / 5.0f);
    return 0.7f / rms;  // Target RMS level
}
```

---

## 7. Implementation Guide

### 7.1 Data Structure

```cpp
struct VowelFormants {
    std::array<float, 5> frequencies;
    std::array<float, 5> bandwidths;
    std::array<float, 5> amplitudes;

    VowelFormants() = default;

    VowelFormants(std::initializer_list<float> freqs,
                  std::initializer_list<float> bws,
                  std::initializer_list<float> amps) {
        std::copy(freqs.begin(), freqs.end(), frequencies.begin());
        std::copy(bws.begin(), bws.end(), bandwidths.begin());
        std::copy(amps.begin(), amps.end(), amplitudes.begin());
    }
};
```

### 7.2 Vowel Table Class

```cpp
class VowelTables {
public:
    enum class VoiceType {
        MaleTenor,
        MaleBass,
        Female,
        Child
    };

    enum class Vowel {
        OO = 0,
        OH = 1,
        AH = 2,
        EH = 3,
        EE = 4
    };

private:
    std::array<std::array<VowelFormants, 5>, 4> tables;

public:
    VowelTables() {
        initializeTables();
    }

    const VowelFormants& get(VoiceType voice, Vowel vowel) const {
        return tables[static_cast<int>(voice)][static_cast<int>(vowel)];
    }

    VowelFormants interpolate(VoiceType voice, float vowelParam) const {
        // vowelParam: 0.0 to 1.0
        float scaledParam = vowelParam * 4.0f;
        int index = std::clamp(static_cast<int>(scaledParam), 0, 3);
        float fraction = scaledParam - static_cast<float>(index);

        const auto& v1 = tables[static_cast<int>(voice)][index];
        const auto& v2 = tables[static_cast<int>(voice)][index + 1];

        VowelFormants result;
        for (int i = 0; i < 5; ++i) {
            result.frequencies[i] = v1.frequencies[i] +
                                    (v2.frequencies[i] - v1.frequencies[i]) * fraction;
            result.bandwidths[i] = v1.bandwidths[i] +
                                   (v2.bandwidths[i] - v1.bandwidths[i]) * fraction;
            result.amplitudes[i] = v1.amplitudes[i] +
                                   (v2.amplitudes[i] - v1.amplitudes[i]) * fraction;
        }
        return result;
    }

private:
    void initializeTables() {
        // Male Tenor (default)
        tables[0][0] = VowelFormants({300, 870, 2240, 2990, 3320},
                                      {80, 100, 140, 180, 250},
                                      {1.0, 0.7, 0.45, 0.35, 0.25});
        tables[0][1] = VowelFormants({400, 800, 2600, 2900, 3300},
                                      {80, 100, 140, 180, 250},
                                      {1.0, 0.75, 0.5, 0.35, 0.25});
        tables[0][2] = VowelFormants({750, 1220, 2600, 2900, 3300},
                                      {80, 100, 140, 180, 250},
                                      {1.0, 0.65, 0.5, 0.35, 0.25});
        tables[0][3] = VowelFormants({600, 1700, 2600, 2900, 3300},
                                      {80, 100, 140, 180, 250},
                                      {1.0, 0.8, 0.55, 0.35, 0.25});
        tables[0][4] = VowelFormants({270, 2290, 3010, 3300, 3750},
                                      {80, 100, 140, 180, 250},
                                      {1.0, 0.7, 0.4, 0.3, 0.2});

        // Male Bass
        tables[1][0] = VowelFormants({250, 750, 2050, 2750, 3050},
                                      {80, 100, 140, 180, 250},
                                      {1.0, 0.7, 0.45, 0.35, 0.25});
        // ... (similar pattern for other vowels)

        // Female
        tables[2][0] = VowelFormants({370, 950, 2670, 3350, 3850},
                                      {88, 110, 154, 198, 275},
                                      {1.0, 0.7, 0.45, 0.35, 0.25});
        // ... (similar pattern for other vowels)

        // Child
        tables[3][0] = VowelFormants({450, 1200, 3200, 4200, 4800},
                                      {96, 120, 168, 216, 300},
                                      {1.0, 0.7, 0.45, 0.35, 0.25});
        // ... (similar pattern for other vowels)
    }
};
```

### 7.3 Formant Shift Application

```cpp
VowelFormants applyFormantShift(const VowelFormants& base, float shiftSemitones) {
    float scale = std::pow(2.0f, shiftSemitones / 12.0f);

    VowelFormants shifted = base;
    for (int i = 0; i < 5; ++i) {
        shifted.frequencies[i] *= scale;
        shifted.bandwidths[i] *= scale;  // Scale BW proportionally
    }
    return shifted;
}
```

### 7.4 Usage Example

```cpp
// Initialize
VowelTables vowelTables;

// Get formants for specific voice and vowel position
float vowelParam = 0.35f;  // Between OH and AH
auto formants = vowelTables.interpolate(VowelTables::VoiceType::MaleTenor,
                                        vowelParam);

// Apply formant shift
float formantShift = 4.0f;  // +4 semitones (toward female)
formants = applyFormantShift(formants, formantShift);

// Update formant bank
formantBank.updateFormants(formants.frequencies.data(),
                          formants.bandwidths.data(),
                          formants.amplitudes.data(),
                          sampleRate);
```

---

## References

1. **Peterson, G. E., & Barney, H. L. (1952).** "Control methods used in a study of the vowels." *Journal of the Acoustical Society of America*, 24(2), 175-184.

2. **Hillenbrand, J., Getty, L. A., Clark, M. J., & Wheeler, K. (1995).** "Acoustic characteristics of American English vowels." *Journal of the Acoustical Society of America*, 97(5), 3099-3111.

3. **Fant, G. (1960).** *Acoustic Theory of Speech Production.* The Hague: Mouton.

4. **Klatt, D. H., & Klatt, L. C. (1990).** "Analysis, synthesis, and perception of voice quality variations among female and male talkers." *Journal of the Acoustical Society of America*, 87(2), 820-857.

5. **Rodet, X. (1984).** "Time-domain formant-wave-function synthesis." *Computer Music Journal*, 8(3), 9-14.

---

## Appendix: Quick Reference

**Formant Shift Guidelines:**
- Male → Female: +4 to +6 semitones
- Female → Male: -4 to -6 semitones
- Adult → Child: +6 to +12 semitones
- Subtle character change: ±2 semitones

**Vowel Parameter Mapping:**
- 0.00 - 0.25: OO → OH
- 0.25 - 0.50: OH → AH
- 0.50 - 0.75: AH → EH
- 0.75 - 1.00: EH → EE

**Formant Priority for Limited Resources:**
If CPU is constrained, implement formants in priority order:
1. F1, F2 (essential for vowel recognition)
2. F3 (adds naturalness)
3. F4, F5 (can be omitted if necessary)

**Typical Q Factors:**
- F1: 3-8
- F2: 8-15
- F3: 15-25
- F4, F5: 15-25

---

**END OF FORMANT TABLES DOCUMENT**
