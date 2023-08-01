#include "SynthVoice.hpp"

void SynthVoice::prepare(uint32_t sampleRate)
{
    m_oscillator.prepare(sampleRate);
}

void SynthVoice::process(AudioBuffer& bufferToFill)
{
    for (size_t sample = 0; sample < bufferToFill.bufferSize(); sample++) {
        auto value = m_oscillator.getNextSample();

        for (size_t channel = 0; channel < bufferToFill.numChannels(); channel++) {
            bufferToFill.addSample(channel, sample, value);
        }
    }
}

void SynthVoice::setFrequency(float frequency)
{
    m_oscillator.setFrequency(frequency);
}

void SynthVoice::setPulseWidth(float pulseWidth)
{
    m_oscillator.setPulseWidth(pulseWidth);
}

void SynthVoice::setShape(Oscillator::Shape shape)
{
    m_oscillator.setShape(shape);
}

