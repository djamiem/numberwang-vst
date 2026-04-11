#pragma once

#include <JuceHeader.h>
#include "SynthVoice.h"

class StereoWidenerAudioProcessor : public juce::AudioProcessor
{
public:
    StereoWidenerAudioProcessor();
    ~StereoWidenerAudioProcessor() override;

    void prepareToPlay  (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock   (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi()  const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int  getNumPrograms()    override { return 1; }
    int  getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    juce::MidiKeyboardState            keyboardState;

    // UI display — written audio thread, read UI thread
    std::atomic<int>  randomNumber   { 0 };
    std::atomic<int>  lastNoteNumber { 0 };
    std::atomic<bool> numberwangFlag { false };
    std::atomic<bool> panicRequested { false };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void advanceArpIndex();

    juce::Synthesiser synth;
    juce::Random      rng;

    static constexpr int numVoices = 8;

    // Numberwang 5th injection
    int    numberwangFifthNote        = -1;
    double samplesUntilFifthRelease   = 0.0;
    double samplesUntilNWAllowed      = 0.0;   // cooldown between triggers
    static constexpr double nwCooldownSeconds = 5.0;

    // Arpeggiator state
    juce::Array<int>   heldNotes;
    juce::uint8        heldNoteVels[128] {};
    int                arpIndex              = 0;
    bool               arpGoingUp            = true;
    double             samplesUntilNextArpStep = 0.0;
    int                currentArpNote        = -1;
    bool               prevHoldEnabled       = false;

    // ── Numberwang chaos: temporary FX override ───────────────────────────────
    struct NWChaosState
    {
        bool   active           = false;
        double samplesRemaining = 0.0;
        bool   chorusOn = false, delayOn = false, widthOn = false, ssawOn = false;
        float  chorusRate = 1.0f, chorusDepth = 0.5f, chorusMix   = 0.5f;
        float  delayTime  = 0.25f, delayFeedback = 0.6f, delayMix = 0.5f;
        float  widthAmount = 1.5f;
        float  ssawLevel  = 0.8f, ssawDetune = 0.6f, ssawSpread  = 0.9f;
        float  filterCutoff = 800.0f, filterRes = 0.7f;
        int    osc1Octave = 0, osc2Octave = 0, ssawOctave = 0;
    } nwChaos;

    // ── FX state ──────────────────────────────────────────────────────────────
    double cachedSampleRate  = 44100.0;
    double filterLfoPhase    = 0.0;

    // Delay (ping-pong): up to 1 second at 96 kHz
    static constexpr int maxDelaySamples = 96001;
    float delayBufL[maxDelaySamples] {};
    float delayBufR[maxDelaySamples] {};
    int   delayWritePos = 0;

    // Chorus: ~50 ms buffer at 96 kHz
    static constexpr int maxChorusSamples = 4801;
    float  chorusBufL[maxChorusSamples] {};
    float  chorusBufR[maxChorusSamples] {};
    int    chorusWritePos  = 0;
    double chorusLfoPhaseL = 0.0;
    double chorusLfoPhaseR = 0.5;  // 180° offset for stereo movement

    void applyWidener (juce::AudioBuffer<float>&, float widthAmount);
    void applyChorus  (juce::AudioBuffer<float>&, float rate, float depth, float mix);
    void applyDelay   (juce::AudioBuffer<float>&, float timeSecs, float feedback, float mix);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoWidenerAudioProcessor)
};
