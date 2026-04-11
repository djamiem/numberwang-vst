cmake_minimum_required(VERSION 3.22)
project(StereoWidener VERSION 1.9.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_plugin(StereoWidener
    COMPANY_NAME "Jamie"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR "/Users/jamiemichalski/Desktop/DSKTP/C Drive/vst3"
    PLUGIN_MANUFACTURER_CODE Jmcl
    PLUGIN_CODE Swid
    FORMATS VST3 Standalone
    PRODUCT_NAME "Stereo Widener"
)

juce_add_binary_data(StereoWidenerAssets
    SOURCES
        Assets/BebasNeue-Regular.ttf
        Source/UI/index.html
        Source/UI/style.css
        Source/UI/app.js
        Source/UI/juce_frontend.js
        Source/UI/check_native_interop.js
)

juce_generate_juce_header(StereoWidener)

target_sources(StereoWidener PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/SynthVoice.h
)

target_compile_definitions(StereoWidener PUBLIC
    JUCE_WEB_BROWSER=1
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(StereoWidener
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
        StereoWidenerAssets
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
