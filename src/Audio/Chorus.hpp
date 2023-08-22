#ifndef CHORUS_HPP
#define CHORUS_HPP

#include "AudioProcessor.hpp"
#include "DelayLine.hpp"
#include "SmoothedValue.hpp"
#include "../Synth/Oscillator.hpp"

#include <vector>

class Chorus : public AudioProcessor
{
public:
    Chorus(size_t numChannels);
    ~Chorus() override = default;

    void prepare(uint32_t sampleRate) override;
    void process(AudioBuffer& bufferToFill) override;

    enum Param
    {
        Depth,
        Rate,
        Feedback,
        DryWet,
    };

    template<Param ParamType>
    void setParam(float value)
    {
        if constexpr (ParamType == Param::Depth) {
            m_depth = value;
        } else if constexpr (ParamType == Param::Rate) {
            m_lfo.setFrequency(value);
        } else if constexpr (ParamType == Param::Feedback) {
            m_feedback = value;
        } else if constexpr (ParamType == Param::DryWet) {
            m_dryWet = value;
        }
    }

private:
    std::vector<DelayLine> m_delayLines;
    std::vector<float> m_heldSamples;
    Oscillator m_lfo;
    SmoothedValue m_smoothedLfoValue;

    float m_depth{0.5f};
    float m_feedback{0.5f};
    float m_dryWet{0.5f};

    size_t m_lfoUpdateRate{100};
    size_t m_lfoUpdateCounter{0};
};

#endif
