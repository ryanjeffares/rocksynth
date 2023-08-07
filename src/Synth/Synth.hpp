#ifndef SYNTH_HPP
#define SYNTH_HPP

#include "../Audio/AudioProcessor.hpp"
#include "SynthVoice.hpp"

class Synth : public AudioProcessor
{
public:
    ~Synth() override = default;

    void prepare(uint32_t sampleRate) override;
    void process(AudioBuffer& bufferToFill) override;

    void noteOn(uint8_t midiNote);
    void noteOff(uint8_t midiNote);

    template<size_t OscNumber> requires (OscNumber < 2)
    void setPulseWidth(float pulseWidth)
    {
        for (auto& voice : m_voices) {
            voice.setPulseWidth<OscNumber>(pulseWidth);
        }
    }

    template<size_t OscNumber> requires (OscNumber < 2)
    void setShape(Oscillator::Shape shape)
    {
        for (auto& voice : m_voices) {
            voice.setShape<OscNumber>(shape);
        }
    }

    template<Adsr::Phase ParamType> requires (ParamType != Adsr::Phase::Idle)
    void setAdsrParam(float value)
    {
        for (auto& voice : m_voices) {
            voice.setAdsrParam<ParamType>(value);
        }
    }

private:
    size_t m_lastVoiceIndex{0};
    std::array<SynthVoice, 8> m_voices;
};

#endif
