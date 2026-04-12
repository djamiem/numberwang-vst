#include "PluginEditor.h"

// ── Resource provider helpers ─────────────────────────────────────────────────

static const char* mimeFor (const juce::String& ext)
{
    if (ext == "html") return "text/html";
    if (ext == "css")  return "text/css";
    if (ext == "js")   return "text/javascript";
    if (ext == "ttf")  return "font/truetype";
    if (ext == "json") return "application/json";
    return "application/octet-stream";
}

struct UIFile { const char* path; const char* data; int size; };

static const UIFile uiFiles[] =
{
    { "index.html",              BinaryData::index_html,              BinaryData::index_htmlSize              },
    { "style.css",               BinaryData::style_css,               BinaryData::style_cssSize               },
    { "app.js",                  BinaryData::app_js,                  BinaryData::app_jsSize                  },
    { "juce_frontend.js",        BinaryData::juce_frontend_js,        BinaryData::juce_frontend_jsSize        },
    { "check_native_interop.js", BinaryData::check_native_interop_js, BinaryData::check_native_interop_jsSize },
    { "BebasNeue-Regular.ttf",   BinaryData::BebasNeueRegular_ttf,    BinaryData::BebasNeueRegular_ttfSize    },
};

std::optional<juce::WebBrowserComponent::Resource>
NumberwangAudioProcessorEditor::getResource (const juce::String& url)
{
    const auto file = (url == "/" || url.isEmpty())
                        ? juce::String ("index.html")
                        : url.fromFirstOccurrenceOf ("/", false, false);

    const auto ext = file.fromLastOccurrenceOf (".", false, false);

#ifdef NUMBERWANG_DEV_UI
    // Dev mode: serve directly from Source/UI/ on disk so HTML/CSS/JS changes
    // are visible after a standalone relaunch with no rebuild required.
    // __FILE__ resolves to the absolute path of this .cpp file at compile time.
    static const juce::File devDir = juce::File (__FILE__).getSiblingFile ("UI");
    const auto diskFile = devDir.getChildFile (file);
    if (diskFile.existsAsFile())
    {
        juce::MemoryBlock mb;
        diskFile.loadFileAsData (mb);
        return juce::WebBrowserComponent::Resource {
            std::vector<std::byte> (
                static_cast<const std::byte*> (mb.getData()),
                static_cast<const std::byte*> (mb.getData()) + mb.getSize()),
            mimeFor (ext)
        };
    }
    // Fall through to BinaryData for assets not in Source/UI/ (e.g. the font)
#endif

    for (const auto& f : uiFiles)
    {
        if (file == f.path)
        {
            std::vector<std::byte> data ((size_t) f.size);
            std::memcpy (data.data(), f.data, (size_t) f.size);
            return juce::WebBrowserComponent::Resource { std::move (data), mimeFor (ext) };
        }
    }

    return std::nullopt;
}

// ── Build the WebBrowserComponent Options ─────────────────────────────────────
// (returns by value so it can be used in the member initialiser list)

juce::WebBrowserComponent::Options NumberwangAudioProcessorEditor::buildOptions (NumberwangAudioProcessorEditor* ed,
                                                         juce::WebSliderRelay& osc1LevelRelay,
                                                         juce::WebSliderRelay& osc1DetuneRelay,
                                                         juce::WebSliderRelay& osc1OctaveRelay,
                                                         juce::WebSliderRelay& osc2LevelRelay,
                                                         juce::WebSliderRelay& osc2OctaveRelay,
                                                         juce::WebSliderRelay& noiseLevelRelay,
                                                         juce::WebSliderRelay& filterCutoffRelay,
                                                         juce::WebSliderRelay& filterResRelay,
                                                         juce::WebSliderRelay& filterEnvAttackRelay,
                                                         juce::WebSliderRelay& filterEnvAmountRelay,
                                                         juce::WebSliderRelay& filterLfoRateRelay,
                                                         juce::WebSliderRelay& filterLfoAmountRelay,
                                                         juce::WebSliderRelay& attackRelay,
                                                         juce::WebSliderRelay& decayRelay,
                                                         juce::WebSliderRelay& sustainRelay,
                                                         juce::WebSliderRelay& releaseRelay,
                                                         juce::WebSliderRelay& masterVolRelay,
                                                         juce::WebSliderRelay& ssawOctaveRelay,
                                                         juce::WebSliderRelay& ssawLevelRelay,
                                                         juce::WebSliderRelay& ssawDetuneRelay,
                                                         juce::WebSliderRelay& ssawSpreadRelay,
                                                         juce::WebSliderRelay& widthAmountRelay,
                                                         juce::WebSliderRelay& chorusRateRelay,
                                                         juce::WebSliderRelay& chorusDepthRelay,
                                                         juce::WebSliderRelay& chorusMixRelay,
                                                         juce::WebSliderRelay& delayTimeRelay,
                                                         juce::WebSliderRelay& delayFeedbackRelay,
                                                         juce::WebSliderRelay& delayMixRelay,
                                                         juce::WebSliderRelay& arpBpmRelay,
                                                         juce::WebToggleButtonRelay& numberwangRelay,
                                                         juce::WebToggleButtonRelay& arpEnabledRelay,
                                                         juce::WebToggleButtonRelay& arpHoldRelay,
                                                         juce::WebToggleButtonRelay& ssawEnabledRelay,
                                                         juce::WebToggleButtonRelay& widthEnabledRelay,
                                                         juce::WebToggleButtonRelay& chorusEnabledRelay,
                                                         juce::WebToggleButtonRelay& delayEnabledRelay,
                                                         juce::WebToggleButtonRelay& filterLfoSyncRelay,
                                                         juce::WebComboBoxRelay& osc1WaveRelay,
                                                         juce::WebComboBoxRelay& osc2WaveRelay,
                                                         juce::WebComboBoxRelay& arpDivisionRelay,
                                                         juce::WebComboBoxRelay& filterLfoDivRelay)
{
    return juce::WebBrowserComponent::Options{}
        .withBackend (juce::WebBrowserComponent::Options::Backend::webview2)
        .withWinWebView2Options (
            juce::WebBrowserComponent::Options::WinWebView2{}
                .withUserDataFolder (juce::File::getSpecialLocation (
                    juce::File::SpecialLocationType::tempDirectory)))
        .withNativeIntegrationEnabled()
        // ── Slider relays
        .withOptionsFrom (osc1LevelRelay)
        .withOptionsFrom (osc1DetuneRelay)
        .withOptionsFrom (osc1OctaveRelay)
        .withOptionsFrom (osc2LevelRelay)
        .withOptionsFrom (osc2OctaveRelay)
        .withOptionsFrom (noiseLevelRelay)
        .withOptionsFrom (filterCutoffRelay)
        .withOptionsFrom (filterResRelay)
        .withOptionsFrom (filterEnvAttackRelay)
        .withOptionsFrom (filterEnvAmountRelay)
        .withOptionsFrom (filterLfoRateRelay)
        .withOptionsFrom (filterLfoAmountRelay)
        .withOptionsFrom (attackRelay)
        .withOptionsFrom (decayRelay)
        .withOptionsFrom (sustainRelay)
        .withOptionsFrom (releaseRelay)
        .withOptionsFrom (masterVolRelay)
        .withOptionsFrom (ssawOctaveRelay)
        .withOptionsFrom (ssawLevelRelay)
        .withOptionsFrom (ssawDetuneRelay)
        .withOptionsFrom (ssawSpreadRelay)
        .withOptionsFrom (widthAmountRelay)
        .withOptionsFrom (chorusRateRelay)
        .withOptionsFrom (chorusDepthRelay)
        .withOptionsFrom (chorusMixRelay)
        .withOptionsFrom (delayTimeRelay)
        .withOptionsFrom (delayFeedbackRelay)
        .withOptionsFrom (delayMixRelay)
        .withOptionsFrom (arpBpmRelay)
        // ── Toggle relays
        .withOptionsFrom (numberwangRelay)
        .withOptionsFrom (arpEnabledRelay)
        .withOptionsFrom (arpHoldRelay)
        .withOptionsFrom (ssawEnabledRelay)
        .withOptionsFrom (widthEnabledRelay)
        .withOptionsFrom (chorusEnabledRelay)
        .withOptionsFrom (delayEnabledRelay)
        .withOptionsFrom (filterLfoSyncRelay)
        // ── Combo relays
        .withOptionsFrom (osc1WaveRelay)
        .withOptionsFrom (osc2WaveRelay)
        .withOptionsFrom (arpDivisionRelay)
        .withOptionsFrom (filterLfoDivRelay)
        // ── Native functions
        .withNativeFunction ("panic", [ed] (const auto&, auto complete)
        {
            ed->audioProcessor.panicRequested.store (true);
            complete (juce::var{});
        })
        .withNativeFunction ("shiftOctave", [ed] (const auto& args, auto complete)
        {
            const int dir = args.isEmpty() ? 0 : (int) args[0];
            ed->keyboardOctave = juce::jlimit (0, 8, ed->keyboardOctave + dir);
            ed->keyboardComp.setKeyPressBaseOctave (ed->keyboardOctave);
            complete (juce::var (ed->keyboardOctave));
        })
        .withNativeFunction ("noteOn", [ed] (const auto& args, auto complete)
        {
            if (args.size() >= 1)
            {
                const int note = juce::jlimit (0, 127, (int) args[0]);
                const float vel = args.size() >= 2 ? juce::jlimit (0.0f, 1.0f, (float) args[1]) : 0.8f;
                ed->audioProcessor.keyboardState.noteOn (1, note, vel);
            }
            complete (juce::var{});
        })
        .withNativeFunction ("noteOff", [ed] (const auto& args, auto complete)
        {
            if (!args.isEmpty())
            {
                const int note = juce::jlimit (0, 127, (int) args[0]);
                ed->audioProcessor.keyboardState.noteOff (1, note, 0.0f);
            }
            complete (juce::var{});
        })
        // ── Resource provider (serves HTML/CSS/JS from binary data)
        .withResourceProvider ([ed] (const auto& url)
        {
            return ed->getResource (url);
        });
}

// ── Constructor ───────────────────────────────────────────────────────────────

NumberwangAudioProcessorEditor::NumberwangAudioProcessorEditor (NumberwangAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      // ── WebView built with all relays wired
      webComponent (buildOptions (this,
                                  osc1LevelRelay, osc1DetuneRelay, osc1OctaveRelay,
                                  osc2LevelRelay, osc2OctaveRelay,
                                  noiseLevelRelay,
                                  filterCutoffRelay, filterResRelay,
                                  filterEnvAttackRelay, filterEnvAmountRelay,
                                  filterLfoRateRelay, filterLfoAmountRelay,
                                  attackRelay, decayRelay, sustainRelay, releaseRelay,
                                  masterVolRelay,
                                  ssawOctaveRelay, ssawLevelRelay, ssawDetuneRelay, ssawSpreadRelay,
                                  widthAmountRelay,
                                  chorusRateRelay, chorusDepthRelay, chorusMixRelay,
                                  delayTimeRelay, delayFeedbackRelay, delayMixRelay,
                                  arpBpmRelay,
                                  numberwangRelay, arpEnabledRelay, arpHoldRelay,
                                  ssawEnabledRelay, widthEnabledRelay,
                                  chorusEnabledRelay, delayEnabledRelay,
                                  filterLfoSyncRelay,
                                  osc1WaveRelay, osc2WaveRelay,
                                  arpDivisionRelay, filterLfoDivRelay)),
      // ── Slider attachments
      osc1LevelAtt       (*p.apvts.getParameter ("osc1_level"),         osc1LevelRelay),
      osc1DetuneAtt      (*p.apvts.getParameter ("osc1_detune"),        osc1DetuneRelay),
      osc1OctaveAtt      (*p.apvts.getParameter ("osc1_octave"),        osc1OctaveRelay),
      osc2LevelAtt       (*p.apvts.getParameter ("osc2_level"),         osc2LevelRelay),
      osc2OctaveAtt      (*p.apvts.getParameter ("osc2_octave"),        osc2OctaveRelay),
      noiseLevelAtt      (*p.apvts.getParameter ("noise_level"),        noiseLevelRelay),
      filterCutoffAtt    (*p.apvts.getParameter ("filter_cutoff"),      filterCutoffRelay),
      filterResAtt       (*p.apvts.getParameter ("filter_res"),         filterResRelay),
      filterEnvAttackAtt (*p.apvts.getParameter ("filter_env_attack"),  filterEnvAttackRelay),
      filterEnvAmountAtt (*p.apvts.getParameter ("filter_env_amount"),  filterEnvAmountRelay),
      filterLfoRateAtt   (*p.apvts.getParameter ("filter_lfo_rate"),    filterLfoRateRelay),
      filterLfoAmountAtt (*p.apvts.getParameter ("filter_lfo_amount"),  filterLfoAmountRelay),
      attackAtt          (*p.apvts.getParameter ("attack"),             attackRelay),
      decayAtt           (*p.apvts.getParameter ("decay"),              decayRelay),
      sustainAtt         (*p.apvts.getParameter ("sustain"),            sustainRelay),
      releaseAtt         (*p.apvts.getParameter ("release"),            releaseRelay),
      masterVolAtt       (*p.apvts.getParameter ("master_volume"),      masterVolRelay),
      ssawOctaveAtt      (*p.apvts.getParameter ("ssaw_octave"),        ssawOctaveRelay),
      ssawLevelAtt       (*p.apvts.getParameter ("ssaw_level"),         ssawLevelRelay),
      ssawDetuneAtt      (*p.apvts.getParameter ("ssaw_detune"),        ssawDetuneRelay),
      ssawSpreadAtt      (*p.apvts.getParameter ("ssaw_spread"),        ssawSpreadRelay),
      widthAmountAtt     (*p.apvts.getParameter ("width_amount"),       widthAmountRelay),
      chorusRateAtt      (*p.apvts.getParameter ("chorus_rate"),        chorusRateRelay),
      chorusDepthAtt     (*p.apvts.getParameter ("chorus_depth"),       chorusDepthRelay),
      chorusMixAtt       (*p.apvts.getParameter ("chorus_mix"),         chorusMixRelay),
      delayTimeAtt       (*p.apvts.getParameter ("delay_time"),         delayTimeRelay),
      delayFeedbackAtt   (*p.apvts.getParameter ("delay_feedback"),     delayFeedbackRelay),
      delayMixAtt        (*p.apvts.getParameter ("delay_mix"),          delayMixRelay),
      arpBpmAtt          (*p.apvts.getParameter ("arp_bpm"),            arpBpmRelay),
      // ── Toggle attachments
      numberwangAtt    (*p.apvts.getParameter ("numberwang"),      numberwangRelay),
      arpEnabledAtt    (*p.apvts.getParameter ("arp_enabled"),     arpEnabledRelay),
      arpHoldAtt       (*p.apvts.getParameter ("arp_hold"),        arpHoldRelay),
      ssawEnabledAtt   (*p.apvts.getParameter ("ssaw_enabled"),    ssawEnabledRelay),
      widthEnabledAtt  (*p.apvts.getParameter ("width_enabled"),   widthEnabledRelay),
      chorusEnabledAtt (*p.apvts.getParameter ("chorus_enabled"),  chorusEnabledRelay),
      delayEnabledAtt  (*p.apvts.getParameter ("delay_enabled"),   delayEnabledRelay),
      filterLfoSyncAtt (*p.apvts.getParameter ("filter_lfo_sync"), filterLfoSyncRelay),
      // ── Combo attachments
      osc1WaveAtt     (*p.apvts.getParameter ("osc1_wave"),      osc1WaveRelay),
      osc2WaveAtt     (*p.apvts.getParameter ("osc2_wave"),      osc2WaveRelay),
      arpDivisionAtt  (*p.apvts.getParameter ("arp_division"),   arpDivisionRelay),
      filterLfoDivAtt (*p.apvts.getParameter ("filter_lfo_div"), filterLfoDivRelay),
      // ── MIDI keyboard
      keyboardComp (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    addAndMakeVisible (webComponent);
    webComponent.goToURL (juce::WebBrowserComponent::getResourceProviderRoot());

    keyboardComp.setAvailableRange (36, 96);
    keyboardComp.setKeyPressBaseOctave (keyboardOctave);
    keyboardComp.setColour (juce::MidiKeyboardComponent::whiteNoteColourId,
                            juce::Colours::white);
    keyboardComp.setColour (juce::MidiKeyboardComponent::blackNoteColourId,
                            juce::Colour (0xff001166));
    keyboardComp.setColour (juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId,
                            juce::Colour (0x80ffd700));
    keyboardComp.setColour (juce::MidiKeyboardComponent::keyDownOverlayColourId,
                            juce::Colour (0xffffd700));
    addAndMakeVisible (keyboardComp);
    keyboardComp.addMouseListener (&keyboardFocusGrabber, false);
    keyboardComp.grabKeyboardFocus();

    setSize (1200, 840);
    startTimerHz (30);
}

NumberwangAudioProcessorEditor::~NumberwangAudioProcessorEditor()
{
    stopTimer();
}

// ── Timer: push numberwang display state to JS ────────────────────────────────

void NumberwangAudioProcessorEditor::timerCallback()
{
    const bool fired = audioProcessor.numberwangFlag.exchange (false);

    juce::DynamicObject::Ptr d (new juce::DynamicObject());
    d->setProperty ("roll",  audioProcessor.randomNumber.load());
    d->setProperty ("note",  audioProcessor.lastNoteNumber.load());
    d->setProperty ("fired", fired);

    webComponent.emitEventIfBrowserIsVisible ("numberwang_display", juce::var (d.get()));
}

// ── Paint / Resized ───────────────────────────────────────────────────────────

void NumberwangAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff001166));
}

void NumberwangAudioProcessorEditor::resized()
{
    const int keyH = 48;
    webComponent .setBounds (0, 0,          getWidth(), getHeight() - keyH);
    keyboardComp .setBounds (0, getHeight() - keyH, getWidth(), keyH);
}
