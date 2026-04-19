# Numberwang Pro VST — Dev Notes

## 2026-04-19 — Add AU format for Logic Pro compatibility (v1.9.25)

Added `AU` to FORMATS in CMakeLists.txt so the plugin builds as an Audio Unit alongside VST3; Logic Pro on Mac only loads AU plugins.

## 2026-04-12 — Super Saw level → inline knob next to ON button (v1.9.22)

Moved ssaw_level from a separate slider row into an xs rotary knob beside the ON toggle; saves one row height so SPREAD is no longer clipped.

## 2026-04-12 — Stack OSC 1/2, window 800→840px to fix super saw clip (v1.9.20)

OSC 1 and OSC 2 changed from side-by-side 2-column grid to vertically stacked with a gold divider; window height increased 800→840px so super saw section (and its SPREAD row) is fully visible.

## 2026-04-12 — Font upgrade, scoreboard height, accessibility pass (v1.9.18)

Switched UI font from BebasNeue to Avenir Next (sophisticated, readable geometric sans on macOS); BebasNeue kept only for NUMBERWANG! banner title. Scoreboard height raised to 108px so vol knob and OCT controls are fully visible. Bumped font sizes throughout (section titles 13px, labels/values 12px), reduced excessive letter-spacing, raised all text opacities to 0.88–0.92, increased slider track/thumb and toggle sizes for better accessibility.

## 2026-04-12 — Scoreboard right fix, ADSR horizontal row, Super Saw to col 2 (v1.9.16)

Added missing CSS for score-right/oct-btn-sm/knob-svg--xs so vol knob and octave controls render correctly; bumped scoreboard height to 84px; ADSR knobs changed from 2×2 grid to single 4-knob horizontal row; Super Saw moved from column 3 to bottom of column 2.

## 2026-04-12 — Fix duplicate oct IDs + missing master_volume knob setup (v1.9.14)

Removed redundant "KEYBOARD OCTAVE" section from column 2 (oct controls moved to scoreboard); added setupKnob("master_volume") so the VOL knob SVG is interactive.

## 2026-04-12 — Fix OS beep on key hold (v1.9.7)

Call preventDefault() before the ev.repeat early-return so held keys don't trigger macOS system beep.

## 2026-04-12 — ADSR row layout + keyboard beep fix (v1.9.6)

Converted ENVELOPE from broken column-flex knob layout to standard rows (ATK/DECAY/SUSTAIN/REL); added e.preventDefault() to all handled keydown events to stop OS beep.

## 2026-04-12 — QWERTY keyboard + ADSR fix (v1.9.5)

Fixed ADSR sliders invisible in column-flex context (flex:none override on .adsr-knob input); ported QWERTY→MIDI, z/x octave shift, and spacebar arp-hold from the old C++ editor to JS/native functions (noteOn, noteOff).

## 2026-04-12 — 3-column layout + rename to Numberwang Pro (v1.9.4)

Switched UI from 2 columns to 3 (Sources / Modulation / FX) at 1200×800 to fix viewport clipping; renamed product from "Stereo Widener" to "Numberwang Pro" throughout.

## 2026-04-12 — Desktop symlink update (v1.9.3)

Rebuilt after renaming project folder to `numberwang-vst`; updated Desktop symlink to "Numberwang Pro".

## 2026-04-11 — HTML/WebView UI (v1.9.2)

Replaced the native JUCE C++ editor with a WebBrowserComponent-based HTML/CSS/JS UI; old editor archived in `Source/archive/` for easy rollback.

## 2026-04-11 — Filter envelope + LFO (v1.9.1)

### What changed
- **Filter envelope** (per-voice): ENV ATK knob (0.001–2 s) + ENV AMT (0–1) open the filter cutoff from closed toward its base cutoff over the attack time on each note-on; `filterEnvPhase` resets to 0 on `startNote`
- **Filter LFO** (global, processBlock-rate): LFO RATE knob (0.1–20 Hz) + LFO AMT (0–1) apply a ±8 kHz sine modulation on top of base cutoff and envelope contribution
- **Tempo sync**: SYNC button locks LFO rate to `arp_bpm`; 5 division buttons (1/16, 1/8, 1/4, 1/2, 1 BAR) select subdivision — stored as `filter_lfo_div` AudioParameterChoice
- **New params**: `filter_env_attack`, `filter_env_amount`, `filter_lfo_rate`, `filter_lfo_amount`, `filter_lfo_sync`, `filter_lfo_div`
- **Architecture**: `setFilterParams` now stores `storedBaseCutoff`/`storedQ` instead of calling `setLowPass`; coefficients applied once per block in `renderNextBlock` with env + LFO baked in; both suppressed during Numberwang chaos
- **UI**: window 780 → 888 px; new FILTER MOD section between filter knobs and ADSR

---

## 2026-04-11 — Octave knobs, spacebar hold, z/x keyboard octave shift (v1.9.0)

### What changed
- **Octave knobs**: OSC1, OSC2, and Super Saw each get an OCT slider (-3 to +3 semitone-octaves); applied as `phaseDelta * 2^octave` in the voice so it shifts pitch cleanly
- **z/x keyboard shortcuts**: `z` = octave down, `x` = octave up (was `a`/`x`)
- **Spacebar = hold toggle**: spacebar now toggles `arp_hold` on/off
- **Numberwang chaos**: all three octaves randomize (-2 to +2) on chaos trigger, restore automatically when chaos ends (parameters unchanged)
- **New params**: `osc1_octave`, `osc2_octave`, `ssaw_octave` (AudioParameterInt, -3 to +3, default 0)

---

## 2026-04-11 — Numberwang animation: gradient + spinning text (v1.8.6)

### What changed
- **Timer bumped** 10 Hz → 30 Hz for smooth animation
- **Duration**: 16 flash frames at 30 Hz = ~500 ms total
- **No blocking overlay**: gradient opacity max 35% so UI controls remain visible
- **Animating gradient**: diagonal 3-stop rainbow (hues shift with `t`) sweeps across the screen; fades in/holds/fades out
- **Spinning text**: ease-out cubic; starts at scale 0.05 + full rotation (2π), grows to 1.5× + unspins over first 65% of animation; then fades and slightly shrinks over the remaining 35%

---

## 2026-04-11 — Super saw level balance + proportional cross-fade (v1.8.5)

### What changed
- **Super saw loudness**: normalization changed from `ssawLevel / NUM_SAW` (≈0.1 at default) to `ssawLevel * 0.5` — makes super saw roughly equal RMS to mono oscs at the same level setting
- **Proportional cross-fade**: mono osc mix is multiplied by `(1 - ssawLevel)` when super saw is enabled — at 50% super saw, mono oscs are at 50%; at 100% super saw, mono oscs are silent; at 0%, mono oscs are unaffected
- Noise is not cross-faded (independent of the mix)

---

## 2026-04-11 — Bug fixes: OSC level faders, stuck notes, chaos choppiness (v1.8.4)

### What changed
- **OSC level faders restored**: moved to row 2 (full-width fader below wave buttons) — previous layout left only ~22px which was too small to use
- **Stuck notes**: `focusLost` override calls `keyboardComp.resetAnyKeysInUse()` so QWERTY notes don't stick when window loses focus
- **Chaos end choppiness**: detect chaos→normal transition; reset all voice filter states (avoids IIR pop from coefficient snap) and zero delay+chorus buffers (avoids stale chaos echoes)

---

## 2026-04-11 — Numberwang chaos FX randomization (v1.8.3)

### What changed
- When Numberwang triggers, all FX (chorus, delay, stereo width) are force-enabled with randomized values for 2 beats (half a measure), then revert to user settings — no APVTS params are mutated
- Super saw also randomly enabled (~60% chance) during chaos with randomized detune/spread
- Filter sweeps to a random cutoff (200–3000 Hz) with high resonance during chaos
- Chaos state is a plain struct (`NWChaosState`) in the processor; audio-thread-only, no atomics needed
- Duration tied to `arp_bpm` parameter; defaults reasonable even if arp is off

---

## 2026-04-11 — Panic button, vol knob fix, filter/res as rotary knobs (v1.8.2)

### What changed
- **PANIC button**: red button in scoreboard (left column, beside Numberwang toggle); sets `panicRequested` atomic which audio thread reads to call `allNotesOff`, clear held notes, zero delay buffers, and return immediately
- **Master volume knob**: repositioned to fit within scoreboard area (right column); TextBoxAbove prevents overflow below y=174; knob sized 68×70px
- **Filter & resonance**: converted from horizontal sliders to side-by-side rotary knobs; CUTOFF on left half, RES on right half; each 80px tall (label 14px + rotary 66px)
- **UI height**: 720 → 780px to accommodate taller filter section; ADSR top shifted from y=478 → y=530; piano from y=672 → y=732

---

## Plugin overview
- JUCE VST3 stereo widener with mid/side processing
- Has a "Numberwang" feature: FFT detects dominant frequency, maps it to a number, matches against a random 1–10 roll
- Build: `cd build && cmake --build . --config Release`
- Installs to: `/Users/jamiemichalski/Desktop/DSKTP/C Drive/vst3/Stereo Widener.vst3`

---

## 2026-04-11 — Master volume knob (v1.8.1)

### What changed
- Master volume rotary knob (0–2.0 = 0–200%) in the scoreboard right area
- Applied after all FX via `buffer.applyGain()` — clean single gain stage
- Default 1.0 (unity), goes to 2× for boosting quiet patches

---

## 2026-04-11 — Super saw, stereo widener, chorus, delay + wider UI (v1.8.0)

### What changed
- **Super Saw** (right column, ON button + 3 sliders):
  - 7 detuned sawtooth voices per synth voice with stereo pan spread
  - Pan positions: centre + 3 symmetric pairs at 100%/60%/25% of spread
  - Equal-power panning gives genuine stereo output per voice
  - DETUNE: 0–1 scales max detune ±50 cents (similar to JP-8000 super saw)
  - SPREAD: 0–1 sets stereo pan width of the 7 saws
  - Both filter (cutoff/res) and ADSR envelope apply to the super saw signal
  - Saw phases staggered on note start to avoid click transient
- **Stereo Widener** (M/S post-FX, ON + WIDTH slider 0–2):
  - `width=1.0` = unchanged; `<1` = narrows toward mono; `>1` = extra width
  - Works best with super saw active (stereo content needed)
- **Chorus** (post-FX, ON + RATE/DEPTH/MIX):
  - Dual LFO-modulated delay lines; L/R 180° phase offset for stereo movement
  - 7 ms centre delay ±10 ms depth, rate 0.1–5 Hz
- **Delay** (ping-pong post-FX, ON + TIME/FDBK/MIX):
  - L echoes appear on R and vice versa (ping-pong)
  - Time 0.05–1.0 s, feedback up to 0.95
  - Default 0.375 s (dotted-eighth at 120 BPM)
- **UI**: widened 760→960 px; left column (0–480) = existing synth controls; right column (488–960) = new FX sections
- Version 1.7.0 → **1.8.0** (minor — major new feature set)

---

## 2026-04-11 — Noise channel, filter + resonance, octave switching (v1.7.0)

### What changed
- **Noise channel**: white noise mixed alongside oscillators, shaped by ADSR, with `noise_level` fader (0–1, default 0)
- **State-variable LP filter** (`juce::dsp::StateVariableTPTFilter`) per voice:
  - `filter_cutoff`: 20 Hz – 20 kHz (skewed), default 20 kHz (fully open)
  - `filter_res`: 0–1 resonance (0 = overdamped, ~0.3 = Butterworth, 0.9+ = squelchy); hard-clamped to 0.95 to prevent self-oscillation
  - Filter resets on note start to avoid state bleed between notes
- **Octave switching**: press `a` to shift keyboard down one octave, `x` to shift up (range 0–8)
- UI: new NOISE & FILTER section between arp and ADSR; noise row (full-width fader) + filter row (cutoff left half, resonance right half)
- Version 1.6.3 → **1.7.0** (minor — two new features)

---

## 2026-04-11 — Fix: hold-off leaves note stuck (v1.6.3)

### What changed
- **Bug fix**: turning off HOLD left one arp note stuck playing indefinitely
- Root cause: note-off for `currentArpNote` was added to `midiMessages` before `midiMessages.swapWith(arpMidi)`, which silently discarded it
- Fix: capture `releaseNote` before the arp swap, inject note-off after `swapWith()` so it survives in the final MIDI buffer
- Same fix covers the "arp disabled while playing" path

---

## 2026-04-11 — Detune, arpeggiator + hold, UI resize (v1.6.0)

### What changed
- **Detune** (`osc1_detune`, -100 to +100 cents): applied per-voice as `phaseDelta * pow(2, cents/1200)`, live-updating
- **Arpeggiator**: sample-accurate up-down bounce pattern; lives entirely in `processBlock`, replaces the MIDI buffer when enabled
  - Parameters: `arp_enabled`, `arp_hold`, `arp_bpm` (40-240), `arp_division` (1/4, 1/8, 1/16, 1/32)
  - Hold: note-offs ignored while hold is on; turning hold off clears held notes and stops arp note
  - Up-down bounce: e.g. [C,E,G] → C→E→G→E→C→E→... (no repeat at top/bottom)
- **UI resized**: 760×720 (was 900 tall); OSC 1 and OSC 2 now side-by-side
- Arp section sits between osc panels and ADSR
- Version 1.5.1 → **1.6.0** (minor bump)

---

## 2026-04-11 — Dual oscillators with waveform select + level faders (v1.5.0)

### What changed
- `SynthVoice.h`: each voice now has 2 oscillators, each with sine/square/triangle and independent level
- Phase accumulator switched to 0-1 range (cleaner square/triangle math)
- Waveforms: sine = `sin(phase * 2π)`, square = `phase < 0.5 ? 1 : -1`, triangle = piecewise linear
- New parameters: `osc1_wave` (AudioParameterChoice), `osc1_level`, `osc2_wave`, `osc2_level`
- UI: osc sections between scoreboard and ADSR — each row has label + 3 toggle buttons (SINE/SQR/TRI) + horizontal level fader
- UI height expanded 720 → 900px
- Button styling: gold when active, dark blue when inactive, Bebas Neue font
- Version 1.4.0 → **1.5.0** (minor bump)

---

## 2026-04-11 — Synth conversion + Standalone build (v1.4.0)

### What changed
- Converted from stereo widener effect → **polyphonic sine synth** (8 voices)
- Added **Standalone** to FORMATS — standalone `.app` at `build/StereoWidener_artefacts/Debug/Standalone/`
- Added `SynthVoice.h` — `NWVoice` (sine + ADSR) and `NWSound`
- Processor: removed FFT/widener/LP filter; now has `juce::Synthesiser` + `MidiKeyboardState`
- Parameters changed: `width` → `attack`, `decay`, `sustain`, `release`
- Editor: replaced width knob with 4 ADSR rotary knobs; added `MidiKeyboardComponent` (C2–C7)
- Computer keyboard input: QWERTY mapped to MIDI via `keyboardComp` forwarded from editor's `keyPressed`/`keyStateChanged`
- Numberwang game: fires when `midiNote % 10` matches random roll 1–10
- Version 1.3.0 → **1.4.0** (minor bump)

---

## 2026-04-11 — Bebas Neue font + double UI size (v1.3.0)

### What changed
- Downloaded Bebas Neue Regular (OFL license) into `Assets/BebasNeue-Regular.ttf`
- Embedded via `juce_add_binary_data` — font is baked into the VST binary
- All text in the UI now uses Bebas Neue
- UI doubled from 280×360 → **560×720**
- Version 1.2.0 → **1.3.0** (minor bump — new font + layout)

---

## 2026-04-11 — LP filter sweep on every quarter beat

### What changed
- LP filter sweep now fires on **every quarter note** (previously only when Numberwang matched)
- Sweep duration extended to **full quarter note** (was 1/16th note — quarter of a beat)
- Cutoff floor lowered from 100 Hz → **50 Hz** for more dramatic low-end muffling
- Added **high resonance** (`Q ≈ 12`, `resonance = 0.08`) — gives a noticeable whistling/squelch at the cutoff as it sweeps down
- Sweep shape: exponential (log-scale), 20 kHz → 50 Hz over one beat

### Version
1.1.0 → **1.2.0** (minor bump — new always-on dramatic filter sweep behaviour)

### Numberwang
- Still detected and flagged each beat (for UI display), but no longer gates the filter

## 2026-04-12 — ADSR + filter rotary knobs, viewport fill, remove yellow envelope (v1.9.9)

ADSR + filter rotary knobs, viewport fill, remove yellow envelope

## 2026-04-12 — Rename StereoWidener → Numberwang throughout C++ and cmake (v1.9.10)

Rename StereoWidener → Numberwang throughout C++ and cmake

## 2026-04-12 — Accessibility: larger knobs/text, raise dim text opacity, danger color to orange for color-blind users (v1.9.11)

Accessibility: larger knobs/text, raise dim text opacity, danger color to orange for color-blind users

## 2026-04-12 — Add ccache + parallel jobs; prime cache for fast incremental builds (v1.9.12)

Add ccache + parallel jobs; prime cache for fast incremental builds

## 2026-04-12 — Verify ccache hit rate on second build (v1.9.13)

Verify ccache hit rate on second build

## 2026-04-12 — Auto build bump (v1.9.15)

Auto build bump

## 2026-04-12 — Auto build bump (v1.9.17)

Auto build bump

## 2026-04-12 — Auto build bump (v1.9.19)

Auto build bump

## 2026-04-12 — Auto build bump (v1.9.21)

Auto build bump

## 2026-04-12 — Auto build bump (v1.9.23)

Auto build bump

## 2026-04-12 — WebView UI overhaul: rotary knobs, Avenir Next font, 3-column layout, stacked OSCs, super saw knob panel, dev hot-reload mode (v1.9.24)

WebView UI overhaul: rotary knobs, Avenir Next font, 3-column layout, stacked OSCs, super saw knob panel, dev hot-reload mode

## 2026-04-19 — Auto build bump (v1.9.26)

Auto build bump

## 2026-04-19 — Auto build bump (v1.9.27)

Auto build bump
