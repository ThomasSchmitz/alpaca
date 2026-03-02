# MonkSynth VST3

## DSP Math & Signal Processing Specification

This document defines the mathematical and DSP architecture for the
MonkSynth instrument. It is implementation-oriented and intended for
engineering use.

------------------------------------------------------------------------

# 1. Signal Flow Overview

Final processing order:

Excitation → Formant Bank → Spectral Tilt → Ensemble → Delay → Reverb →
Output Gain

All processing must be sample-rate independent and free of dynamic
memory allocation in the audio thread.

------------------------------------------------------------------------

# 2. Pitch & Portamento

## 2.1 MIDI Note to Frequency

Given MIDI note number n:

f0 = 440 \* 2\^((n - 69) / 12)

------------------------------------------------------------------------

## 2.2 Portamento (Glide)

Use exponential smoothing for pitch glide:

f(t) = f_target + (f_prev - f_target) \* exp(-1 / (tau \* Fs))

Where: - tau = glide time in seconds - Fs = sample rate

Alternative: Use linear ramp over glide_time samples.

------------------------------------------------------------------------

# 3. Vibrato

Low-frequency oscillator:

LFO(t) = sin(2π f_LFO t)

Pitch modulation:

f_mod = f0 \* 2\^(depth \* LFO(t) / 12)

Depth measured in semitones.

------------------------------------------------------------------------

# 4. Excitation Signal

Use bandlimited saw or impulse train.

## 4.1 PolyBLEP Saw

Basic saw:

x(t) = 2 \* phase - 1

Apply PolyBLEP correction at discontinuities to prevent aliasing.

------------------------------------------------------------------------

# 5. Formant Synthesis

Two supported models:

A)  Parallel Bandpass Filter Bank\
B)  FOF (Formant Wave Function)

------------------------------------------------------------------------

# 5A. Parallel Bandpass Filter Model

Each formant implemented as biquad bandpass filter.

Transfer function:

H(z) = (b0 + b1 z\^-1 + b2 z\^-2) / (1 + a1 z\^-1 + a2 z\^-2)

Center frequency: F_i\
Bandwidth: BW_i

Q factor:

Q_i = F_i / BW_i

Use bilinear transform to compute coefficients.

------------------------------------------------------------------------

# 5B. FOF Model (Recommended)

Each formant generates grains at pitch period T0 = 1 / f0.

Grain waveform:

g_i(t) = A_i \* sin(2π F_i t) \* e\^(-t / tau_i)

Where: - F_i = formant frequency - tau_i = decay constant derived from
bandwidth - A_i = amplitude

Bandwidth relationship:

BW_i ≈ 1 / (π tau_i)

Sum across all formants:

y(t) = Σ g_i(t - kT0)

for all active grains k.

------------------------------------------------------------------------

# 6. Vowel Morphing

Each vowel V has:

F_i(V), BW_i(V), A_i(V)

For continuous morph parameter m ∈ \[0,1\]:

Linear interpolation:

F_i(m) = (1 - m) F_i(V1) + m F_i(V2)

Apply smoothing:

p\[n\] = p\[n-1\] + alpha (target - p\[n-1\])

alpha = 1 - exp(-1 / (tau \* Fs))

------------------------------------------------------------------------

# 7. Spectral Tilt

First-order shelving filter or simple tilt:

y\[n\] = x\[n\] + g \* (x\[n\] - LPF(x\[n\]))

Where g controls brightness.

------------------------------------------------------------------------

# 8. Ensemble (Micro-Detune)

Duplicate signal paths with slight frequency offsets:

f0_L = f0 \* 2\^(detune / 1200) f0_R = f0 \* 2\^(-detune / 1200)

Optional small delay offset (5--20 ms).

------------------------------------------------------------------------

# 9. Delay

Circular buffer implementation.

## 9.1 Basic Delay

y\[n\] = x\[n\] + feedback \* y\[n - D\]

Where D = delay_samples.

Ping-pong routing:

Left feedback → Right input\
Right feedback → Left input

Add lowpass damping inside feedback loop:

y_fb = LPF(y\[n - D\])

------------------------------------------------------------------------

# 10. Reverb

Use Schroeder or FDN structure.

## 10.1 Comb Filter

y\[n\] = x\[n\] + g \* y\[n - D\]

## 10.2 Allpass Filter

y\[n\] = -g x\[n\] + x\[n - D\] + g y\[n - D\]

Shimmer mode: Insert pitch shifter in feedback loop (+12 semitones).

------------------------------------------------------------------------

# 11. Parameter Smoothing

For all user-controlled parameters:

One-pole smoothing:

y\[n\] = y\[n-1\] + alpha (x\[n\] - y\[n-1\])

alpha = 1 - exp(-1 / (tau \* Fs))

Typical tau: - Pitch: 5--20 ms - Formants: 10--30 ms - FX parameters:
20--50 ms

------------------------------------------------------------------------

# 12. Denormal Prevention

Add DC offset or use:

if (abs(sample) \< 1e-15) sample = 0

Or enable CPU flush-to-zero.

------------------------------------------------------------------------

# 13. Oversampling (Optional)

To reduce aliasing in FOF or distortion stages:

-   2x or 4x oversampling
-   Polyphase IIR or FIR halfband filters
-   Downsample after nonlinear processing

------------------------------------------------------------------------

# 14. Numerical Stability

-   Clamp feedback \< 0.98
-   Avoid Q \> 50
-   Normalize formant gains to prevent clipping
-   Use double precision internally if CPU allows

------------------------------------------------------------------------

# 15. Performance Targets

-   No heap allocations in processBlock
-   Pre-allocate delay buffers
-   SIMD optimize biquad where possible
-   Avoid recalculating coefficients per sample

------------------------------------------------------------------------

End of DSP Specification
