#ifndef SYNTH_VOICE_HPP
#define SYNTH_VOICE_HPP

#include "../Audio/AudioProcessor.hpp"
#include "Oscillator.hpp"

class SynthVoice : AudioProcessor
{
public:
    ~SynthVoice() override = default;

    void prepare(uint32_t sampleRate) override;
    void process(AudioBuffer& bufferToFill) override;

    void setFrequency(float frequency);
    void setPulseWidth(float pulseWidth);
    void setShape(Oscillator::Shape shape);

private:
    Oscillator m_oscillator;
};

#endif
