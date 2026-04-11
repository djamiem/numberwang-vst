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
const panicFn      = getNativeFunction("panic");
const shiftOctaveFn = getNativeFunction("shiftOctave");

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
bindSlider("filter_cutoff");
bindSlider("filter_res");

// Filter Mod (Envelope + LFO)
bindSlider("filter_env_attack");
bindSlider("filter_env_amount");
bindToggle("filter_lfo_sync");
bindCombo ("filter_lfo_div");
bindSlider("filter_lfo_rate");
bindSlider("filter_lfo_amount");

// ADSR Envelope
bindSlider("attack");
bindSlider("decay");
bindSlider("sustain");
bindSlider("release");

// Master Volume
bindSlider("master_volume");

// Numberwang toggle
bindToggle("numberwang");

// Super Saw
bindToggle("ssaw_enabled");
bindSlider("ssaw_octave");
bindSlider("ssaw_level");
bindSlider("ssaw_detune");
bindSlider("ssaw_spread");

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
