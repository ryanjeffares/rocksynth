#ifndef SYNTH_VOICE_HPP
#define SYNTH_VOICE_HPP

#include "Adsr.hpp"
#include "../Audio/AudioProcessor.hpp"
#include "Oscillator.hpp"
#include "Vcf.hpp"

#include <array>

class SynthVoice : public AudioProcessor
{
public:
    SynthVoice(size_t numChannels);
    ~SynthVoice() override = default;

    void prepare(uint32_t sampleRate) override;
    void process(AudioBuffer& bufferToFill) override;

    void noteOn(uint8_t midiNote, uint8_t velocity) noexcept;
    void noteOff() noexcept;

    template<size_t OscNumber> requires (OscNumber < 2)
    void setPulseWidth(float pulseWidth) noexcept
    {
        m_oscillators[OscNumber].setPulseWidth(pulseWidth);
    }

    template<size_t OscNumber> requires (OscNumber < 2)
    void setShape(Oscillator::Shape shape) noexcept
    {
        m_oscillators[OscNumber].setShape(shape);
    }

    template<size_t OscNumber> requires (OscNumber < 2)
    void setVolume(float volume) noexcept
    {
        m_oscillatorVolumes[OscNumber] = volume;
    }

    template<Adsr::Phase ParamType> requires (ParamType != Adsr::Phase::Idle)
    void setAdsrParam(float value) noexcept
    {
        m_adsr.setParam<ParamType>(value);
    }

    template<Adsr::Phase ParamType> requires (ParamType != Adsr::Phase::Idle)
    void setVcfAdsrParam(float value) noexcept
    {
        m_vcf.setAdsrParam<ParamType>(value);
    }

    void setCutoffFrequency(float cutoffFrequency);
    void setQ(float q);

    [[nodiscard]] uint8_t getCurrentNote() const noexcept;
    [[nodiscard]] bool getIsNotePlaying() const noexcept;

private:
    uint8_t m_currentNote{0};
    float m_currentVelocity{0.0f};

    Adsr m_adsr;
    Vcf m_vcf;
    std::array<Oscillator, 2> m_oscillators;
    std::array<float, 2> m_oscillatorVolumes{0.5f, 0.5f};
};

#endif
