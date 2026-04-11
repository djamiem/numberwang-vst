# Numberwang VST — Developer Handoff

Hey Andy — this doc covers how the UI works, where to find important things, and how to add new assets or parameters.

---

## Architecture overview

The plugin UI is a **WebView embedded inside a JUCE plugin window**. There's no traditional JUCE GUI toolkit involved — the entire visual interface is HTML/CSS/JS, served from compiled-in binary data. JUCE sits underneath and handles:

- The audio engine (`PluginProcessor`)
- The WebView host and parameter relay system (`PluginEditor`)
- A native MIDI keyboard rendered below the WebView

```
┌──────────────────────────────────────┐
│         WebView (960 × 792)          │  ← HTML/CSS/JS lives here
│   index.html / style.css / app.js   │
├──────────────────────────────────────┤
│     JUCE MidiKeyboardComponent       │  ← native, 48px tall
└──────────────────────────────────────┘
```

---

## Key files

| File | What it does |
|------|--------------|
| `Source/PluginProcessor.h/.cpp` | Audio engine, parameter layout (APVTS), synth voices, FX |
| `Source/PluginEditor.h/.cpp` | WebView setup, relay declarations, parameter attachments |
| `Source/SynthVoice.h` | Individual synth voice (oscillators, filter, envelope) |
| `Source/UI/index.html` | UI markup — knobs, toggles, sections |
| `Source/UI/style.css` | All visual styling |
| `Source/UI/app.js` | Wires HTML controls to JUCE parameters |
| `Source/UI/juce_frontend.js` | JUCE-provided JS bridge (don't edit) |
| `Source/UI/check_native_interop.js` | Dev helper for checking the bridge is live |
| `Assets/BebasNeue-Regular.ttf` | Font bundled as binary data |
| `CMakeLists.txt` | Build config — also where you register new assets |

---

## How parameters flow: the relay system

Every knob/toggle/dropdown goes through three layers:

```
C++ parameter (APVTS)
      ↕  WebSliderParameterAttachment
C++ relay  (WebSliderRelay "param_name")
      ↕  juce_frontend.js bridge
JS  (getSliderState("param_name"))
      ↕  app.js bindSlider()
HTML  <input type="range" id="param_name">
```

### Adding a new slider parameter — end to end

**1. Declare the parameter** in `PluginProcessor.cpp` inside `createParameterLayout()`:
```cpp
params.push_back(std::make_unique<juce::AudioParameterFloat>(
    "my_param", "My Param",
    juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
```

**2. Declare the relay** in `PluginEditor.h`:
```cpp
juce::WebSliderRelay myParamRelay { "my_param" };
```

**3. Add the attachment** in `PluginEditor.h`:
```cpp
juce::WebSliderParameterAttachment myParamAtt;
```

**4. Register the relay** in `PluginEditor.h` — add it to the `buildOptions` signature, then in `PluginEditor.cpp` add `.withOptionsFrom(myParamRelay)` to the options chain in `buildOptions()`.

**5. Wire the attachment** in the `PluginEditor` constructor initialiser list:
```cpp
myParamAtt (*p.apvts.getParameter("my_param"), myParamRelay),
```

**6. Add the HTML control** in `index.html`:
```html
<input type="range" id="my_param" min="0" max="1" step="0.01">
<span id="my_param_val"></span>
```

**7. Bind it in `app.js`**:
```js
bindSlider("my_param");
```

For toggles use `WebToggleButtonRelay` / `WebToggleButtonParameterAttachment` / `bindToggle()`.
For dropdowns use `WebComboBoxRelay` / `WebComboBoxParameterAttachment` / `bindCombo()`.

---

## Native functions (C++ ↔ JS calls)

Two native functions are registered in `PluginEditor.cpp` inside `buildOptions()`:

| Function | Direction | What it does |
|----------|-----------|--------------|
| `panic` | JS → C++ | Stops all voices immediately |
| `shiftOctave(dir)` | JS → C++ | Shifts the on-screen keyboard octave up/down, returns new octave |

To add a new one, add a `.withNativeFunction("name", ...)` call in `buildOptions()`, then call it from JS with `getNativeFunction("name")`.

To push data **from C++ to JS** (like the Numberwang display does), use:
```cpp
webComponent.emitEventIfBrowserIsVisible("event_name", juce::var(...));
```
Then in JS listen with `window.addEventListener("event_name", handler)`.

---

## Adding a font or other asset

There are **three places** to touch:

### 1. Drop the file in
Put fonts in `Assets/`, or UI files (images, extra JS, etc.) in `Source/UI/`.

### 2. Register it in `CMakeLists.txt`
Add it to the `juce_add_binary_data` block:
```cmake
juce_add_binary_data(StereoWidenerAssets
    SOURCES
        Assets/BebasNeue-Regular.ttf
        Assets/MyNewFont.ttf          # ← add here
        Source/UI/index.html
        Source/UI/style.css
        ...
)
```

### 3. Expose it via the resource provider in `PluginEditor.cpp`
Add an entry to the `uiFiles[]` array near the top of the file:
```cpp
static const UIFile uiFiles[] =
{
    { "index.html",           BinaryData::index_html,           BinaryData::index_htmlSize           },
    ...
    { "MyNewFont.ttf",        BinaryData::MyNewFont_ttf,         BinaryData::MyNewFont_ttfSize         },
};
```

The `BinaryData::` symbol name is derived from the filename: spaces and hyphens become underscores, dots before the extension become underscores. When in doubt, check `build/juce_binarydata_StereoWidenerAssets/JuceLibraryCode/BinaryData.h` after building to see the generated names.

### 4. Use it in CSS
```css
@font-face {
    font-family: "MyNewFont";
    src: url("MyNewFont.ttf") format("truetype");
}
```

The URL path must match the key you used in `uiFiles[]`.

---

## Building

```bash
# First time / after changing CMakeLists.txt
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Subsequent builds
cmake --build build --config Release

# Or just the standalone for quick testing
cmake --build build --target StereoWidener_Standalone --config Release
```

The built VST3 is automatically copied to the path set by `VST3_COPY_DIR` in `CMakeLists.txt`.

---

## Things worth knowing

- **Version bumping**: before every build, bump the version in `CMakeLists.txt` (`project(StereoWidener VERSION x.y.z)`) and append a line to `NOTES.md`. Patch for small changes, minor for new features.
- **The WebView only navigates to the resource provider root** — `NWBrowser::pageAboutToLoad` blocks any external URLs. This is intentional.
- **The MIDI keyboard is native JUCE**, not part of the WebView. It gets mouse focus when the cursor enters its area so keyboard shortcuts work.
- **Numberwang**: there's a "chaos mode" in `PluginProcessor` that randomly fires when Numberwang is toggled on, injecting a fifth and temporarily overriding FX settings. It's on a 5-second cooldown. The JS receives `numberwang_display` events via the timer in `PluginEditor`.
