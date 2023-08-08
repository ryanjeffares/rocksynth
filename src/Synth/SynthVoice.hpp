#ifndef SYNTH_VOICE_HPP
#define SYNTH_VOICE_HPP

#include "Adsr.hpp"
#include "../Audio/AudioProcessor.hpp"
#include "Oscillator.hpp"

#include <array>

class SynthVoice : public AudioProcessor
{
public:
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

    template<Adsr::Phase ParamType> requires (ParamType != Adsr::Phase::Idle)
    void setAdsrParam(float value) noexcept
    {
        m_adsr.setParam<ParamType>(value);
    }

    [[nodiscard]] uint8_t getCurrentNote() const noexcept;

private:
    uint8_t m_currentNote{0};
    float m_currentVelocity{0.0f};

    Adsr m_adsr;
    std::array<Oscillator, 2> m_oscillators;
};

#endif
