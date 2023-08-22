#include "Chorus.hpp"

static float map(float value, float inMin, float inMax, float outMin, float outMax)
{
    auto inFraction = (value - inMin) / (inMax - inMin);
    return ((outMax - outMin) * inFraction) + outMin;
}

Chorus::Chorus(size_t numChannels)
    : m_delayLines(numChannels)
    , m_heldSamples(numChannels)
{

}

void Chorus::prepare(uint32_t sampleRate)
{
    for (auto& delayLine : m_delayLines) {
        delayLine.prepare(sampleRate);
        delayLine.setMaxDelaySamples(sampleRate);
        delayLine.setDelaySeconds(0.025f);
    }

    m_lfo.prepare(sampleRate);
    m_lfo.setShape(Oscillator::Shape::Sine);
    m_lfo.setFrequency(0.5f);

    m_smoothedLfoValue.prepare(sampleRate / m_lfoUpdateRate);
    m_smoothedLfoValue.setSmoothingTime(0.05f);
    m_smoothedLfoValue.setCurrentAndTargetValue(0.0f);
}

void Chorus::process(AudioBuffer& bufferToFill)
{
    for (size_t sample = 0; sample < bufferToFill.bufferSize(); sample++) {
        if (m_lfoUpdateCounter == 0) {
            auto lfoValue = m_lfo.getNextSample() * m_depth;
            m_smoothedLfoValue.setTargetValue(lfoValue);
            m_lfoUpdateCounter = m_lfoUpdateRate;
        }
        
        m_lfoUpdateCounter--;
        auto smoothedLfoValue = m_smoothedLfoValue.getNextValue();

        for (size_t channel = 0; channel < bufferToFill.numChannels(); channel++) {
            auto inputSample = bufferToFill.getSample(channel, sample);
            m_delayLines[channel].setDelaySeconds(map(smoothedLfoValue, -1.0f, 1.0f, 0.005f, 0.04f));
            auto delayedSample = m_delayLines[channel].getNextSample(inputSample + m_heldSamples[channel] * m_feedback);
            bufferToFill.setSample(channel, sample, inputSample * (1.0f - m_dryWet) + delayedSample * m_dryWet);
            m_heldSamples[channel] = delayedSample;
        }
    }
}
