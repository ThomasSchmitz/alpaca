# MonkSynth VST3

## UI Component & Implementation Specification (JUCE)

This document specifies the **JUCE UI component tree**, layout approach,
scaling, performance rules, animation strategy, and MIDI-learn UX
wiring.

Primary goals: - Clean "monk vibe" UI with a modern aesthetic - Clear
performance focus (Monk + XY pad) with collapsible modern FX - High-DPI
friendly and scalable - Real-time safe: UI never touches audio thread
directly - Complete, consistent binding to parameters and MIDI learn

------------------------------------------------------------------------

# 1. UI Principles

## 1.1 Visual Hierarchy

1.  Monk view (identity / feedback)
2.  XY pad (primary interaction)
3.  Core knobs (glide, voice, vibrato)
4.  FX panels (tone, ensemble, delay, reverb)

## 1.2 Minimalism Rules

-   No overcrowded knob arrays.
-   Collapsible panels hide complexity.
-   Flat, calm theme (no skeuomorphic metal textures).
-   Avoid copyrighted imagery; monk character must be original.

------------------------------------------------------------------------

# 2. Base Size, Grid, Scaling

## 2.1 Base Layout

-   Base UI size: **1200 × 720**
-   Outer margin: **24 px**
-   Spacing grid: **8 px** increments

## 2.2 Scaling Strategy

Use a single UI scale factor: - Default: 1.0 - Allowed range: 0.75--1.5
(or host DPI-dependent)

Implementation: - Store **base rectangles** in a `Layout` utility
(pixel-grid). - Multiply base rectangles by scale factor at layout
time. - Prefer `setTransform(AffineTransform::scale(scale))` for
subtrees only if needed; otherwise compute scaled bounds per component.

Recommended: - Provide a UI setting parameter (non-automatable)
`ui_scale` OR infer from display scale and allow manual override.

------------------------------------------------------------------------

# 3. Component Tree

File paths are illustrative (match repo layout).

## 3.1 Top Level

`MonkSynthAudioProcessorEditor` - `HeaderBar` - `MainContent` -
`PerformanceColumn` - `MonkView` - `XYPad` - `CoreKnobRow` -
`FxColumn` - `TonePanel` - `EnsemblePanel` (collapsible) - `DelayPanel`
(collapsible) - `ReverbPanel` (collapsible) - `ModPanel` (optional,
future) - `MidiLearnOverlay` (topmost, optional)

## 3.2 Component Responsibilities

### HeaderBar

-   Preset dropdown
-   A/B
-   MIDI Learn toggle
-   Settings (scale, MIDI options shortcut)
-   Optional CPU meter (UI-side estimate only)

### MonkView

-   Visual feedback:
    -   mouth open based on amplitude meter (from processor)
    -   "expression" based on vowel
    -   head tilt or glow based on pitch / vibrato
-   Never blocks primary interaction.
-   Should not repaint at audio rate.

### XYPad

-   Primary XY controller
-   Y axis → `vowel` parameter
-   X axis → optional pitch offset / absolute pitch depending on
    `pitch_control_mode`
-   Smooth pointer tracking
-   Optional "snap markers" showing vowel regions
-   Supports right-click context for MIDI Learn on associated parameters

### CoreKnobRow

-   Glide (`glide_ms`)
-   Voice (`voice_character`)
-   Vibrato (`vibrato_depth`), with a small secondary control or
    shift-click for `vibrato_rate_hz`

### FxColumn Panels

-   Panels are collapsible with animated height (optional) or immediate
    collapse (v1)
-   Each panel contains 3--6 primary controls, no more.
-   Advanced controls (sync division, damping) can be "foldout row"
    inside panel.

### MidiLearnOverlay

-   Shows "Learning..." status
-   Displays last received MIDI message summary
-   Highlights currently armed target control
-   Optional: list of current mappings (shortcut opens mapping editor)

------------------------------------------------------------------------

# 4. Layout Specification (Implementation)

## 4.1 Layout Engine

-   Macro layout: custom rect math or `FlexBox` (choose one and stick to
    it).
-   Recommendation: **rect math** for top-level due to fixed grid spec
    and collapsible panels.

### Layout.h

Provide: -
`struct UiRects { juce::Rectangle<int> header, leftCol, rightCol, monk, xy, knobRow, tone, ensemble, delay, reverb; };` -
`UiRects computeRects(int width, int height, float scale);`

Keep all rects aligned to 8 px grid after scaling: -
`snapToGrid(rect, grid=8*scale)`

## 4.2 Collapsible Panels

Each panel has: - Header area: 48 px (scaled) - Body area: expanded
height depends on panel

Implementation options: - Immediate collapse: body height = 0 - Animated
collapse: use `juce::AnimatedPosition` or a timer-driven lerp over \~150
ms

In v1, immediate collapse is acceptable and simpler.

------------------------------------------------------------------------

# 5. Parameter Binding Patterns

## 5.1 Use APVTS Attachments

For standard controls: - `SliderAttachment` - `ButtonAttachment` -
`ComboBoxAttachment`

For custom controls (XYPad, custom knob): - Implement a small binding
layer with: - `ParameterHandle` wrapper for get/set normalized values -
Use `beginChangeGesture/endChangeGesture` around drags - Call
`setValueNotifyingHost` with normalized value

## 5.2 Avoid Excessive Host Notifications

-   During drag, you may notify host at UI frame rate (\~60 Hz), not per
    mouse event.
-   Use a throttled timer or accumulate changes.

------------------------------------------------------------------------

# 6. Custom Control Specs

## 6.1 Knob Component (Knob.h/.cpp)

Recommended features: - Circular knob with value arc - Label below -
Double-click resets to default - Shift-drag for fine adjustment -
Right-click menu: - Learn MIDI - Clear mapping - Invert - Set range... -
Pickup mode...

Knob must expose: - Parameter ID it is bound to - Function to set
"mapped" indicator (small corner dot) - Function to set "learning
target" highlight

## 6.2 XYPad (XYPad.h/.cpp)

### Coordinate mapping

Given mouse position p in pad rect: - xNorm = clamp((p.x - rect.x) /
rect.w, 0..1) - yNorm = clamp(1 - (p.y - rect.y) / rect.h, 0..1) (top =
1)

Binding: - yNorm → `vowel` - xNorm → either `xy_pitch` (internal) or
mapped to `pitch_offset` parameter if you include one

### Gesture semantics

-   On mouseDown:
    -   begin gestures for affected params
-   On drag:
    -   update params (throttled optional)
-   On mouseUp:
    -   end gestures

### Markers

Show 5 vowel labels along Y axis: - OO, OH, AH, EH, EE This is a UI-only
overlay; internal vowel tables can still be continuous.

### MIDI Learn

-   Right-click on pad opens menu:
    -   Learn `vowel`
    -   Learn pitch X control (if enabled)
    -   Clear

------------------------------------------------------------------------

# 7. Metering & Animation Data (Processor → UI)

## 7.1 What UI Needs (Low Rate)

Provide a thread-safe "UI model" struct updated from audio thread at
block rate, read by UI timer at \~30--60 Hz.

`UiTelemetry` (example): - `float rmsL, rmsR` or mono RMS -
`float pitchHz` (or MIDI note float) - `float vowel01` -
`float vibAmount01` (optional) - `bool gateOpen` (note active)

## 7.2 Thread-Safe Transfer

Recommended approach: - Use atomics for each float (or a lock-free ring
buffer). - UI reads atomics on a `Timer` callback.

Avoid: - Calling into processor with locks. - Accessing audio buffers.

## 7.3 Repaint Policy

-   `MonkView` repaints at 30--60 Hz timer, not continuously.
-   `XYPad` repaints on input changes or at 30 Hz if you show smoothing
    trails.
-   Panels repaint only on interaction.

------------------------------------------------------------------------

# 8. MIDI Learn UI Wiring

## 8.1 Central MIDI Learn Controller

`MidiLearnController` (UI-side) responsibilities: - Track learn mode
on/off - Track "armed" parameter target (string ID) - Show overlay and
highlighting - Communicate target selection to processor's `MidiLearn`
system

## 8.2 Component Integration

Each mappable UI control implements: - `getParameterID()` -
`setIsLearningTarget(bool)` - `setIsMidiMapped(bool)`

When user chooses "Learn" from context menu: - UI sets armed target ID -
Calls processor method: `armMidiLearnTarget(paramID)` (message thread) -
Overlay shows "Move a MIDI control..."

On successful mapping: - Processor updates mapping snapshot and state -
UI polls `isMapped(paramID)` periodically or receives callback via
`ChangeBroadcaster` (message thread)

------------------------------------------------------------------------

# 9. Mapping Editor Panel (Optional v1, Recommended v1.1)

If included: - Simple table list: Source → Target → Transform -
Buttons: - Clear - Re-learn - Invert - Set min/max - Curve - Smoothing -
Pickup

This can be a separate modal component invoked from Header settings.

------------------------------------------------------------------------

# 10. Theme & Rendering

## 10.1 Theme.h

Centralize colors, fonts, radii: - Background color - Panel background -
Text primary/secondary - Accent highlight (also used for MIDI learn
highlight) - Corner radius constants (e.g., 12 px scaled) - Shadow style
(subtle, optional)

## 10.2 Drawing Rules

-   Use cached paths where possible
-   Avoid heavy gradients
-   Avoid per-pixel effects that trigger slow software rendering on some
    hosts

------------------------------------------------------------------------

# 11. Accessibility & UX Details

-   Ensure tab focus traversal for controls (optional but good).
-   Provide tooltips for advanced controls.
-   Make text readable at 0.75 scale.
-   Support double-click reset and command-click fine adjust (macOS
    conventions).

------------------------------------------------------------------------

# 12. Implementation Checklist (Codex-Friendly)

1.  Create `Theme` + `Layout` utilities
2.  Build `HeaderBar`
3.  Build `Knob` component + APVTS attachment support
4.  Build `XYPad` with direct parameter setting + gestures
5.  Build `MonkView` + telemetry timer
6.  Build `Panel` base (collapsible header + body)
7.  Implement Tone/Ensemble/Delay/Reverb panels
8.  Implement `MidiLearnOverlay` and per-control context menus
9.  Wire MIDI learn: `arm target` → `receive mapping` → UI highlight
10. Verify scaling and resizing behavior

------------------------------------------------------------------------

# 13. Acceptance Criteria

UI is considered correct when: - Window resizes cleanly; controls remain
aligned and readable - XYPad feels smooth; vowel morph is continuous
without stepping - All controls respond to host automation - MIDI learn
can be initiated from any control via context menu - Mapped controls
display a clear visual indicator - Monk animation responds to audio
telemetry without stutter - CPU overhead from UI is minimal and does not
spike during playback

------------------------------------------------------------------------

End of UI Component & Implementation Specification
