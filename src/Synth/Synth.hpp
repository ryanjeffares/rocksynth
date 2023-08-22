#ifndef SYNTH_HPP
#define SYNTH_HPP

#include "../Audio/AudioProcessor.hpp"
#include "../Audio/Chorus.hpp"
#include "SynthVoice.hpp"

class Synth : public AudioProcessor
{
public:
    Synth(size_t numChannels);
    ~Synth() override = default;

    void prepare(uint32_t sampleRate) override;
    void process(AudioBuffer& bufferToFill) override;

    void noteOn(uint8_t midiNote, uint8_t velocity) noexcept;
    void noteOff(uint8_t midiNote) noexcept;

    template<size_t OscNumber> requires (OscNumber < 2)
    void setPulseWidth(float pulseWidth) noexcept
    {
        for (auto& voice : m_voices) {
            voice.setPulseWidth<OscNumber>(pulseWidth);
        }
    }

    template<size_t OscNumber> requires (OscNumber < 2)
    void setShape(Oscillator::Shape shape) noexcept
    {
        for (auto& voice : m_voices) {
            voice.setShape<OscNumber>(shape);
        }
    }

    template<size_t OscNumber> requires (OscNumber < 2)
    void setVolume(float volume) noexcept
    {
        for (auto& voice : m_voices) {
            voice.setVolume<OscNumber>(volume);
        }
    }

    template<Adsr::Phase ParamType> requires (ParamType != Adsr::Phase::Idle)
    void setAdsrParam(float value) noexcept
    {
        for (auto& voice : m_voices) {
            voice.setAdsrParam<ParamType>(value);
        }
    }

    template<Adsr::Phase ParamType> requires (ParamType != Adsr::Phase::Idle)
    void setVcfAdsrParam(float value) noexcept
    {
        for (auto& voice : m_voices) {
            voice.setVcfAdsrParam<ParamType>(value);
        }
    }

    void setCutoffFrequency(float cutoffFrequency);
    void setQ(float q);

    template<Chorus::Param ParamType>
    void setChorusParam(float value)
    {
        m_chorus.setParam<ParamType>(value);
    }

private:
    size_t m_lastVoiceIndex{0};
    std::array<SynthVoice, 8> m_voices;
    Chorus m_chorus;
};

#endif
