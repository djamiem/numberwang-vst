#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <BinaryData.h>

// ─── Browser subclass to prevent navigation away from the app ─────────────────
struct NWBrowser : juce::WebBrowserComponent
{
    using juce::WebBrowserComponent::WebBrowserComponent;

    bool pageAboutToLoad (const juce::String& newURL) override
    {
        return newURL.startsWith (juce::WebBrowserComponent::getResourceProviderRoot());
    }
};

// ─────────────────────────────────────────────────────────────────────────────

class StereoWidenerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          private juce::Timer
{
public:
    explicit StereoWidenerAudioProcessorEditor (StereoWidenerAudioProcessor&);
    ~StereoWidenerAudioProcessorEditor() override;

    void paint   (juce::Graphics&) override;
    void resized () override;

private:
    void timerCallback() override;
    std::optional<juce::WebBrowserComponent::Resource> getResource (const juce::String& url);

    static juce::WebBrowserComponent::Options buildOptions (StereoWidenerAudioProcessorEditor* ed,
        juce::WebSliderRelay& osc1LevelRelay, juce::WebSliderRelay& osc1DetuneRelay,
        juce::WebSliderRelay& osc1OctaveRelay, juce::WebSliderRelay& osc2LevelRelay,
        juce::WebSliderRelay& osc2OctaveRelay, juce::WebSliderRelay& noiseLevelRelay,
        juce::WebSliderRelay& filterCutoffRelay, juce::WebSliderRelay& filterResRelay,
        juce::WebSliderRelay& filterEnvAttackRelay, juce::WebSliderRelay& filterEnvAmountRelay,
        juce::WebSliderRelay& filterLfoRateRelay, juce::WebSliderRelay& filterLfoAmountRelay,
        juce::WebSliderRelay& attackRelay, juce::WebSliderRelay& decayRelay,
        juce::WebSliderRelay& sustainRelay, juce::WebSliderRelay& releaseRelay,
        juce::WebSliderRelay& masterVolRelay, juce::WebSliderRelay& ssawOctaveRelay,
        juce::WebSliderRelay& ssawLevelRelay, juce::WebSliderRelay& ssawDetuneRelay,
        juce::WebSliderRelay& ssawSpreadRelay, juce::WebSliderRelay& widthAmountRelay,
        juce::WebSliderRelay& chorusRateRelay, juce::WebSliderRelay& chorusDepthRelay,
        juce::WebSliderRelay& chorusMixRelay, juce::WebSliderRelay& delayTimeRelay,
        juce::WebSliderRelay& delayFeedbackRelay, juce::WebSliderRelay& delayMixRelay,
        juce::WebSliderRelay& arpBpmRelay, juce::WebToggleButtonRelay& numberwangRelay,
        juce::WebToggleButtonRelay& arpEnabledRelay, juce::WebToggleButtonRelay& arpHoldRelay,
        juce::WebToggleButtonRelay& ssawEnabledRelay, juce::WebToggleButtonRelay& widthEnabledRelay,
        juce::WebToggleButtonRelay& chorusEnabledRelay, juce::WebToggleButtonRelay& delayEnabledRelay,
        juce::WebToggleButtonRelay& filterLfoSyncRelay, juce::WebComboBoxRelay& osc1WaveRelay,
        juce::WebComboBoxRelay& osc2WaveRelay, juce::WebComboBoxRelay& arpDivisionRelay,
        juce::WebComboBoxRelay& filterLfoDivRelay);

    StereoWidenerAudioProcessor& audioProcessor;
    int keyboardOctave = 4;

    // ── Slider relays ──────────────────────────────────────────────────────────
    juce::WebSliderRelay osc1LevelRelay        { "osc1_level"         };
    juce::WebSliderRelay osc1DetuneRelay       { "osc1_detune"        };
    juce::WebSliderRelay osc1OctaveRelay       { "osc1_octave"        };
    juce::WebSliderRelay osc2LevelRelay        { "osc2_level"         };
    juce::WebSliderRelay osc2OctaveRelay       { "osc2_octave"        };
    juce::WebSliderRelay noiseLevelRelay       { "noise_level"        };
    juce::WebSliderRelay filterCutoffRelay     { "filter_cutoff"      };
    juce::WebSliderRelay filterResRelay        { "filter_res"         };
    juce::WebSliderRelay filterEnvAttackRelay  { "filter_env_attack"  };
    juce::WebSliderRelay filterEnvAmountRelay  { "filter_env_amount"  };
    juce::WebSliderRelay filterLfoRateRelay    { "filter_lfo_rate"    };
    juce::WebSliderRelay filterLfoAmountRelay  { "filter_lfo_amount"  };
    juce::WebSliderRelay attackRelay           { "attack"             };
    juce::WebSliderRelay decayRelay            { "decay"              };
    juce::WebSliderRelay sustainRelay          { "sustain"            };
    juce::WebSliderRelay releaseRelay          { "release"            };
    juce::WebSliderRelay masterVolRelay        { "master_volume"      };
    juce::WebSliderRelay ssawOctaveRelay       { "ssaw_octave"        };
    juce::WebSliderRelay ssawLevelRelay        { "ssaw_level"         };
    juce::WebSliderRelay ssawDetuneRelay       { "ssaw_detune"        };
    juce::WebSliderRelay ssawSpreadRelay       { "ssaw_spread"        };
    juce::WebSliderRelay widthAmountRelay      { "width_amount"       };
    juce::WebSliderRelay chorusRateRelay       { "chorus_rate"        };
    juce::WebSliderRelay chorusDepthRelay      { "chorus_depth"       };
    juce::WebSliderRelay chorusMixRelay        { "chorus_mix"         };
    juce::WebSliderRelay delayTimeRelay        { "delay_time"         };
    juce::WebSliderRelay delayFeedbackRelay    { "delay_feedback"     };
    juce::WebSliderRelay delayMixRelay         { "delay_mix"          };
    juce::WebSliderRelay arpBpmRelay           { "arp_bpm"            };

    // ── Toggle relays ──────────────────────────────────────────────────────────
    juce::WebToggleButtonRelay numberwangRelay    { "numberwang"       };
    juce::WebToggleButtonRelay arpEnabledRelay    { "arp_enabled"      };
    juce::WebToggleButtonRelay arpHoldRelay       { "arp_hold"         };
    juce::WebToggleButtonRelay ssawEnabledRelay   { "ssaw_enabled"     };
    juce::WebToggleButtonRelay widthEnabledRelay  { "width_enabled"    };
    juce::WebToggleButtonRelay chorusEnabledRelay { "chorus_enabled"   };
    juce::WebToggleButtonRelay delayEnabledRelay  { "delay_enabled"    };
    juce::WebToggleButtonRelay filterLfoSyncRelay { "filter_lfo_sync"  };

    // ── ComboBox relays ────────────────────────────────────────────────────────
    juce::WebComboBoxRelay osc1WaveRelay      { "osc1_wave"      };
    juce::WebComboBoxRelay osc2WaveRelay      { "osc2_wave"      };
    juce::WebComboBoxRelay arpDivisionRelay   { "arp_division"   };
    juce::WebComboBoxRelay filterLfoDivRelay  { "filter_lfo_div" };

    // ── WebView ────────────────────────────────────────────────────────────────
    NWBrowser webComponent;

    // ── Parameter attachments ──────────────────────────────────────────────────
    juce::WebSliderParameterAttachment       osc1LevelAtt, osc1DetuneAtt, osc1OctaveAtt;
    juce::WebSliderParameterAttachment       osc2LevelAtt, osc2OctaveAtt;
    juce::WebSliderParameterAttachment       noiseLevelAtt;
    juce::WebSliderParameterAttachment       filterCutoffAtt, filterResAtt;
    juce::WebSliderParameterAttachment       filterEnvAttackAtt, filterEnvAmountAtt;
    juce::WebSliderParameterAttachment       filterLfoRateAtt, filterLfoAmountAtt;
    juce::WebSliderParameterAttachment       attackAtt, decayAtt, sustainAtt, releaseAtt;
    juce::WebSliderParameterAttachment       masterVolAtt;
    juce::WebSliderParameterAttachment       ssawOctaveAtt, ssawLevelAtt, ssawDetuneAtt, ssawSpreadAtt;
    juce::WebSliderParameterAttachment       widthAmountAtt;
    juce::WebSliderParameterAttachment       chorusRateAtt, chorusDepthAtt, chorusMixAtt;
    juce::WebSliderParameterAttachment       delayTimeAtt, delayFeedbackAtt, delayMixAtt;
    juce::WebSliderParameterAttachment       arpBpmAtt;

    juce::WebToggleButtonParameterAttachment numberwangAtt, arpEnabledAtt, arpHoldAtt;
    juce::WebToggleButtonParameterAttachment ssawEnabledAtt, widthEnabledAtt;
    juce::WebToggleButtonParameterAttachment chorusEnabledAtt, delayEnabledAtt;
    juce::WebToggleButtonParameterAttachment filterLfoSyncAtt;

    juce::WebComboBoxParameterAttachment     osc1WaveAtt, osc2WaveAtt;
    juce::WebComboBoxParameterAttachment     arpDivisionAtt, filterLfoDivAtt;

    // ── MIDI keyboard (native JUCE component, sits below the WebView) ──────────
    juce::MidiKeyboardComponent keyboardComp;

    // Reclaims keyboard focus from the WebView when the mouse enters the keyboard area
    struct KeyFocusGrabber : juce::MouseListener {
        juce::Component& target;
        explicit KeyFocusGrabber (juce::Component& t) : target (t) {}
        void mouseEnter (const juce::MouseEvent&) override { target.grabKeyboardFocus(); }
    } keyboardFocusGrabber { keyboardComp };

    bool lastNwFlag = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoWidenerAudioProcessorEditor)
};
