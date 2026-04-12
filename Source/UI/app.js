/*
  ============================================================
  NUMBERWANG SYNTH — Parameter Wiring (app.js)
  ============================================================

  DEVELOPER GUIDE
  ---------------
  This file connects HTML controls to JUCE plugin parameters.
  Each section maps a JUCE parameter name to an HTML element.

  Three helper functions do the heavy lifting:
    bindSlider(paramName)   — wires a <input type="range">
    bindToggle(paramName)   — wires a <button class="toggle">
    bindCombo(paramName)    — wires a .radio-group

  Parameter names must match exactly what's declared as
  WebSliderRelay / WebToggleButtonRelay / WebComboBoxRelay
  in PluginEditor.h on the C++ side.
  ============================================================
*/

import {
  getSliderState,
  getToggleState,
  getComboBoxState,
  getNativeFunction,
} from "./juce_frontend.js";

// ── Native functions (C++ callbacks) ──────────────────────────────────────────
const panicFn       = getNativeFunction("panic");
const shiftOctaveFn = getNativeFunction("shiftOctave");
const noteOnFn      = getNativeFunction("noteOn");
const noteOffFn     = getNativeFunction("noteOff");

// ── Current octave display ────────────────────────────────────────────────────
let currentOctave = 4;
function updateOctaveDisplay(oct) {
  currentOctave = oct;
  document.getElementById("oct-display").textContent = oct;
}

document.getElementById("oct-down").addEventListener("click", () =>
  shiftOctaveFn(-1).then(updateOctaveDisplay)
);
document.getElementById("oct-up").addEventListener("click", () =>
  shiftOctaveFn(1).then(updateOctaveDisplay)
);

document.getElementById("panic-btn").addEventListener("click", () =>
  panicFn()
);

// ── HELPERS ───────────────────────────────────────────────────────────────────

/**
 * Connects a JUCE slider parameter to an <input type="range"> element
 * and its adjacent value display <span>.
 *
 * The input element must have id="<paramName>".
 * The value span must have id="<paramName>_val".
 *
 * @param {string} paramName - must match the WebSliderRelay name in C++
 */
function bindSlider(paramName) {
  const state  = getSliderState(paramName);
  const input  = document.getElementById(paramName);
  const valEl  = document.getElementById(paramName + "_val");

  if (!input) return;

  // Format the displayed value
  function formatVal() {
    const v = state.getScaledValue();
    const label = state.properties.label || "";
    if (Number.isInteger(v) || Math.abs(v) >= 100) {
      return Math.round(v) + (label ? " " + label : "");
    }
    return v.toFixed(2) + (label ? " " + label : "");
  }

  // Push C++ → HTML
  function refresh() {
    input.value = state.getNormalisedValue();
    if (valEl) valEl.textContent = formatVal();
    // Update the CSS gradient fill on the track
    input.style.setProperty("--thumb-pct", (state.getNormalisedValue() * 100).toFixed(1));
  }

  // Pull HTML → C++
  input.addEventListener("pointerdown", () => state.sliderDragStarted());
  input.addEventListener("pointerup",   () => state.sliderDragEnded());
  input.addEventListener("input",       () => {
    state.setNormalisedValue(parseFloat(input.value));
    if (valEl) valEl.textContent = formatVal();
    input.style.setProperty("--thumb-pct", (parseFloat(input.value) * 100).toFixed(1));
  });

  // Listen for changes pushed from C++ (e.g. automation, undo)
  state.valueChangedEvent.addListener(refresh);

  refresh();
}

/**
 * Connects a JUCE bool parameter to a <button class="toggle"> element.
 * The button must have id="<paramName>".
 *
 * @param {string} paramName - must match the WebToggleButtonRelay name in C++
 */
function bindToggle(paramName) {
  const state = getToggleState(paramName);
  const btn   = document.getElementById(paramName);

  if (!btn) return;

  function refresh() {
    btn.classList.toggle("active", state.getValue());
  }

  btn.addEventListener("click", () => {
    state.setValue(!state.getValue());
    refresh();
  });

  state.valueChangedEvent.addListener(refresh);
  refresh();
}

/**
 * Connects a JUCE choice parameter to a group of <button class="radio"> elements.
 * The container must have id="<paramName>" and contain buttons with data-index attributes.
 *
 * @param {string} paramName - must match the WebComboBoxRelay name in C++
 */
function bindCombo(paramName) {
  const state   = getComboBoxState(paramName);
  const container = document.getElementById(paramName);

  if (!container) return;

  const buttons = container.querySelectorAll(".radio");

  function refresh() {
    const idx = state.getChoiceIndex();
    buttons.forEach((btn, i) => btn.classList.toggle("active", i === idx));
  }

  buttons.forEach((btn, i) => {
    btn.addEventListener("click", () => {
      state.setChoiceIndex(i);
      refresh();
    });
  });

  state.valueChangedEvent.addListener(refresh);
  refresh();
}

// ── ROTARY KNOBS ──────────────────────────────────────────────────────────────
//
// SVG arc knob: 270° sweep from 225° (7 o'clock, value=0) to 135° (5 o'clock, value=1).
// Each knob keeps a hidden <input type="range"> for JUCE relay compatibility.
// The input.value property is overridden so C++ → display updates go through the knob.

const KNOB_CX    = 40;
const KNOB_CY    = 40;
const KNOB_R     = 28;  // arc radius
const KNOB_IR    = 18;  // indicator line radius
const KNOB_START = 225; // degrees, value = 0
const KNOB_SWEEP = 270; // total degrees of travel

function _knobPt(deg) {
  const r = deg * Math.PI / 180;
  return [KNOB_CX + KNOB_R * Math.cos(r), KNOB_CY + KNOB_R * Math.sin(r)];
}

function _updateKnobDisplay(id, norm) {
  const fillEl = document.getElementById(id + '_fill');
  const ptrEl  = document.getElementById(id + '_ptr');
  const angleDeg = KNOB_START + norm * KNOB_SWEEP;

  if (fillEl) {
    if (norm < 0.002) {
      fillEl.setAttribute('d', '');
    } else {
      const [sx, sy] = _knobPt(KNOB_START);
      const [ex, ey] = _knobPt(angleDeg);
      const largeArc = (norm * KNOB_SWEEP) > 180 ? 1 : 0;
      fillEl.setAttribute('d',
        `M ${sx.toFixed(2)} ${sy.toFixed(2)} A ${KNOB_R} ${KNOB_R} 0 ${largeArc} 1 ${ex.toFixed(2)} ${ey.toFixed(2)}`
      );
    }
  }

  if (ptrEl) {
    const r = angleDeg * Math.PI / 180;
    ptrEl.setAttribute('x2', (KNOB_CX + KNOB_IR * Math.cos(r)).toFixed(2));
    ptrEl.setAttribute('y2', (KNOB_CY + KNOB_IR * Math.sin(r)).toFixed(2));
  }
}

/**
 * Sets up a rotary SVG knob for a given parameter.
 * MUST be called BEFORE bindSlider() for the same parameter so the
 * value property override is in place when bindSlider calls refresh().
 *
 * @param {string} id - matches the <input id="..."> and <svg id="..._knob">
 */
function setupKnob(id) {
  const input = document.getElementById(id);
  const svg   = document.getElementById(id + '_knob');
  if (!input || !svg) return;

  // Override input.value so C++ → JS updates drive the knob display
  let _val = '0';
  Object.defineProperty(input, 'value', {
    get: () => _val,
    set: (v) => { _val = String(v); _updateKnobDisplay(id, parseFloat(v) || 0); },
    configurable: true,
    enumerable: true,
  });

  // Drag: vertical movement (up = increase)
  let dragActive  = false;
  let dragStartY  = 0;
  let dragStartVal = 0;

  svg.addEventListener('pointerdown', (e) => {
    dragActive   = true;
    dragStartY   = e.clientY;
    dragStartVal = parseFloat(_val) || 0;
    svg.setPointerCapture(e.pointerId);
    input.dispatchEvent(new Event('pointerdown', { bubbles: false }));
  });

  svg.addEventListener('pointermove', (e) => {
    if (!dragActive) return;
    const dy = dragStartY - e.clientY;          // up = positive delta
    const newVal = Math.max(0, Math.min(1, dragStartVal + dy / 160));
    _val = String(newVal);
    _updateKnobDisplay(id, newVal);
    input.dispatchEvent(new Event('input',     { bubbles: false }));
  });

  svg.addEventListener('pointerup', () => {
    if (dragActive) {
      dragActive = false;
      input.dispatchEvent(new Event('pointerup', { bubbles: false }));
    }
  });

  svg.addEventListener('pointercancel', () => { dragActive = false; });
}

// ── WIRE ALL PARAMETERS ───────────────────────────────────────────────────────
//
// Each line below connects one parameter.
// To add a new parameter:
//   1. Add a WebSliderRelay / WebToggleButtonRelay / WebComboBoxRelay in PluginEditor.h
//   2. Add the corresponding HTML element in index.html
//   3. Add a bindSlider / bindToggle / bindCombo call below

// Oscillator 1
bindCombo ("osc1_wave");
bindSlider("osc1_level");
bindSlider("osc1_detune");
bindSlider("osc1_octave");

// Oscillator 2
bindCombo ("osc2_wave");
bindSlider("osc2_level");
bindSlider("osc2_octave");

// Arpeggiator
bindToggle("arp_enabled");
bindToggle("arp_hold");
bindSlider("arp_bpm");
bindCombo ("arp_division");

// Noise & Filter
bindSlider("noise_level");
setupKnob("filter_cutoff"); bindSlider("filter_cutoff");
setupKnob("filter_res");    bindSlider("filter_res");

// Filter Mod (Envelope + LFO)
bindSlider("filter_env_attack");
bindSlider("filter_env_amount");
bindToggle("filter_lfo_sync");
bindCombo ("filter_lfo_div");
bindSlider("filter_lfo_rate");
bindSlider("filter_lfo_amount");

// ADSR Envelope — setupKnob must precede bindSlider so the value override is live
setupKnob("attack");   bindSlider("attack");
setupKnob("decay");    bindSlider("decay");
setupKnob("sustain");  bindSlider("sustain");
setupKnob("release");  bindSlider("release");

// Master Volume
setupKnob("master_volume"); bindSlider("master_volume");

// Numberwang toggle
bindToggle("numberwang");

// Super Saw
bindToggle("ssaw_enabled");
setupKnob("ssaw_level");   bindSlider("ssaw_level");
setupKnob("ssaw_octave");  bindSlider("ssaw_octave");
setupKnob("ssaw_detune");  bindSlider("ssaw_detune");
setupKnob("ssaw_spread");  bindSlider("ssaw_spread");

// Stereo Width
bindToggle("width_enabled");
bindSlider("width_amount");

// Chorus
bindToggle("chorus_enabled");
bindSlider("chorus_rate");
bindSlider("chorus_depth");
bindSlider("chorus_mix");

// Delay
bindToggle("delay_enabled");
bindSlider("delay_time");
bindSlider("delay_feedback");
bindSlider("delay_mix");

// ── KEYBOARD INPUT ────────────────────────────────────────────────────────────
//
// QWERTY → MIDI piano mapping (same layout as JUCE MidiKeyboardComponent):
//   a w s e d f t g y h u j k
//   C C#D D#E F F#G G#A A#B C(+1)
//
// z = octave down, x = octave up, space = toggle arp hold

const QWERTY_NOTE = { a:0, w:1, s:2, e:3, d:4, f:5, t:6, g:7, y:8, h:9, u:10, j:11, k:12 };
const heldKeys = new Set();

document.addEventListener("keydown", (ev) => {
  const key = ev.key.toLowerCase();
  const handled = key === "z" || key === "x" || key === " " || key in QWERTY_NOTE;

  // Always suppress OS beep for handled keys, even on key-repeat events
  if (handled) ev.preventDefault();
  if (ev.repeat) return;

  if (key === "z") { shiftOctaveFn(-1).then(updateOctaveDisplay); return; }
  if (key === "x") { shiftOctaveFn(1).then(updateOctaveDisplay); return; }

  if (key === " ") {
    const state = getToggleState("arp_hold");
    state.setValue(!state.getValue());
    const btn = document.getElementById("arp_hold");
    if (btn) btn.classList.toggle("active", state.getValue());
    return;
  }

  if (key in QWERTY_NOTE && !heldKeys.has(key)) {
    heldKeys.add(key);
    const note = (currentOctave + 1) * 12 + QWERTY_NOTE[key];
    noteOnFn(note, 0.8);
  }
});

document.addEventListener("keyup", (ev) => {
  const key = ev.key.toLowerCase();
  if (key in QWERTY_NOTE && heldKeys.has(key)) {
    heldKeys.delete(key);
    const note = (currentOctave + 1) * 12 + QWERTY_NOTE[key];
    noteOffFn(note);
  }
});

// ── NUMBERWANG EVENT (C++ → JS) ───────────────────────────────────────────────
//
// The C++ timer emits a "numberwang_display" event every 30Hz with
// { roll: int, note: int, fired: bool }
//
window.__JUCE__.backend.addEventListener("numberwang_display", (data) => {
  document.getElementById("roll").textContent  = data.roll  > 0 ? data.roll  : "-";
  document.getElementById("note").textContent  = data.note  > 0 ? data.note  : "-";

  if (data.fired) {
    const flash = document.getElementById("nw-flash");
    flash.classList.remove("hidden");
    // Remove and re-add class to restart the animation
    flash.style.animation = "none";
    flash.offsetHeight;  // force reflow
    flash.style.animation = "";
    flash.classList.remove("hidden");
    setTimeout(() => flash.classList.add("hidden"), 520);
  }
});
