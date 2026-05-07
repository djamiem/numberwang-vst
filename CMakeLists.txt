cmake_minimum_required(VERSION 3.22)
project(Numberwang VERSION 1.9.27)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── ccache: cache compiled objects so unchanged modules skip recompile ─────────
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

add_subdirectory(JUCE)

if(APPLE)
    set(NUMBERWANG_FORMATS AU VST3 Standalone)
    set(NUMBERWANG_VST3_COPY_DIR "/Users/jamiemichalski/Desktop/DSKTP/C Drive/vst3")
elseif(WIN32)
    set(NUMBERWANG_FORMATS VST3 Standalone)
    set(NUMBERWANG_VST3_COPY_DIR "C:/Program Files/Common Files/VST3")
else()
    set(NUMBERWANG_FORMATS VST3 Standalone)
    set(NUMBERWANG_VST3_COPY_DIR "")
endif()

juce_add_plugin(Numberwang
    COMPANY_NAME "Jamie"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR "${NUMBERWANG_VST3_COPY_DIR}"
    PLUGIN_MANUFACTURER_CODE Jmcl
    PLUGIN_CODE Swid
    FORMATS ${NUMBERWANG_FORMATS}
    PRODUCT_NAME "Numberwang Pro"
    NEEDS_WEBVIEW2 TRUE
)

juce_add_binary_data(NumberwangAssets
    SOURCES
        Assets/BebasNeue-Regular.ttf
        Source/UI/index.html
        Source/UI/style.css
        Source/UI/app.js
        Source/UI/juce_frontend.js
        Source/UI/check_native_interop.js
)

juce_generate_juce_header(Numberwang)

target_sources(Numberwang PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/SynthVoice.h
)

# ── Dev UI: serve HTML/CSS/JS from disk so changes don't need a rebuild ───────
# Enable with:  cmake -B build -DNUMBERWANG_DEV_UI=ON   (or run ./dev.sh)
# Disable with: cmake -B build -DNUMBERWANG_DEV_UI=OFF  (or run ./build.sh)
option(NUMBERWANG_DEV_UI "Serve UI files from Source/UI/ on disk (standalone hot-reload)" OFF)

target_compile_definitions(Numberwang PUBLIC
    JUCE_WEB_BROWSER=1
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

if(WIN32)
    target_compile_definitions(Numberwang PUBLIC JUCE_USE_WIN_WEBVIEW2=1)
endif()

if(NUMBERWANG_DEV_UI)
    target_compile_definitions(Numberwang PRIVATE NUMBERWANG_DEV_UI=1)
    message(STATUS "Dev UI mode ON — UI files served from Source/UI/ on disk")
endif()

target_link_libraries(Numberwang
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
        NumberwangAssets
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
