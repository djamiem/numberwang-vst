#pragma once
#include <JuceHeader.h>

struct NWSound : public juce::SynthesiserSound
{
    bool appliesToNote    (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

// ─── Direct-form-I biquad lowpass ─────────────────────────────────────────────
struct BiquadLP
{
    void setLowPass (double sampleRate, float cutoffHz, float q) noexcept
    {
        cutoffHz = juce::jlimit (20.0f, (float)(sampleRate * 0.49), cutoffHz);
        q        = juce::jmax  (0.01f, q);
        const float w0    = juce::MathConstants<float>::twoPi * cutoffHz / (float)sampleRate;
        const float cosW  = std::cos (w0);
        const float alpha = std::sin (w0) / (2.0f * q);
        const float a0inv = 1.0f / (1.0f + alpha);
        b0 = (1.0f - cosW) * 0.5f * a0inv;
        b1 = (1.0f - cosW)        * a0inv;
        b2 = b0;
        a1 = -2.0f * cosW         * a0inv;
        a2 = (1.0f - alpha)       * a0inv;
    }

    void reset() noexcept { x1 = x2 = y1 = y2 = 0.0f; }

    float process (float x) noexcept
    {
        float y = b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2;
        x2 = x1; x1 = x;
        y2 = y1; y1 = y;
        return y;
    }

    float b0=1, b1=0, b2=0, a1=0, a2=0;
    float x1=0, x2=0, y1=0, y2=0;
};

// ─────────────────────────────────────────────────────────────────────────────

class NWVoice : public juce::SynthesiserVoice
{
public:
    enum class WaveType { Sine = 0, Square = 1, Triangle = 2 };

    bool canPlaySound (juce::SynthesiserSound* s) override
    {
        return dynamic_cast<NWSound*>(s) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int) override
    {
        phase1         = 0.0;
        phase2         = 0.0;
        level          = (double)velocity * 0.10;
        phaseDeltaBase = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber)
                         / getSampleRate();

        // Stagger super-saw phases to avoid attack click
        for (int k = 0; k < NUM_SAW; ++k)
            ssawPhase[k] = (double)k / (double)NUM_SAW;

        filterEnvPhase = 0.0f;
        filterL.reset();
        filterR.reset();
        adsr.noteOn();
    }

    void stopNote (float, bool allowTailOff) override
    {
        if (allowTailOff) adsr.noteOff();
        else { clearCurrentNote(); adsr.reset(); }
    }

    void renderNextBlock (juce::AudioBuffer<float>& output,
                          int startSample, int numSamples) override
    {
        if (phaseDeltaBase == 0.0) return;

        // Update filter coefficients once per block: base cutoff + envelope + LFO
        {
            filterEnvPhase = juce::jmin(1.0f, filterEnvPhase
                                              + filterEnvAttackDelta * (float)numSamples);
            const float envContrib = filterEnvAmount * filterEnvPhase
                                     * (20000.0f - storedBaseCutoff);
            const float effectiveCutoff = juce::jlimit(20.0f, 20000.0f,
                storedBaseCutoff + envContrib + filterLfoContrib);
            filterL.setLowPass(storedSampleRate, effectiveCutoff, storedQ);
            filterR.setLowPass(storedSampleRate, effectiveCutoff, storedQ);
        }

        const double osc1Delta = phaseDeltaBase
                                 * std::pow(2.0, (double)osc1Octave + (double)osc1DetuneCents / 1200.0);
        const double osc2Delta = phaseDeltaBase
                                 * std::pow(2.0, (double)osc2Octave);

        // Precompute super-saw per-voice deltas (avoids pow inside per-sample loop)
        // detuneWeights: centre=0, then ±1, ±0.5, ±0.25
        static const float detuneWeights[NUM_SAW] = { 0.0f, -1.0f, 1.0f,
                                                      -0.5f,  0.5f,
                                                      -0.25f, 0.25f };
        double sawDelta[NUM_SAW];
        const double sawBase  = phaseDeltaBase * std::pow(2.0, (double)ssawOctave);
        const float  maxCents = ssawDetune * 50.0f; // ±50 cents max
        for (int k = 0; k < NUM_SAW; ++k)
            sawDelta[k] = sawBase
                          * std::pow(2.0, (double)(detuneWeights[k] * maxCents) / 1200.0);

        for (int i = startSample; i < startSample + numSamples; ++i)
        {
            float env = adsr.getNextSample();

            // ── Oscillators (mono) ────────────────────────────────────────────
            float s1    = renderWave(phase1, osc1Wave) * osc1Level;
            float s2    = renderWave(phase2, osc2Wave) * osc2Level;
            float noise = (noiseRng.nextFloat() * 2.0f - 1.0f) * noiseLevel;
            // Proportional cross-fade: mono oscs pull back as super saw comes up
            const float oscGain = ssawEnabled ? (1.0f - ssawLevel) : 1.0f;
            float monoOsc = (s1 + s2) * 0.5f * oscGain + noise;

            phase1 += osc1Delta; if (phase1 >= 1.0) phase1 -= 1.0;
            phase2 += osc2Delta; if (phase2 >= 1.0) phase2 -= 1.0;

            // ── Super Saw (stereo) ────────────────────────────────────────────
            float ssawL = 0.0f, ssawR = 0.0f;
            if (ssawEnabled)
            {
                for (int k = 0; k < NUM_SAW; ++k)
                {
                    // Sawtooth: phase 0→1 = amplitude +1→-1
                    float saw = 1.0f - 2.0f * (float)ssawPhase[k];

                    // Equal-power panning
                    float pan   = ssawPan[k];
                    float gainL = std::sqrt(juce::jmax(0.0f, 0.5f * (1.0f - pan)));
                    float gainR = std::sqrt(juce::jmax(0.0f, 0.5f * (1.0f + pan)));
                    ssawL += saw * gainL;
                    ssawR += saw * gainR;

                    ssawPhase[k] += sawDelta[k];
                    if (ssawPhase[k] >= 1.0) ssawPhase[k] -= 1.0;
                }
                const float norm = ssawLevel * 0.5f;  // ≈ 1/sqrt(NUM_SAW), balanced with mono osc level
                ssawL *= norm;
                ssawR *= norm;
            }

            // ── Mix + ADSR + filter ───────────────────────────────────────────
            const float envLevel = (float)(level * env);
            float sampleL = filterL.process((monoOsc + ssawL) * envLevel);
            float sampleR = filterR.process((monoOsc + ssawR) * envLevel);

            if (output.getNumChannels() >= 2)
            {
                output.addSample(0, i, sampleL);
                output.addSample(1, i, sampleR);
            }
            else
            {
                output.addSample(0, i, (sampleL + sampleR) * 0.5f);
            }

            if (!adsr.isActive()) { clearCurrentNote(); break; }
        }
    }

    // ── Setters ───────────────────────────────────────────────────────────────

    void setOsc1Params (WaveType wave, float lvl, float detuneCents, int octave)
    {
        osc1Wave        = wave;
        osc1Level       = lvl;
        osc1DetuneCents = detuneCents;
        osc1Octave      = octave;
    }

    void setOsc2Params (WaveType wave, float lvl, int octave)
    {
        osc2Wave   = wave;
        osc2Level  = lvl;
        osc2Octave = octave;
    }

    void setNoiseLevel (float lvl) { noiseLevel = lvl; }

    void setFilterParams (float cutoffHz, float resonance)
    {
        storedBaseCutoff = cutoffHz;
        storedQ          = 0.5f + resonance * 9.5f;
    }

    void setFilterEnvParams (float attackSecs, float amount)
    {
        filterEnvAmount      = amount;
        filterEnvAttackDelta = (storedSampleRate > 0.0 && attackSecs > 0.0f)
                               ? (1.0f / (float)(attackSecs * storedSampleRate))
                               : 1.0f;
    }

    void setFilterLfoContrib (float contribHz)
    {
        filterLfoContrib = contribHz;
    }

    void setSuperSawParams (bool enabled, float lvl, float detune, float spreadAmt, int octave)
    {
        ssawEnabled = enabled;
        ssawLevel   = lvl;
        ssawDetune  = detune;
        ssawOctave  = octave;

        // Symmetric pan spread: centre + 3 pairs at spread * {1, 0.6, 0.25}
        const float s = juce::jlimit(0.0f, 1.0f, spreadAmt);
        ssawPan[0] =  0.0f;
        ssawPan[1] = -s;          ssawPan[2] =  s;
        ssawPan[3] = -s * 0.6f;  ssawPan[4] =  s * 0.6f;
        ssawPan[5] = -s * 0.25f; ssawPan[6] =  s * 0.25f;
    }

    void setADSRParameters (juce::ADSR::Parameters p) { adsr.setParameters(p); }

    void resetFilters() { filterL.reset(); filterR.reset(); }

    void prepareVoice (double sampleRate)
    {
        storedSampleRate = sampleRate;
        adsr.setSampleRate(sampleRate);
        storedBaseCutoff = 20000.0f;
        storedQ          = 0.707f;
        filterL.setLowPass(sampleRate, 20000.0f, 0.707f);
        filterR.setLowPass(sampleRate, 20000.0f, 0.707f);
        filterL.reset();
        filterR.reset();
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

private:
    static float renderWave (double phase, WaveType type)
    {
        switch (type)
        {
            case WaveType::Sine:
                return (float)std::sin(phase * juce::MathConstants<double>::twoPi);
            case WaveType::Square:
                return phase < 0.5 ? 1.0f : -1.0f;
            case WaveType::Triangle:
                return (float)(phase < 0.5 ? (4.0 * phase - 1.0)
                                           : (3.0 - 4.0 * phase));
        }
        return 0.0f;
    }

    static constexpr int NUM_SAW = 7;

    double    phase1 = 0.0, phase2 = 0.0;
    double    phaseDeltaBase   = 0.0;
    double    level            = 0.0;
    double    storedSampleRate = 44100.0;
    float     osc1DetuneCents  = 0.0f;
    int       osc1Octave       = 0;
    WaveType  osc1Wave  = WaveType::Sine;
    WaveType  osc2Wave  = WaveType::Sine;
    float     osc1Level  = 1.0f;
    float     osc2Level  = 0.5f;
    int       osc2Octave = 0;
    float     noiseLevel = 0.0f;

    // Super Saw
    double ssawPhase[NUM_SAW]  = {};
    float  ssawPan[NUM_SAW]    = {};
    bool   ssawEnabled = false;
    float  ssawLevel   = 0.0f;
    float  ssawDetune  = 0.3f;
    int    ssawOctave  = 0;

    // Filter state
    float storedBaseCutoff   = 20000.0f;
    float storedQ            = 0.707f;
    float filterEnvAmount    = 0.0f;
    float filterEnvPhase     = 1.0f;      // 0=fully closed, 1=fully open in envelope terms
    float filterEnvAttackDelta = 0.0f;    // per-sample increment
    float filterLfoContrib   = 0.0f;      // Hz offset from LFO

    juce::Random noiseRng;
    BiquadLP     filterL, filterR;   // stereo filter pair (same coefficients, independent state)
    juce::ADSR   adsr;
};
