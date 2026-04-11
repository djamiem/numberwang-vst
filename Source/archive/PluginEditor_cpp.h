#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <BinaryData.h>

class NumberwangLookAndFeel : public juce::LookAndFeel_V4
{
public:
    NumberwangLookAndFeel();

    void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h,
                          float sliderPos, float startAngle, float endAngle,
                          juce::Slider&) override;

    void drawButtonBackground(juce::Graphics&, juce::Button&,
                              const juce::Colour&, bool, bool) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&, bool, bool) override;

    juce::Font bebasFont { juce::FontOptions{} };
};

// ─────────────────────────────────────────────────────────────────────────────

struct OscSection
{
    juce::Label      label;
    juce::TextButton sinBtn { "SINE" };
    juce::TextButton sqrBtn { "SQR"  };
    juce::TextButton triBtn { "TRI"  };
    juce::Slider     levelSlider;
    juce::Slider     octaveSlider;
    juce::Label      octaveLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> octaveAtt;
};

// ─────────────────────────────────────────────────────────────────────────────

class StereoWidenerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          private juce::Timer
{
public:
    explicit StereoWidenerAudioProcessorEditor(StereoWidenerAudioProcessor&);
    ~StereoWidenerAudioProcessorEditor() override;

    void paint            (juce::Graphics&) override;
    void paintOverChildren(juce::Graphics&) override;
    void resized          () override;

    bool keyPressed      (const juce::KeyPress&) override;
    bool keyStateChanged (bool isKeyDown)         override;
    void focusLost       (FocusChangeType)        override;

private:
    void timerCallback() override;
    void setupOscSection(OscSection&, const juce::String& name,
                         const juce::String& waveParamId,
                         const juce::String& levelParamId,
                         const juce::String& octaveParamId,
                         int radioGroupId);
    void updateWaveButtons(OscSection&, const juce::String& waveParamId);
    void updateDivButtons();
    void updateLfoDivButtons();

    StereoWidenerAudioProcessor& audioProcessor;
    NumberwangLookAndFeel        numberwangLAF;

    // Oscillators
    OscSection osc1, osc2;

    juce::Slider detuneSlider;
    juce::Label  detuneLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment detuneAtt;

    // Arpeggiator
    juce::TextButton arpToggle  { "ARP"  };
    juce::TextButton holdToggle { "HOLD" };
    juce::AudioProcessorValueTreeState::ButtonAttachment arpAtt, holdAtt;

    juce::TextButton div4Btn  { "1/4"  };
    juce::TextButton div8Btn  { "1/8"  };
    juce::TextButton div16Btn { "1/16" };
    juce::TextButton div32Btn { "1/32" };

    juce::Slider bpmSlider;
    juce::Label  bpmLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment bpmAtt;

    // Noise
    juce::Slider noiseLevelSlider;
    juce::Label  noiseLevelLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment noiseLevelAtt;

    // Filter
    juce::Slider filterCutoffSlider, filterResSlider;
    juce::Label  filterCutoffLabel,  filterResLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment filterCutoffAtt, filterResAtt;

    // Filter Envelope
    juce::Slider filterEnvAttackSlider, filterEnvAmountSlider;
    juce::Label  filterEnvAttackLabel,  filterEnvAmountLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment filterEnvAttackAtt, filterEnvAmountAtt;

    // Filter LFO
    juce::Slider     filterLfoRateSlider, filterLfoAmountSlider;
    juce::Label      filterLfoRateLabel,  filterLfoAmountLabel;
    juce::TextButton filterLfoSyncBtn { "SYNC" };
    juce::AudioProcessorValueTreeState::ButtonAttachment filterLfoSyncAtt;
    juce::TextButton lfoDiv16Btn { "1/16" };
    juce::TextButton lfoDiv8Btn  { "1/8"  };
    juce::TextButton lfoDiv4Btn  { "1/4"  };
    juce::TextButton lfoDiv2Btn  { "1/2"  };
    juce::TextButton lfoDiv1Btn  { "1 BAR" };
    juce::AudioProcessorValueTreeState::SliderAttachment filterLfoRateAtt, filterLfoAmountAtt;

    // ADSR
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label  attackLabel,  decayLabel,  sustainLabel,  releaseLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment
        attackAtt, decayAtt, sustainAtt, releaseAtt;

    // Numberwang
    juce::TextButton numberwangToggle { "NUMBERWANG" };
    juce::AudioProcessorValueTreeState::ButtonAttachment numberwangAtt;

    // Master volume
    juce::Slider masterVolSlider;
    juce::Label  masterVolLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment masterVolAtt;

    // Panic
    juce::TextButton panicBtn { "PANIC" };

    // ── Right column ──────────────────────────────────────────────────────────

    // Super Saw
    juce::TextButton ssawEnableBtn  { "ON" };
    juce::AudioProcessorValueTreeState::ButtonAttachment ssawEnableAtt;
    juce::Slider ssawLevelSlider,  ssawDetuneSlider,  ssawSpreadSlider;
    juce::Label  ssawLevelLabel,   ssawDetuneLabel,   ssawSpreadLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment ssawLevelAtt, ssawDetuneAtt, ssawSpreadAtt;
    juce::Slider ssawOctaveSlider;
    juce::Label  ssawOctaveLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment ssawOctaveAtt;

    // Stereo Width
    juce::TextButton widthEnableBtn { "ON" };
    juce::AudioProcessorValueTreeState::ButtonAttachment widthEnableAtt;
    juce::Slider widthAmountSlider;
    juce::Label  widthAmountLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment widthAmountAtt;

    // Chorus
    juce::TextButton chorusEnableBtn { "ON" };
    juce::AudioProcessorValueTreeState::ButtonAttachment chorusEnableAtt;
    juce::Slider chorusRateSlider, chorusDepthSlider, chorusMixSlider;
    juce::Label  chorusRateLabel,  chorusDepthLabel,  chorusMixLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment chorusRateAtt, chorusDepthAtt, chorusMixAtt;

    // Delay
    juce::TextButton delayEnableBtn { "ON" };
    juce::AudioProcessorValueTreeState::ButtonAttachment delayEnableAtt;
    juce::Slider delayTimeSlider, delayFeedbackSlider, delayMixSlider;
    juce::Label  delayTimeLabel,  delayFeedbackLabel,  delayMixLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment delayTimeAtt, delayFeedbackAtt, delayMixAtt;

    // ─────────────────────────────────────────────────────────────────────────
    juce::MidiKeyboardComponent keyboardComp;

    int numberwangFlashFrames = 0;
    int keyboardOctave = 4;   // base octave for QWERTY keyboard input

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoWidenerAudioProcessorEditor)
};
