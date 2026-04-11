#include "PluginEditor.h"

static void drawStar(juce::Graphics& g, float cx, float cy, float outerR, float innerR)
{
    juce::Path star;
    for (int i = 0; i < 10; ++i)
    {
        auto angle = juce::MathConstants<float>::pi * (float)i / 5.0f
                     - juce::MathConstants<float>::halfPi;
        auto r = (i % 2 == 0) ? outerR : innerR;
        juce::Point<float> pt(cx + r * std::cos(angle), cy + r * std::sin(angle));
        if (i == 0) star.startNewSubPath(pt);
        else         star.lineTo(pt);
    }
    star.closeSubPath();
    g.fillPath(star);
}

// ─── NumberwangLookAndFeel ────────────────────────────────────────────────────

NumberwangLookAndFeel::NumberwangLookAndFeel()
{
    bebasFont = juce::Font(juce::FontOptions(
        juce::Typeface::createSystemTypefaceFor(
            BinaryData::BebasNeueRegular_ttf,
            BinaryData::BebasNeueRegular_ttfSize)).withHeight(20.0f));

    setColour(juce::Slider::textBoxTextColourId,       juce::Colour(0xffffd700));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff001166));
    setColour(juce::Slider::textBoxOutlineColourId,    juce::Colour(0xffffd700));
    setColour(juce::Slider::textBoxHighlightColourId,  juce::Colour(0x40ffd700));
}

void NumberwangLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                              int x, int y, int width, int height,
                                              float sliderPos,
                                              float rotaryStartAngle, float rotaryEndAngle,
                                              juce::Slider&)
{
    const auto gold     = juce::Colour(0xffffd700);
    const auto darkBlue = juce::Colour(0xff001166);
    auto radius  = (float)juce::jmin(width / 2, height / 2) - 6.0f;
    auto centreX = (float)x + (float)width  * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto angle   = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto innerR  = radius - 4.0f;

    g.setColour(gold);
    g.drawEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f, 3.0f);
    g.setColour(darkBlue);
    g.fillEllipse(centreX - innerR, centreY - innerR, innerR * 2.0f, innerR * 2.0f);

    juce::Path arc;
    arc.addArc(centreX - radius + 2.0f, centreY - radius + 2.0f,
               (radius - 2.0f) * 2.0f, (radius - 2.0f) * 2.0f,
               rotaryStartAngle, angle, true);
    g.setColour(gold);
    g.strokePath(arc, juce::PathStrokeType(3.0f));

    juce::Path ptr;
    ptr.addRectangle(-1.5f, -(innerR - 2.0f), 3.0f, innerR * 0.55f);
    ptr.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.fillPath(ptr);
    g.fillEllipse(centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
}

void NumberwangLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& btn,
                                                  const juce::Colour&, bool, bool isDown)
{
    const auto gold     = juce::Colour(0xffffd700);
    const auto darkBlue = juce::Colour(0xff001166);
    bool active = btn.getToggleState();
    g.setColour((active || isDown) ? gold : darkBlue);
    g.fillRoundedRectangle(btn.getLocalBounds().toFloat(), 3.0f);
    g.setColour(gold);
    g.drawRoundedRectangle(btn.getLocalBounds().toFloat().reduced(0.5f), 3.0f, 1.5f);
}

void NumberwangLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& btn,
                                            bool, bool)
{
    g.setColour(btn.getToggleState() ? juce::Colour(0xff001166) : juce::Colour(0xffffd700));
    g.setFont(bebasFont.withHeight(15.0f));
    g.drawFittedText(btn.getButtonText(), btn.getLocalBounds(),
                     juce::Justification::centred, 1);
}

// ─── Editor ──────────────────────────────────────────────────────────────────

StereoWidenerAudioProcessorEditor::StereoWidenerAudioProcessorEditor(StereoWidenerAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      detuneAtt   (p.apvts, "osc1_detune", detuneSlider),
      arpAtt      (p.apvts, "arp_enabled", arpToggle),
      holdAtt     (p.apvts, "arp_hold",    holdToggle),
      bpmAtt      (p.apvts, "arp_bpm",     bpmSlider),
      noiseLevelAtt  (p.apvts, "noise_level",   noiseLevelSlider),
      filterCutoffAtt(p.apvts, "filter_cutoff", filterCutoffSlider),
      filterResAtt   (p.apvts, "filter_res",    filterResSlider),
      filterEnvAttackAtt (p.apvts, "filter_env_attack", filterEnvAttackSlider),
      filterEnvAmountAtt (p.apvts, "filter_env_amount", filterEnvAmountSlider),
      filterLfoRateAtt   (p.apvts, "filter_lfo_rate",   filterLfoRateSlider),
      filterLfoAmountAtt (p.apvts, "filter_lfo_amount", filterLfoAmountSlider),
      filterLfoSyncAtt   (p.apvts, "filter_lfo_sync",   filterLfoSyncBtn),
      attackAtt   (p.apvts, "attack",      attackSlider),
      decayAtt    (p.apvts, "decay",       decaySlider),
      sustainAtt  (p.apvts, "sustain",     sustainSlider),
      releaseAtt  (p.apvts, "release",     releaseSlider),
      numberwangAtt(p.apvts, "numberwang", numberwangToggle),
      masterVolAtt   (p.apvts, "master_volume",  masterVolSlider),
      ssawEnableAtt  (p.apvts, "ssaw_enabled",  ssawEnableBtn),
      ssawLevelAtt   (p.apvts, "ssaw_level",    ssawLevelSlider),
      ssawDetuneAtt  (p.apvts, "ssaw_detune",   ssawDetuneSlider),
      ssawSpreadAtt  (p.apvts, "ssaw_spread",   ssawSpreadSlider),
      ssawOctaveAtt  (p.apvts, "ssaw_octave",   ssawOctaveSlider),
      widthEnableAtt (p.apvts, "width_enabled", widthEnableBtn),
      widthAmountAtt (p.apvts, "width_amount",  widthAmountSlider),
      chorusEnableAtt(p.apvts, "chorus_enabled",chorusEnableBtn),
      chorusRateAtt  (p.apvts, "chorus_rate",   chorusRateSlider),
      chorusDepthAtt (p.apvts, "chorus_depth",  chorusDepthSlider),
      chorusMixAtt   (p.apvts, "chorus_mix",    chorusMixSlider),
      delayEnableAtt (p.apvts, "delay_enabled", delayEnableBtn),
      delayTimeAtt   (p.apvts, "delay_time",    delayTimeSlider),
      delayFeedbackAtt(p.apvts,"delay_feedback",delayFeedbackSlider),
      delayMixAtt    (p.apvts, "delay_mix",     delayMixSlider),
      keyboardComp(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    const auto gold = juce::Colour(0xffffd700);

    // Oscillator sections
    setupOscSection(osc1, "OSC 1", "osc1_wave", "osc1_level", "osc1_octave", 1);
    setupOscSection(osc2, "OSC 2", "osc2_wave", "osc2_level", "osc2_octave", 2);

    // Detune (OSC 1 only)
    detuneSlider.setLookAndFeel(&numberwangLAF);
    detuneSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    detuneSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 22);
    addAndMakeVisible(detuneSlider);

    detuneLabel.setText("DETUNE", juce::dontSendNotification);
    detuneLabel.setFont(numberwangLAF.bebasFont.withHeight(16.0f));
    detuneLabel.setColour(juce::Label::textColourId, gold.withAlpha(0.7f));
    detuneLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(detuneLabel);

    // Arp toggles
    for (auto* btn : { &arpToggle, &holdToggle })
    {
        btn->setLookAndFeel(&numberwangLAF);
        btn->setClickingTogglesState(true);
        addAndMakeVisible(btn);
    }

    // Division buttons (radio group 10)
    for (auto* btn : { &div4Btn, &div8Btn, &div16Btn, &div32Btn })
    {
        btn->setLookAndFeel(&numberwangLAF);
        btn->setRadioGroupId(10);
        btn->setClickingTogglesState(true);
        addAndMakeVisible(btn);
    }

    div4Btn.onClick  = [this] { audioProcessor.apvts.getParameter("arp_division")->setValueNotifyingHost(0.0f / 3.0f); };
    div8Btn.onClick  = [this] { audioProcessor.apvts.getParameter("arp_division")->setValueNotifyingHost(1.0f / 3.0f); };
    div16Btn.onClick = [this] { audioProcessor.apvts.getParameter("arp_division")->setValueNotifyingHost(2.0f / 3.0f); };
    div32Btn.onClick = [this] { audioProcessor.apvts.getParameter("arp_division")->setValueNotifyingHost(3.0f / 3.0f); };

    // BPM slider
    bpmSlider.setLookAndFeel(&numberwangLAF);
    bpmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    bpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 22);
    addAndMakeVisible(bpmSlider);

    bpmLabel.setText("BPM", juce::dontSendNotification);
    bpmLabel.setFont(numberwangLAF.bebasFont.withHeight(16.0f));
    bpmLabel.setColour(juce::Label::textColourId, gold.withAlpha(0.7f));
    bpmLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(bpmLabel);

    // Master volume knob (scoreboard right area)
    masterVolSlider.setLookAndFeel(&numberwangLAF);
    masterVolSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 64, 16);
    addAndMakeVisible(masterVolSlider);

    masterVolLabel.setText("MASTER VOL", juce::dontSendNotification);
    masterVolLabel.setFont(numberwangLAF.bebasFont.withHeight(14.0f));
    masterVolLabel.setColour(juce::Label::textColourId, gold.withAlpha(0.7f));
    masterVolLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(masterVolLabel);

    // Panic button
    panicBtn.setLookAndFeel(&numberwangLAF);
    panicBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff880000));
    panicBtn.onClick = [this] { audioProcessor.panicRequested.store(true); };
    addAndMakeVisible(panicBtn);

    // Noise level
    noiseLevelSlider.setLookAndFeel(&numberwangLAF);
    noiseLevelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    noiseLevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 22);
    addAndMakeVisible(noiseLevelSlider);

    noiseLevelLabel.setText("NOISE", juce::dontSendNotification);
    noiseLevelLabel.setFont(numberwangLAF.bebasFont.withHeight(16.0f));
    noiseLevelLabel.setColour(juce::Label::textColourId, gold.withAlpha(0.7f));
    noiseLevelLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(noiseLevelLabel);

    // Filter
    auto setupHSlider = [&](juce::Slider& s, juce::Label& lbl, const juce::String& name)
    {
        s.setLookAndFeel(&numberwangLAF);
        s.setSliderStyle(juce::Slider::LinearHorizontal);
        s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 22);
        addAndMakeVisible(s);
        lbl.setText(name, juce::dontSendNotification);
        lbl.setFont(numberwangLAF.bebasFont.withHeight(16.0f));
        lbl.setColour(juce::Label::textColourId, gold.withAlpha(0.7f));
        lbl.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(lbl);
    };
    setupHSlider(filterCutoffSlider, filterCutoffLabel, "CUTOFF");
    filterCutoffSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    filterCutoffLabel.setJustificationType(juce::Justification::centred);

    setupHSlider(filterResSlider, filterResLabel, "RES");
    filterResSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    filterResSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    filterResLabel.setJustificationType(juce::Justification::centred);

    // Filter Envelope sliders
    setupHSlider(filterEnvAttackSlider, filterEnvAttackLabel, "ENV ATK");
    setupHSlider(filterEnvAmountSlider, filterEnvAmountLabel, "ENV AMT");

    // Filter LFO
    filterLfoSyncBtn.setLookAndFeel(&numberwangLAF);
    filterLfoSyncBtn.setClickingTogglesState(true);
    addAndMakeVisible(filterLfoSyncBtn);

    for (auto* btn : { &lfoDiv16Btn, &lfoDiv8Btn, &lfoDiv4Btn, &lfoDiv2Btn, &lfoDiv1Btn })
    {
        btn->setLookAndFeel(&numberwangLAF);
        btn->setRadioGroupId(20);
        btn->setClickingTogglesState(true);
        addAndMakeVisible(btn);
    }

    lfoDiv16Btn.onClick = [this] { audioProcessor.apvts.getParameter("filter_lfo_div")->setValueNotifyingHost(0.0f); };
    lfoDiv8Btn .onClick = [this] { audioProcessor.apvts.getParameter("filter_lfo_div")->setValueNotifyingHost(0.25f); };
    lfoDiv4Btn .onClick = [this] { audioProcessor.apvts.getParameter("filter_lfo_div")->setValueNotifyingHost(0.5f); };
    lfoDiv2Btn .onClick = [this] { audioProcessor.apvts.getParameter("filter_lfo_div")->setValueNotifyingHost(0.75f); };
    lfoDiv1Btn .onClick = [this] { audioProcessor.apvts.getParameter("filter_lfo_div")->setValueNotifyingHost(1.0f); };

    setupHSlider(filterLfoRateSlider,   filterLfoRateLabel,   "LFO RATE");
    setupHSlider(filterLfoAmountSlider, filterLfoAmountLabel, "LFO AMT");

    // ADSR knobs
    auto setupKnob = [&](juce::Slider& s, juce::Label& lbl, const juce::String& name)
    {
        s.setLookAndFeel(&numberwangLAF);
        s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 32);
        addAndMakeVisible(s);
        lbl.setText(name, juce::dontSendNotification);
        lbl.setJustificationType(juce::Justification::centred);
        lbl.setFont(numberwangLAF.bebasFont.withHeight(18.0f));
        lbl.setColour(juce::Label::textColourId, gold);
        addAndMakeVisible(lbl);
    };
    setupKnob(attackSlider,  attackLabel,  "ATTACK");
    setupKnob(decaySlider,   decayLabel,   "DECAY");
    setupKnob(sustainSlider, sustainLabel, "SUSTAIN");
    setupKnob(releaseSlider, releaseLabel, "RELEASE");

    // Numberwang toggle
    numberwangToggle.setLookAndFeel(&numberwangLAF);
    numberwangToggle.setClickingTogglesState(true);
    addAndMakeVisible(numberwangToggle);

    // ── Right column setup ────────────────────────────────────────────────────

    auto setupEnableBtn = [&](juce::TextButton& btn)
    {
        btn.setLookAndFeel(&numberwangLAF);
        btn.setClickingTogglesState(true);
        addAndMakeVisible(btn);
    };

    // Super Saw
    setupEnableBtn(ssawEnableBtn);
    setupHSlider(ssawOctaveSlider, ssawOctaveLabel, "OCT");
    setupHSlider(ssawLevelSlider,  ssawLevelLabel,  "LEVEL");
    setupHSlider(ssawDetuneSlider, ssawDetuneLabel, "DETUNE");
    setupHSlider(ssawSpreadSlider, ssawSpreadLabel, "SPREAD");

    // Stereo Width
    setupEnableBtn(widthEnableBtn);
    setupHSlider(widthAmountSlider, widthAmountLabel, "WIDTH");

    // Chorus
    setupEnableBtn(chorusEnableBtn);
    setupHSlider(chorusRateSlider,  chorusRateLabel,  "RATE");
    setupHSlider(chorusDepthSlider, chorusDepthLabel, "DEPTH");
    setupHSlider(chorusMixSlider,   chorusMixLabel,   "MIX");

    // Delay
    setupEnableBtn(delayEnableBtn);
    setupHSlider(delayTimeSlider,     delayTimeLabel,     "TIME");
    setupHSlider(delayFeedbackSlider, delayFeedbackLabel, "FDBK");
    setupHSlider(delayMixSlider,      delayMixLabel,      "MIX");

    // Keyboard
    keyboardComp.setAvailableRange(36, 96);
    keyboardComp.setKeyPressBaseOctave(4);
    keyboardComp.setColour(juce::MidiKeyboardComponent::whiteNoteColourId,         juce::Colours::white);
    keyboardComp.setColour(juce::MidiKeyboardComponent::blackNoteColourId,         juce::Colour(0xff001166));
    keyboardComp.setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId,  juce::Colour(0xff0033aa));
    keyboardComp.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, juce::Colour(0x80ffd700));
    keyboardComp.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId,    juce::Colour(0xffffd700));
    addAndMakeVisible(keyboardComp);

    setWantsKeyboardFocus(true);
    setSize(960, 888);
    startTimerHz(30);
}

StereoWidenerAudioProcessorEditor::~StereoWidenerAudioProcessorEditor()
{
    stopTimer();
    for (auto* s : { &masterVolSlider,
                     &attackSlider, &decaySlider, &sustainSlider, &releaseSlider,
                     &detuneSlider, &bpmSlider,
                     &noiseLevelSlider, &filterCutoffSlider, &filterResSlider,
                     &filterEnvAttackSlider, &filterEnvAmountSlider,
                     &filterLfoRateSlider, &filterLfoAmountSlider,
                     &ssawLevelSlider, &ssawDetuneSlider, &ssawSpreadSlider, &ssawOctaveSlider,
                     &widthAmountSlider,
                     &chorusRateSlider, &chorusDepthSlider, &chorusMixSlider,
                     &delayTimeSlider, &delayFeedbackSlider, &delayMixSlider })
        s->setLookAndFeel(nullptr);
    osc1.levelSlider.setLookAndFeel(nullptr);
    osc1.octaveSlider.setLookAndFeel(nullptr);
    osc2.levelSlider.setLookAndFeel(nullptr);
    osc2.octaveSlider.setLookAndFeel(nullptr);
    for (auto* b : { &arpToggle, &holdToggle, &div4Btn, &div8Btn,
                     &div16Btn, &div32Btn, &numberwangToggle,
                     &filterLfoSyncBtn,
                     &lfoDiv16Btn, &lfoDiv8Btn, &lfoDiv4Btn, &lfoDiv2Btn, &lfoDiv1Btn,
                     &ssawEnableBtn, &widthEnableBtn,
                     &chorusEnableBtn, &delayEnableBtn,
                     &panicBtn })
        b->setLookAndFeel(nullptr);
}

void StereoWidenerAudioProcessorEditor::setupOscSection(OscSection& osc,
                                                         const juce::String& name,
                                                         const juce::String& waveParamId,
                                                         const juce::String& levelParamId,
                                                         const juce::String& octaveParamId,
                                                         int radioGroupId)
{
    const auto gold = juce::Colour(0xffffd700);
    osc.label.setText(name, juce::dontSendNotification);
    osc.label.setFont(numberwangLAF.bebasFont.withHeight(16.0f));
    osc.label.setColour(juce::Label::textColourId, gold);
    osc.label.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(osc.label);

    for (auto* btn : { &osc.sinBtn, &osc.sqrBtn, &osc.triBtn })
    {
        btn->setLookAndFeel(&numberwangLAF);
        btn->setRadioGroupId(radioGroupId);
        btn->setClickingTogglesState(true);
        addAndMakeVisible(btn);
    }

    auto setWave = [this, waveParamId](float v) {
        audioProcessor.apvts.getParameter(waveParamId)->setValueNotifyingHost(v);
    };
    osc.sinBtn.onClick = [setWave] { setWave(0.0f); };
    osc.sqrBtn.onClick = [setWave] { setWave(0.5f); };
    osc.triBtn.onClick = [setWave] { setWave(1.0f); };

    osc.levelSlider.setLookAndFeel(&numberwangLAF);
    osc.levelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    osc.levelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 22);
    addAndMakeVisible(osc.levelSlider);

    osc.levelAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, levelParamId, osc.levelSlider);

    // Octave slider
    osc.octaveSlider.setLookAndFeel(&numberwangLAF);
    osc.octaveSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    osc.octaveSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 32, 20);
    addAndMakeVisible(osc.octaveSlider);

    osc.octaveLabel.setText("OCT", juce::dontSendNotification);
    osc.octaveLabel.setFont(numberwangLAF.bebasFont.withHeight(14.0f));
    osc.octaveLabel.setColour(juce::Label::textColourId, gold.withAlpha(0.7f));
    osc.octaveLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(osc.octaveLabel);

    osc.octaveAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, octaveParamId, osc.octaveSlider);
}

void StereoWidenerAudioProcessorEditor::updateWaveButtons(OscSection& osc,
                                                          const juce::String& waveParamId)
{
    int idx = (int)audioProcessor.apvts.getRawParameterValue(waveParamId)->load();
    osc.sinBtn.setToggleState(idx == 0, juce::dontSendNotification);
    osc.sqrBtn.setToggleState(idx == 1, juce::dontSendNotification);
    osc.triBtn.setToggleState(idx == 2, juce::dontSendNotification);
}

void StereoWidenerAudioProcessorEditor::updateDivButtons()
{
    int idx = (int)audioProcessor.apvts.getRawParameterValue("arp_division")->load();
    div4Btn .setToggleState(idx == 0, juce::dontSendNotification);
    div8Btn .setToggleState(idx == 1, juce::dontSendNotification);
    div16Btn.setToggleState(idx == 2, juce::dontSendNotification);
    div32Btn.setToggleState(idx == 3, juce::dontSendNotification);
}

void StereoWidenerAudioProcessorEditor::updateLfoDivButtons()
{
    int idx = (int)audioProcessor.apvts.getRawParameterValue("filter_lfo_div")->load();
    lfoDiv16Btn.setToggleState(idx == 0, juce::dontSendNotification);
    lfoDiv8Btn .setToggleState(idx == 1, juce::dontSendNotification);
    lfoDiv4Btn .setToggleState(idx == 2, juce::dontSendNotification);
    lfoDiv2Btn .setToggleState(idx == 3, juce::dontSendNotification);
    lfoDiv1Btn .setToggleState(idx == 4, juce::dontSendNotification);
}

void StereoWidenerAudioProcessorEditor::timerCallback()
{
    if (audioProcessor.numberwangFlag.exchange(false))
        numberwangFlashFrames = 16;   // 15 visible frames at 30 Hz ≈ 500 ms
    if (numberwangFlashFrames > 0) --numberwangFlashFrames;

    updateWaveButtons(osc1, "osc1_wave");
    updateWaveButtons(osc2, "osc2_wave");
    updateDivButtons();
    updateLfoDivButtons();
    repaint();
}

bool StereoWidenerAudioProcessorEditor::keyPressed(const juce::KeyPress& k)
{
    if (k.getTextCharacter() == 'z')
    {
        keyboardOctave = juce::jmax(0, keyboardOctave - 1);
        keyboardComp.setKeyPressBaseOctave(keyboardOctave);
        return true;
    }
    if (k.getTextCharacter() == 'x')
    {
        keyboardOctave = juce::jmin(8, keyboardOctave + 1);
        keyboardComp.setKeyPressBaseOctave(keyboardOctave);
        return true;
    }
    if (k == juce::KeyPress::spaceKey)
    {
        auto* p = audioProcessor.apvts.getParameter("arp_hold");
        p->setValueNotifyingHost(p->getValue() > 0.5f ? 0.0f : 1.0f);
        return true;
    }
    return keyboardComp.keyPressed(k);
}

bool StereoWidenerAudioProcessorEditor::keyStateChanged(bool isKeyDown)
{
    return keyboardComp.keyStateChanged(isKeyDown);
}

void StereoWidenerAudioProcessorEditor::focusLost(FocusChangeType)
{
    audioProcessor.keyboardState.reset();
}

// ─── paint ───────────────────────────────────────────────────────────────────

void StereoWidenerAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto gold      = juce::Colour(0xffffd700);
    const auto royalBlue = juce::Colour(0xff0033aa);
    const auto darkBlue  = juce::Colour(0xff001166);
    auto bebas = [&](float sz) { return numberwangLAF.bebasFont.withHeight(sz); };

    g.fillAll(royalBlue);

    // Banner
    g.setColour(darkBlue);
    g.fillRect(0, 0, getWidth(), 80);
    g.setColour(gold);
    g.fillRect(0, 80, getWidth(), 3);

    g.setColour(gold);
    g.setFont(bebas(46.0f));
    g.drawFittedText("NUMBERWANG!", 0, 4, getWidth(), 52, juce::Justification::centred, 1);

    g.setColour(juce::Colours::white);
    g.setFont(bebas(18.0f));
    g.drawFittedText("SYNTH", 0, 55, getWidth(), 22, juce::Justification::centred, 1);

    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(bebas(16.0f));
    g.drawText(juce::String("v") + JucePlugin_VersionString,
               getWidth() - 80, 60, 72, 18, juce::Justification::centredRight);

    // Scoreboard — fixed-width boxes in left column
    const int scoreY = 83, scoreH = 88;
    const int boxW = 172;
    const int rn = audioProcessor.randomNumber.load();
    const int nn = audioProcessor.lastNoteNumber.load();

    auto drawBox = [&](int bx, const juce::String& hdr, int val, const juce::String& sub = {})
    {
        auto box = juce::Rectangle<int>(bx, scoreY + 6, boxW, scoreH - 12);
        g.setColour(darkBlue); g.fillRect(box);
        g.setColour(gold);     g.drawRect(box, 2);
        g.setFont(bebas(15.0f));
        g.drawFittedText(hdr, box.getX(), box.getY() + 4, box.getWidth(), 18,
                         juce::Justification::centred, 1);
        g.setFont(bebas(42.0f));
        g.drawFittedText(val > 0 ? juce::String(val) : "-",
                         box.getX(), box.getY() + 22, box.getWidth(),
                         box.getHeight() - (sub.isEmpty() ? 28 : 46),
                         juce::Justification::centred, 1);
        if (sub.isNotEmpty()) {
            g.setColour(gold.withAlpha(0.55f));
            g.setFont(bebas(13.0f));
            g.drawFittedText(sub, box.getX(), box.getBottom() - 18, box.getWidth(), 16,
                             juce::Justification::centred, 1);
            g.setColour(gold);
        }
    };

    drawBox(14, "ROLL", rn);
    drawBox(14 + boxW + 10, "NOTE", nn,
            nn > 0 ? juce::MidiMessage::getMidiNoteName(nn, true, true, 4) : "");

    g.setColour(gold);
    g.fillRect(0, scoreY + scoreH, getWidth(), 3);

    // ── Left column dividers & labels ─────────────────────────────────────────
    const int leftW = 480;
    const auto divCol = gold.withAlpha(0.2f);
    g.setColour(divCol);
    g.fillRect(0, 174, leftW, 1);   // after osc header
    g.fillRect(0, 316, leftW, 1);   // after arp
    g.fillRect(0, 374, leftW, 1);   // before noise/filter
    g.fillRect(0, 506, leftW, 1);   // before filter mod
    g.fillRect(0, 620, leftW, 1);   // before ADSR
    g.setColour(gold.withAlpha(0.08f));
    g.fillRect(leftW / 2, 177, 1, 134);  // osc pair separator
    g.fillRect(leftW / 2, 420, 1, 80);   // filter knob separator

    g.setColour(gold.withAlpha(0.4f));
    g.setFont(bebas(12.0f));
    g.drawText("OSCILLATORS",    0, 178, leftW, 14, juce::Justification::centred);
    g.drawText("ARPEGGIATOR",    0, 320, leftW, 14, juce::Justification::centred);
    g.drawText("NOISE & FILTER", 0, 376, leftW, 14, juce::Justification::centred);
    g.drawText("FILTER MOD",     0, 508, leftW, 14, juce::Justification::centred);
    g.drawText("ENVELOPE",       0, 622, leftW, 14, juce::Justification::centred);

    // Master volume section label in scoreboard right area
    g.setColour(gold.withAlpha(0.4f));
    g.setFont(bebas(11.0f));
    g.drawText("MASTER VOLUME", 488, 84, getWidth() - 488 - 8, 14,
               juce::Justification::centred);

    // ── Column separator ──────────────────────────────────────────────────────
    g.setColour(gold.withAlpha(0.35f));
    g.fillRect(482, 174, 2, 662);   // vertical divider between columns

    // ── Right column dividers & labels ────────────────────────────────────────
    const int rx = 486;
    const int rw = getWidth() - rx;
    g.setColour(divCol);
    g.fillRect(rx, 174, rw, 1);   // top border
    g.fillRect(rx, 318, rw, 1);   // below super saw
    g.fillRect(rx, 392, rw, 1);   // below width
    g.fillRect(rx, 530, rw, 1);   // below chorus

    g.setColour(gold.withAlpha(0.4f));
    g.setFont(bebas(12.0f));
    g.drawText("SUPER SAW",    rx, 176, rw, 14, juce::Justification::centred);
    g.drawText("STEREO WIDTH", rx, 320, rw, 14, juce::Justification::centred);
    g.drawText("CHORUS",       rx, 394, rw, 14, juce::Justification::centred);
    g.drawText("DELAY",        rx, 532, rw, 14, juce::Justification::centred);

    // Bottom strip
    g.setColour(darkBlue);
    g.fillRect(0, getHeight() - 50, getWidth(), 50);
    g.setColour(gold);
    g.fillRect(0, getHeight() - 50, getWidth(), 3);
    g.setColour(gold);
    g.setFont(bebas(20.0f));
    g.drawFittedText("That's Numberwang!",
                     0, getHeight() - 46, getWidth(), 44,
                     juce::Justification::centred, 1);
}

void StereoWidenerAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    if (numberwangFlashFrames <= 0) return;

    // t: 0.0 at animation start → 1.0 at end (15 visible frames)
    const float t = 1.0f - (float)numberwangFlashFrames / 15.0f;

    const float W  = (float)getWidth();
    const float H  = (float)getHeight();
    const float cx = W * 0.5f;
    const float cy = H * 0.5f;

    // ── Animating gradient (non-blocking — max 35% opacity) ──────────────────
    // Alpha envelope: fade in → hold → fade out
    float gradAlpha;
    if      (t < 0.25f) gradAlpha = t / 0.25f;
    else if (t < 0.70f) gradAlpha = 1.0f;
    else                gradAlpha = (1.0f - t) / 0.30f;
    gradAlpha *= 0.35f;

    // Three hues sweep through the spectrum as t advances
    const float h1 = std::fmod(t * 1.2f,        1.0f);
    const float h2 = std::fmod(h1 + 0.33f,      1.0f);
    const float h3 = std::fmod(h1 + 0.66f,      1.0f);

    juce::ColourGradient grad(
        juce::Colour::fromHSV(h1, 1.0f, 1.0f, gradAlpha), 0.0f, 0.0f,
        juce::Colour::fromHSV(h3, 1.0f, 1.0f, gradAlpha), W,    H,
        false);
    grad.addColour(0.5, juce::Colour::fromHSV(h2, 1.0f, 1.0f, gradAlpha));
    g.setGradientFill(grad);
    g.fillAll();

    // ── Text: spin in + grow, then fade out ───────────────────────────────────
    float textAlpha, scale, rotation;

    if (t < 0.65f)
    {
        // Spin-in phase — ease-out cubic
        const float p      = t / 0.65f;
        const float eased  = 1.0f - std::pow(1.0f - p, 3.0f);
        scale     = 0.05f + eased * 1.45f;                          // 0.05 → 1.5
        rotation  = (1.0f - eased) * juce::MathConstants<float>::twoPi;  // full spin → 0
        textAlpha = eased;
    }
    else
    {
        // Fade-out phase
        const float p = (t - 0.65f) / 0.35f;
        scale     = 1.5f - p * 0.5f;    // 1.5 → 1.0
        rotation  = 0.0f;
        textAlpha = 1.0f - p;
    }

    g.saveState();
    g.addTransform(
        juce::AffineTransform::translation(-cx, -cy)
            .scaled (scale, scale)
            .rotated(rotation)
            .translated(cx, cy)
    );
    g.setColour(juce::Colours::white.withAlpha(juce::jlimit(0.0f, 1.0f, textAlpha)));
    g.setFont(numberwangLAF.bebasFont.withHeight(76.0f));
    g.drawFittedText("NUMBERWANG!", 0, (int)cy - 60, (int)W, 120,
                     juce::Justification::centred, 1);
    g.restoreState();
}

// ─── resized ─────────────────────────────────────────────────────────────────

void StereoWidenerAudioProcessorEditor::resized()
{
    const int mg    = 12;
    const int W     = getWidth();    // 960
    const int leftW = 480;           // left column width
    const int rx    = 488;           // right column start x
    const int rw    = W - rx - mg;   // right column usable width
    const int rmg   = 12;

    // ── Master volume knob (scoreboard, right column) ─────────────────────────
    // TextBoxAbove(16) + knob(54) = 70px total; fits within scoreboard (y=83–174)
    const int volCentreX  = (rx + W) / 2;
    masterVolLabel .setBounds(volCentreX - 40, 86,  80, 14);
    masterVolSlider.setBounds(volCentreX - 34, 100, 68, 70);

    // ── Numberwang toggle + Panic (scoreboard, left column) ────────────────────
    numberwangToggle.setBounds(leftW - mg - 120, 147, 120, 24);
    panicBtn        .setBounds(leftW - mg - 120 - 8 - 72, 147, 72, 24);

    // ── Osc sections (left column) ────────────────────────────────────────────
    const int oscTop = 194;
    const int halfW  = leftW / 2;

    auto layoutOsc = [&](OscSection& osc, int ox, bool hasDetune)
    {
        const int avail = halfW - mg * 2;

        // Row 1: label + 3 wave buttons (buttons fill remaining width)
        osc.label.setBounds(ox + mg, oscTop, 44, 20);
        const int bx      = ox + mg + 48;
        const int btnW    = (ox + halfW - mg - bx - 8) / 3;  // 8 = two 4px gaps
        osc.sinBtn.setBounds(bx,              oscTop, btnW, 26);
        osc.sqrBtn.setBounds(bx + btnW + 4,   oscTop, btnW, 26);
        osc.triBtn.setBounds(bx + (btnW+4)*2, oscTop, btnW, 26);

        // Row 2: level fader (full available width)
        osc.levelSlider.setBounds(ox + mg, oscTop + 30, avail, 22);

        // Row 3: OCT label + octave slider
        osc.octaveLabel .setBounds(ox + mg,      oscTop + 56, 28, 20);
        osc.octaveSlider.setBounds(ox + mg + 32, oscTop + 57, avail - 32, 20);

        if (hasDetune)
        {
            detuneLabel .setBounds(ox + mg,      oscTop + 80, 52, 20);
            detuneSlider.setBounds(ox + mg + 56, oscTop + 82, avail - 56, 22);
        }
    };

    layoutOsc(osc1, 0,     true);
    layoutOsc(osc2, halfW, false);

    // ── Arp (left column) ─────────────────────────────────────────────────────
    const int arpY = 336, arpH = 32;

    arpToggle .setBounds(mg,       arpY, 66, arpH);
    holdToggle.setBounds(mg + 72,  arpY, 66, arpH);

    const int divBtnW = 44;
    const int divX    = mg + 72 + 72;
    div4Btn .setBounds(divX,                     arpY, divBtnW, arpH);
    div8Btn .setBounds(divX + divBtnW + 4,       arpY, divBtnW, arpH);
    div16Btn.setBounds(divX + (divBtnW+4) * 2,   arpY, divBtnW, arpH);
    div32Btn.setBounds(divX + (divBtnW+4) * 3,   arpY, divBtnW, arpH);

    const int bpmLblW = 34;
    const int bpmX    = divX + (divBtnW+4) * 4 + 6;
    bpmLabel .setBounds(bpmX,                arpY+6, bpmLblW, 20);
    bpmSlider.setBounds(bpmX + bpmLblW + 4,  arpY+4,
                        leftW - mg - bpmX - bpmLblW - 4, arpH - 6);

    // ── Noise & Filter (left column) ──────────────────────────────────────────
    const int nfLblW = 50;
    const int noiseY = 388;
    noiseLevelLabel .setBounds(mg,               noiseY,   nfLblW, 20);
    noiseLevelSlider.setBounds(mg + nfLblW + 4, noiseY+1,
                               leftW - mg*2 - nfLblW - 4, 22);

    // Filter knobs — side-by-side rotary, label(14) + knob+textbox(66) = 80px
    const int filterY  = 420;
    const int leftHalf = leftW / 2;
    filterCutoffLabel .setBounds(0,          filterY,      leftHalf, 14);
    filterCutoffSlider.setBounds(mg,          filterY + 14, leftHalf - mg * 2, 66);
    filterResLabel .setBounds(leftHalf,       filterY,      leftHalf, 14);
    filterResSlider.setBounds(leftHalf + mg,  filterY + 14, leftHalf - mg * 2, 66);

    // ── Filter Mod (left column, y=518–618) ───────────────────────────────────
    const int fLblW   = 54;
    const int halfLW  = leftW / 2;

    // ENV ATK (left half) | ENV AMT (right half), same row
    const int envY = 518;
    filterEnvAttackLabel .setBounds(mg,                  envY,   fLblW, 20);
    filterEnvAttackSlider.setBounds(mg + fLblW + 4,      envY+1, halfLW - mg - fLblW - 4 - 6, 22);
    filterEnvAmountLabel .setBounds(halfLW + mg,         envY,   fLblW, 20);
    filterEnvAmountSlider.setBounds(halfLW + mg + fLblW + 4, envY+1, halfLW - mg - fLblW - 4 - mg, 22);

    // LFO SYNC + 5 division buttons
    const int lfoSyncY = 544;
    filterLfoSyncBtn.setBounds(mg, lfoSyncY, 46, 20);
    {
        const int divStartX = mg + 46 + 4;
        const int divTotal  = leftW - mg - divStartX;
        const int ldBtnW    = (divTotal - 4 * 4) / 5;
        lfoDiv16Btn.setBounds(divStartX + 0 * (ldBtnW + 4), lfoSyncY, ldBtnW, 20);
        lfoDiv8Btn .setBounds(divStartX + 1 * (ldBtnW + 4), lfoSyncY, ldBtnW, 20);
        lfoDiv4Btn .setBounds(divStartX + 2 * (ldBtnW + 4), lfoSyncY, ldBtnW, 20);
        lfoDiv2Btn .setBounds(divStartX + 3 * (ldBtnW + 4), lfoSyncY, ldBtnW, 20);
        lfoDiv1Btn .setBounds(divStartX + 4 * (ldBtnW + 4), lfoSyncY, ldBtnW, 20);
    }

    // LFO RATE and AMT sliders (full-width)
    const int lfoRateY = 568;
    filterLfoRateLabel  .setBounds(mg,             lfoRateY,   fLblW, 20);
    filterLfoRateSlider .setBounds(mg + fLblW + 4, lfoRateY+1, leftW - mg*2 - fLblW - 4, 22);

    const int lfoAmtY = 594;
    filterLfoAmountLabel  .setBounds(mg,             lfoAmtY,   fLblW, 20);
    filterLfoAmountSlider .setBounds(mg + fLblW + 4, lfoAmtY+1, leftW - mg*2 - fLblW - 4, 22);

    // ── ADSR knobs (left column) ──────────────────────────────────────────────
    const int adsrTop = 636;
    const int knobW   = (leftW - mg * 2) / 4;

    juce::Slider* sliders[] = { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider };
    juce::Label*  labels[]  = { &attackLabel,  &decayLabel,  &sustainLabel,  &releaseLabel  };
    for (int i = 0; i < 4; ++i)
    {
        const int bx = mg + i * knobW;
        labels [i]->setBounds(bx, adsrTop,      knobW, 22);
        sliders[i]->setBounds(bx, adsrTop + 22, knobW, 178);
    }

    // ── Piano keyboard (full width) ────────────────────────────────────────────
    keyboardComp.setBounds(0, 840, W, 48);

    // ══════════════════════════════════════════════════════════════════════════
    // RIGHT COLUMN
    // ══════════════════════════════════════════════════════════════════════════

    auto layoutRSlider = [&](juce::Slider& s, juce::Label& lbl, int y)
    {
        lbl.setBounds(rx + rmg,            y,   56, 20);
        s  .setBounds(rx + rmg + 60, y + 1, rw - rmg - 60, 22);
    };

    // ── Super Saw (y=174–318) ──────────────────────────────────────────────────
    ssawEnableBtn.setBounds(rx + rmg, 192, 80, 24);
    layoutRSlider(ssawOctaveSlider, ssawOctaveLabel, 218);
    layoutRSlider(ssawLevelSlider,  ssawLevelLabel,  242);
    layoutRSlider(ssawDetuneSlider, ssawDetuneLabel, 266);
    layoutRSlider(ssawSpreadSlider, ssawSpreadLabel, 290);

    // ── Stereo Width (y=322–390) ───────────────────────────────────────────────
    widthEnableBtn.setBounds(rx + rmg, 338, 80, 26);
    layoutRSlider(widthAmountSlider, widthAmountLabel, 368);

    // ── Chorus (y=394–528) ────────────────────────────────────────────────────
    chorusEnableBtn.setBounds(rx + rmg, 410, 80, 26);
    layoutRSlider(chorusRateSlider,  chorusRateLabel,  442);
    layoutRSlider(chorusDepthSlider, chorusDepthLabel, 468);
    layoutRSlider(chorusMixSlider,   chorusMixLabel,   494);

    // ── Delay (y=532–666) ─────────────────────────────────────────────────────
    delayEnableBtn.setBounds(rx + rmg, 548, 80, 26);
    layoutRSlider(delayTimeSlider,     delayTimeLabel,     580);
    layoutRSlider(delayFeedbackSlider, delayFeedbackLabel, 606);
    layoutRSlider(delayMixSlider,      delayMixLabel,      632);
}
