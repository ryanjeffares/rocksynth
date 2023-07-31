#include "Oscillator.hpp"

Oscillator::Oscillator(uint32_t sampleRate)
    : m_SampleRate{sampleRate}
{

}

float Oscillator::getNextSample(Shape shape, float frequency, float pulseWidth)
{
    switch (shape) {
        case Shape::Sine: {
            if (m_Phase >= 1.0f) {
                m_Phase = 0.0f;
            }

            auto output = std::sin(m_Phase * std::numbers::pi_v<float> * 2.0f);
            m_Phase += 1.0f / (m_SampleRate / frequency);
            return output;
            break;
        }
        case Shape::Saw: {
            if (m_Phase >= 1.0f) {
                m_Phase = -1.0f;
            }

            auto output = m_Phase;
            m_Phase += 1.0f / (m_SampleRate / frequency) * 2.0f;
            return output;
        }
        case Shape::Pulse: {
            if (m_Phase >= 1.0f) {
                m_Phase = 0.0f;
            }

            auto output = m_Phase <= pulseWidth ? 1.0f : -1.0f;
            m_Phase += 1.0f / (m_SampleRate / frequency);
            return output;
        }
        case Shape::Triangle: {
            if (m_Phase >= 1.0f) {
                m_Phase = 0.0f;
            }

            auto output = m_Phase <= 0.5f ? (m_Phase - 0.25f) * 4.0f : ((1.0f - m_Phase) - 0.25f) * 4.0f;
            m_Phase += 1.0f / (m_SampleRate / frequency);
            return output;
        }
    }
}

