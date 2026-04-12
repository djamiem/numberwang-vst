#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout
NumberwangAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const juce::StringArray waveChoices { "Sine", "Square", "Triangle" };

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "osc1_wave",  1 }, "Osc 1 Wave", waveChoices, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "osc1_level", 1 }, "Osc 1 Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "osc1_detune", 1 }, "Osc 1 Detune",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("ct")));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "osc2_wave",  1 }, "Osc 2 Wave", waveChoices, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "osc2_level", 1 }, "Osc 2 Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "attack",  1 }, "Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.4f), 0.05f,
        juce::AudioParameterFloatAttributes().withLabel("s")));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "decay",   1 }, "Decay",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.4f), 0.1f,
        juce::AudioParameterFloatAttributes().withLabel("s")));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "sustain", 1 }, "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "release", 1 }, "Release",
        juce::NormalisableRange<float>(0.001f, 4.0f, 0.001f, 0.4f), 0.3f,
        juce::AudioParameterFloatAttributes().withLabel("s")));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "numberwang", 1 }, "Numberwang", true));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "arp_enabled", 1 }, "Arp Enabled", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "arp_hold",    1 }, "Arp Hold",    false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "arp_bpm",     1 }, "Arp BPM",
        juce::NormalisableRange<float>(40.0f, 240.0f, 0.5f), 120.0f,
        juce::AudioParameterFloatAttributes().withLabel("bpm")));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "arp_division", 1 }, "Arp Division",
        juce::StringArray{ "1/4", "1/8", "1/16", "1/32" }, 1)); // default 1/8

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "noise_level", 1 }, "Noise Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "filter_cutoff", 1 }, "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.25f), 20000.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "filter_res", 1 }, "Filter Resonance",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    // Master volume (0–2 = 0–200%, default 1.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "master_volume", 1 }, "Master Volume",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

    // Filter Envelope
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "filter_env_attack", 1 }, "Filter Env Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.1f,
        juce::AudioParameterFloatAttributes().withLabel("s")));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "filter_env_amount", 1 }, "Filter Env Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    // Filter LFO
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "filter_lfo_rate", 1 }, "Filter LFO Rate",
        juce::NormalisableRange<float>(0.1f, 20.0f, 0.01f, 0.4f), 1.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "filter_lfo_amount", 1 }, "Filter LFO Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "filter_lfo_sync", 1 }, "Filter LFO Sync", false));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "filter_lfo_div", 1 }, "Filter LFO Division",
        juce::StringArray{ "1/16", "1/8", "1/4", "1/2", "1 BAR" }, 2));  // default 1/4

    // Oscillator octave shifts (-3 to +3 semitone-octaves)
    for (const auto& id : { "osc1_octave", "osc2_octave", "ssaw_octave" })
        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID{ id, 1 }, id, -3, 3, 0));

    // Super Saw
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "ssaw_enabled", 1 }, "Super Saw On", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "ssaw_level",  1 }, "Super Saw Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "ssaw_detune", 1 }, "Super Saw Detune",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "ssaw_spread", 1 }, "Super Saw Spread",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    // Stereo Width
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "width_enabled", 1 }, "Width On", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "width_amount",  1 }, "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

    // Chorus
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "chorus_enabled", 1 }, "Chorus On", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "chorus_rate",  1 }, "Chorus Rate",
        juce::NormalisableRange<float>(0.1f, 5.0f, 0.01f), 1.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "chorus_depth", 1 }, "Chorus Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.4f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "chorus_mix",   1 }, "Chorus Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.4f));

    // Delay
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "delay_enabled",  1 }, "Delay On", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "delay_time",     1 }, "Delay Time",
        juce::NormalisableRange<float>(0.05f, 1.0f, 0.001f), 0.375f,
        juce::AudioParameterFloatAttributes().withLabel("s")));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "delay_feedback", 1 }, "Delay Feedback",
        juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f), 0.4f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "delay_mix",      1 }, "Delay Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));

    return layout;
}

NumberwangAudioProcessor::NumberwangAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    for (int i = 0; i < numVoices; ++i)
        synth.addVoice(new NWVoice());
    synth.addSound(new NWSound());
}

NumberwangAudioProcessor::~NumberwangAudioProcessor() {}

void NumberwangAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    keyboardState.reset();
    cachedSampleRate = sampleRate;

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<NWVoice*>(synth.getVoice(i)))
            v->prepareVoice(sampleRate);

    std::fill(std::begin(delayBufL),  std::end(delayBufL),  0.0f);
    std::fill(std::begin(delayBufR),  std::end(delayBufR),  0.0f);
    std::fill(std::begin(chorusBufL), std::end(chorusBufL), 0.0f);
    std::fill(std::begin(chorusBufR), std::end(chorusBufR), 0.0f);
    delayWritePos   = 0;
    chorusWritePos  = 0;
    chorusLfoPhaseL = 0.0;
    chorusLfoPhaseR = 0.5;

    numberwangFifthNote      = -1;
    samplesUntilFifthRelease = 0.0;
    samplesUntilNWAllowed    = 0.0;
    nwChaos        = {};
    filterLfoPhase = 0.0;
    heldNotes.clear();
    arpIndex               = 0;
    arpGoingUp             = true;
    samplesUntilNextArpStep = 0.0;
    currentArpNote         = -1;
    prevHoldEnabled        = false;

    juce::ignoreUnused(samplesPerBlock);
}

void NumberwangAudioProcessor::releaseResources()
{
    keyboardState.reset();
    heldNotes.clear();
    currentArpNote = -1;
}

void NumberwangAudioProcessor::advanceArpIndex()
{
    const int n = heldNotes.size();
    if (n <= 1) { arpIndex = 0; return; }

    if (arpGoingUp)
    {
        ++arpIndex;
        if (arpIndex >= n)
        {
            arpGoingUp = false;
            arpIndex   = n - 2; // bounce back from top without repeating
            if (arpIndex < 0) arpIndex = 0;
        }
    }
    else
    {
        --arpIndex;
        if (arpIndex < 0)
        {
            arpGoingUp = true;
            arpIndex   = 1; // bounce forward from bottom without repeating
            if (arpIndex >= n) arpIndex = 0;
        }
    }
}

void NumberwangAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    // Panic: kill all voices and clear state
    if (panicRequested.exchange(false))
    {
        synth.allNotesOff(0, true);
        keyboardState.reset();
        heldNotes.clear();
        currentArpNote = -1;
        std::fill(std::begin(delayBufL), std::end(delayBufL), 0.0f);
        std::fill(std::begin(delayBufR), std::end(delayBufR), 0.0f);
        buffer.clear();
        return;
    }

    const bool arpEnabled  = apvts.getRawParameterValue("arp_enabled")->load() > 0.5f;
    const bool holdEnabled = apvts.getRawParameterValue("arp_hold")   ->load() > 0.5f;

    // Capture any note that needs releasing before the arp swap can discard it
    int releaseNote = -1;

    if (prevHoldEnabled && !holdEnabled)
    {
        heldNotes.clear();
        if (currentArpNote >= 0)
        {
            releaseNote    = currentArpNote;
            currentArpNote = -1;
        }
    }
    if (!arpEnabled && currentArpNote >= 0)
    {
        releaseNote    = currentArpNote;
        currentArpNote = -1;
    }
    prevHoldEnabled = holdEnabled;

    // Update held-note list from incoming MIDI
    for (const auto meta : midiMessages)
    {
        const auto msg = meta.getMessage();
        if (msg.isNoteOn())
        {
            if (!heldNotes.contains(msg.getNoteNumber()))
            {
                heldNotes.add(msg.getNoteNumber());
                heldNotes.sort();
            }
            heldNoteVels[msg.getNoteNumber()] = (juce::uint8)msg.getVelocity();
        }
        else if (msg.isNoteOff() && !holdEnabled)
        {
            heldNotes.removeFirstMatchingValue(msg.getNoteNumber());
        }
    }

    // Arpeggiator: replace midiMessages with arp-generated note events
    if (arpEnabled)
    {
        const double bpm      = apvts.getRawParameterValue("arp_bpm")->load();
        const int    divIdx   = (int)apvts.getRawParameterValue("arp_division")->load();
        const double divMults[] = { 1.0, 2.0, 4.0, 8.0 }; // 1/4, 1/8, 1/16, 1/32
        const double stepsPerBeat = divMults[divIdx];
        const double samplesPerStep = getSampleRate() * 60.0 / bpm / stepsPerBeat;

        juce::MidiBuffer arpMidi;
        int pos = 0;

        while (pos < buffer.getNumSamples())
        {
            if (samplesUntilNextArpStep <= 0.0)
            {
                // Note off for the previous arp note
                if (currentArpNote >= 0)
                {
                    arpMidi.addEvent(juce::MidiMessage::noteOff(1, currentArpNote), pos);
                    currentArpNote = -1;
                }

                if (!heldNotes.isEmpty())
                {
                    if (arpIndex >= heldNotes.size()) arpIndex = 0;
                    currentArpNote = heldNotes[arpIndex];
                    const auto vel = heldNoteVels[currentArpNote];
                    arpMidi.addEvent(
                        juce::MidiMessage::noteOn(1, currentArpNote, vel > 0 ? vel : (juce::uint8)100),
                        pos);
                    advanceArpIndex();
                }

                samplesUntilNextArpStep += samplesPerStep;
            }

            const int advance = juce::jmax(1,
                juce::jmin(buffer.getNumSamples() - pos,
                           (int)samplesUntilNextArpStep));
            pos                     += advance;
            samplesUntilNextArpStep -= advance;
        }

        midiMessages.swapWith(arpMidi);
    }

    // Inject any pending note-off AFTER the arp swap so it isn't discarded
    if (releaseNote >= 0)
        midiMessages.addEvent(juce::MidiMessage::noteOff(1, releaseNote), 0);

    // Cooldown tick
    samplesUntilNWAllowed -= buffer.getNumSamples();

    // Chaos countdown tick
    const bool wasChaosActive = nwChaos.active;
    if (nwChaos.active)
    {
        nwChaos.samplesRemaining -= buffer.getNumSamples();
        if (nwChaos.samplesRemaining <= 0.0)
            nwChaos.active = false;
    }
    const bool chaos = nwChaos.active;

    // Chaos just ended this block — reset filter state and clear FX buffers to avoid pops
    if (wasChaosActive && !chaos)
    {
        for (int i = 0; i < synth.getNumVoices(); ++i)
            if (auto* v = dynamic_cast<NWVoice*>(synth.getVoice(i)))
                v->resetFilters();
        std::fill(std::begin(delayBufL),  std::end(delayBufL),  0.0f);
        std::fill(std::begin(delayBufR),  std::end(delayBufR),  0.0f);
        std::fill(std::begin(chorusBufL), std::end(chorusBufL), 0.0f);
        std::fill(std::begin(chorusBufR), std::end(chorusBufR), 0.0f);
    }

    // Numberwang: scan final MIDI for note-ons
    // Collect any new MIDI events in a separate buffer — never modify midiMessages mid-iteration
    juce::MidiBuffer extraEvents;
    const bool nwEnabled = apvts.getRawParameterValue("numberwang")->load() > 0.5f;

    for (const auto meta : midiMessages)
    {
        const auto msg = meta.getMessage();
        if (msg.isNoteOn())
        {
            const int note = msg.getNoteNumber();
            const int nn   = (note % 10 == 0) ? 10 : note % 10;
            const int rn   = rng.nextInt(juce::Range<int>(1, 11));
            lastNoteNumber.store(nn);
            randomNumber.store(rn);

            if (nn == rn && nwEnabled && samplesUntilNWAllowed <= 0.0)
            {
                numberwangFlag.store(true);
                samplesUntilNWAllowed = getSampleRate() * nwCooldownSeconds;

                // Randomize all FX for 2 beats (half a measure)
                const double bpm = apvts.getRawParameterValue("arp_bpm")->load();
                nwChaos.active           = true;
                nwChaos.samplesRemaining = getSampleRate() * 2.0 * 60.0 / bpm;
                nwChaos.chorusOn    = true;
                nwChaos.delayOn     = true;
                nwChaos.widthOn     = true;
                nwChaos.ssawOn      = rng.nextFloat() > 0.4f;
                nwChaos.chorusRate  = 0.5f + rng.nextFloat() * 4.0f;
                nwChaos.chorusDepth = 0.4f + rng.nextFloat() * 0.6f;
                nwChaos.chorusMix   = 0.5f + rng.nextFloat() * 0.5f;
                nwChaos.delayTime   = 0.1f + rng.nextFloat() * 0.4f;
                nwChaos.delayFeedback = 0.4f + rng.nextFloat() * 0.5f;
                nwChaos.delayMix    = 0.4f + rng.nextFloat() * 0.6f;
                nwChaos.widthAmount = 1.2f + rng.nextFloat() * 0.8f;
                nwChaos.ssawLevel   = 0.6f + rng.nextFloat() * 0.4f;
                nwChaos.ssawDetune  = 0.3f + rng.nextFloat() * 0.6f;
                nwChaos.ssawSpread  = 0.5f + rng.nextFloat() * 0.5f;
                nwChaos.filterCutoff = 200.0f + rng.nextFloat() * 2800.0f;
                nwChaos.filterRes    = 0.4f + rng.nextFloat() * 0.5f;
                nwChaos.osc1Octave   = rng.nextInt(5) - 2;   // -2 to +2
                nwChaos.osc2Octave   = rng.nextInt(5) - 2;
                nwChaos.ssawOctave   = rng.nextInt(5) - 2;

                // Inject a perfect 5th (7 semitones) above the highest held note
                if (!heldNotes.isEmpty())
                {
                    const int fifthNote = heldNotes.getLast() + 7;
                    if (fifthNote <= 127)
                    {
                        if (numberwangFifthNote >= 0)
                            extraEvents.addEvent(
                                juce::MidiMessage::noteOff(1, numberwangFifthNote),
                                meta.samplePosition);

                        extraEvents.addEvent(
                            juce::MidiMessage::noteOn(1, fifthNote, (juce::uint8)100),
                            meta.samplePosition);

                        numberwangFifthNote      = fifthNote;
                        samplesUntilFifthRelease = getSampleRate() * 0.75;
                    }
                }
            }
        }
    }

    // Release the Numberwang 5th note after its duration
    if (samplesUntilFifthRelease > 0.0)
    {
        samplesUntilFifthRelease -= buffer.getNumSamples();
        if (samplesUntilFifthRelease <= 0.0 && numberwangFifthNote >= 0)
        {
            extraEvents.addEvent(
                juce::MidiMessage::noteOff(1, numberwangFifthNote),
                juce::jmax(0, buffer.getNumSamples() - 1));
            numberwangFifthNote = -1;
        }
    }

    // Merge extra events into the main buffer now that iteration is complete
    for (const auto meta : extraEvents)
        midiMessages.addEvent(meta.getMessage(), meta.samplePosition);

    // Push params to every voice
    const auto   osc1Wave   = static_cast<NWVoice::WaveType>(
                                  (int)apvts.getRawParameterValue("osc1_wave")->load());
    const float  osc1Level  = apvts.getRawParameterValue("osc1_level") ->load();
    const float  osc1Detune = apvts.getRawParameterValue("osc1_detune")->load();
    const auto   osc2Wave   = static_cast<NWVoice::WaveType>(
                                  (int)apvts.getRawParameterValue("osc2_wave")->load());
    const float  osc2Level    = apvts.getRawParameterValue("osc2_level")    ->load();
    const float  noiseLevel   = apvts.getRawParameterValue("noise_level")   ->load();
    const float  filterCutoff = chaos ? nwChaos.filterCutoff
                                      : apvts.getRawParameterValue("filter_cutoff")->load();
    const float  filterRes    = chaos ? nwChaos.filterRes
                                      : apvts.getRawParameterValue("filter_res")   ->load();

    const bool   ssawEnabled  = chaos ? nwChaos.ssawOn
                                      : (apvts.getRawParameterValue("ssaw_enabled")->load() > 0.5f);
    const float  ssawLevel    = chaos ? nwChaos.ssawLevel
                                      : apvts.getRawParameterValue("ssaw_level")   ->load();
    const float  ssawDetune   = chaos ? nwChaos.ssawDetune
                                      : apvts.getRawParameterValue("ssaw_detune")  ->load();
    const float  ssawSpread   = chaos ? nwChaos.ssawSpread
                                      : apvts.getRawParameterValue("ssaw_spread")  ->load();

    const int osc1Octave = chaos ? nwChaos.osc1Octave
                                 : (int)std::round(apvts.getRawParameterValue("osc1_octave")->load());
    const int osc2Octave = chaos ? nwChaos.osc2Octave
                                 : (int)std::round(apvts.getRawParameterValue("osc2_octave")->load());
    const int ssawOctave = chaos ? nwChaos.ssawOctave
                                 : (int)std::round(apvts.getRawParameterValue("ssaw_octave")->load());

    const juce::ADSR::Parameters adsrParams{
        apvts.getRawParameterValue("attack") ->load(),
        apvts.getRawParameterValue("decay")  ->load(),
        apvts.getRawParameterValue("sustain")->load(),
        apvts.getRawParameterValue("release")->load()
    };

    // Filter envelope params (suppressed during chaos)
    const float fEnvAttack = chaos ? 0.001f : apvts.getRawParameterValue("filter_env_attack")->load();
    const float fEnvAmount = chaos ? 0.0f   : apvts.getRawParameterValue("filter_env_amount")->load();

    // Filter LFO (suppressed during chaos)
    float lfoContrib = 0.0f;
    if (!chaos)
    {
        const float fLfoAmount = apvts.getRawParameterValue("filter_lfo_amount")->load();
        if (fLfoAmount > 0.0f)
        {
            const bool   lfoSync  = apvts.getRawParameterValue("filter_lfo_sync")->load() > 0.5f;
            const int    lfoDivIdx = (int)apvts.getRawParameterValue("filter_lfo_div")->load();
            const double bpm      = apvts.getRawParameterValue("arp_bpm")->load();

            // Beats per cycle for each division: 1/16, 1/8, 1/4, 1/2, 1 BAR
            static const double divBeats[] = { 0.25, 0.5, 1.0, 2.0, 4.0 };
            const double lfoHz = lfoSync
                ? (bpm / 60.0) / divBeats[lfoDivIdx]
                : (double)apvts.getRawParameterValue("filter_lfo_rate")->load();

            filterLfoPhase += lfoHz / getSampleRate() * buffer.getNumSamples();
            while (filterLfoPhase >= 1.0) filterLfoPhase -= 1.0;

            const float lfoValue = (float)std::sin(
                filterLfoPhase * juce::MathConstants<double>::twoPi);
            lfoContrib = lfoValue * fLfoAmount * 8000.0f;  // ±8 kHz range
        }
    }

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* v = dynamic_cast<NWVoice*>(synth.getVoice(i)))
        {
            v->setOsc1Params(osc1Wave, osc1Level, osc1Detune, osc1Octave);
            v->setOsc2Params(osc2Wave, osc2Level, osc2Octave);
            v->setNoiseLevel(noiseLevel);
            v->setFilterParams(filterCutoff, filterRes);
            v->setFilterEnvParams(fEnvAttack, fEnvAmount);
            v->setFilterLfoContrib(lfoContrib);
            v->setSuperSawParams(ssawEnabled, ssawLevel, ssawDetune, ssawSpread, ssawOctave);
            v->setADSRParameters(adsrParams);
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // ── Post-processing FX ────────────────────────────────────────────────────
    {
        const bool on = chaos ? nwChaos.chorusOn
                               : (apvts.getRawParameterValue("chorus_enabled")->load() > 0.5f);
        if (on)
            applyChorus(buffer,
                chaos ? nwChaos.chorusRate  : apvts.getRawParameterValue("chorus_rate") ->load(),
                chaos ? nwChaos.chorusDepth : apvts.getRawParameterValue("chorus_depth")->load(),
                chaos ? nwChaos.chorusMix   : apvts.getRawParameterValue("chorus_mix")  ->load());
    }
    {
        const bool on = chaos ? nwChaos.delayOn
                               : (apvts.getRawParameterValue("delay_enabled")->load() > 0.5f);
        if (on)
            applyDelay(buffer,
                chaos ? nwChaos.delayTime     : apvts.getRawParameterValue("delay_time")    ->load(),
                chaos ? nwChaos.delayFeedback : apvts.getRawParameterValue("delay_feedback")->load(),
                chaos ? nwChaos.delayMix      : apvts.getRawParameterValue("delay_mix")     ->load());
    }
    {
        const bool on = chaos ? nwChaos.widthOn
                               : (apvts.getRawParameterValue("width_enabled")->load() > 0.5f);
        if (on)
            applyWidener(buffer,
                chaos ? nwChaos.widthAmount : apvts.getRawParameterValue("width_amount")->load());
    }

    buffer.applyGain(apvts.getRawParameterValue("master_volume")->load());
}

void NumberwangAudioProcessor::applyWidener(juce::AudioBuffer<float>& buffer,
                                                float widthAmount)
{
    if (buffer.getNumChannels() < 2) return;
    float* L = buffer.getWritePointer(0);
    float* R = buffer.getWritePointer(1);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        const float mid  = (L[i] + R[i]) * 0.5f;
        const float side = (L[i] - R[i]) * 0.5f * widthAmount;
        L[i] = mid + side;
        R[i] = mid - side;
    }
}

void NumberwangAudioProcessor::applyChorus(juce::AudioBuffer<float>& buffer,
                                               float rate, float depth, float mix)
{
    if (buffer.getNumChannels() < 2) return;
    float* L = buffer.getWritePointer(0);
    float* R = buffer.getWritePointer(1);
    const int    N             = buffer.getNumSamples();
    const double lfoInc        = (double)rate / cachedSampleRate;
    const float  baseSamples   = (float)(0.007 * cachedSampleRate); // 7 ms centre
    const float  depthSamples  = (float)(0.010 * cachedSampleRate) * depth; // ±10 ms

    auto readInterp = [&](const float* buf, int writePos, float delay) -> float
    {
        const int   id   = (int)delay;
        const float frac = delay - (float)id;
        const int   r0   = (writePos - id     + maxChorusSamples) % maxChorusSamples;
        const int   r1   = (writePos - id - 1 + maxChorusSamples) % maxChorusSamples;
        return buf[r0] * (1.0f - frac) + buf[r1] * frac;
    };

    for (int i = 0; i < N; ++i)
    {
        const float lfoL = (float)std::sin(chorusLfoPhaseL * juce::MathConstants<double>::twoPi);
        const float lfoR = (float)std::sin(chorusLfoPhaseR * juce::MathConstants<double>::twoPi);

        chorusBufL[chorusWritePos] = L[i];
        chorusBufR[chorusWritePos] = R[i];

        const float delayL = juce::jmax(1.0f, baseSamples + lfoL * depthSamples);
        const float delayR = juce::jmax(1.0f, baseSamples + lfoR * depthSamples);

        L[i] = L[i] * (1.0f - mix) + readInterp(chorusBufL, chorusWritePos, delayL) * mix;
        R[i] = R[i] * (1.0f - mix) + readInterp(chorusBufR, chorusWritePos, delayR) * mix;

        chorusWritePos = (chorusWritePos + 1) % maxChorusSamples;
        chorusLfoPhaseL += lfoInc; if (chorusLfoPhaseL >= 1.0) chorusLfoPhaseL -= 1.0;
        chorusLfoPhaseR += lfoInc; if (chorusLfoPhaseR >= 1.0) chorusLfoPhaseR -= 1.0;
    }
}

void NumberwangAudioProcessor::applyDelay(juce::AudioBuffer<float>& buffer,
                                              float timeSecs, float feedback, float mix)
{
    if (buffer.getNumChannels() < 2) return;
    float* L = buffer.getWritePointer(0);
    float* R = buffer.getWritePointer(1);
    const int N           = buffer.getNumSamples();
    const int delaySamples = juce::jlimit(1, maxDelaySamples - 1,
                                          (int)(timeSecs * cachedSampleRate));

    for (int i = 0; i < N; ++i)
    {
        const int readPos = (delayWritePos - delaySamples + maxDelaySamples) % maxDelaySamples;
        const float dL    = delayBufL[readPos];
        const float dR    = delayBufR[readPos];

        // Ping-pong: L echoes appear on R and vice versa
        delayBufL[delayWritePos] = L[i] + dR * feedback;
        delayBufR[delayWritePos] = R[i] + dL * feedback;

        L[i] = L[i] * (1.0f - mix) + dL * mix;
        R[i] = R[i] * (1.0f - mix) + dR * mix;

        delayWritePos = (delayWritePos + 1) % maxDelaySamples;
    }
}

void NumberwangAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    auto xml   = state.createXml();
    copyXmlToBinary(*xml, destData);
}

void NumberwangAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary(data, sizeInBytes);
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* NumberwangAudioProcessor::createEditor()
{
    return new NumberwangAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NumberwangAudioProcessor();
}
