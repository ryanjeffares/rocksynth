#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

#include <cmath>
#include <concepts>
#include <cstdint>
#include <numbers>

class Oscillator
{
public:
    enum class Shape
    {
        Sine,
        Saw,
        Pulse,
        Triangle,
    };
    
    Oscillator(uint32_t sampleRate)
        : m_SampleRate{sampleRate}
    {

    }

    float getNextSample(Shape shape, float frequency, float pulseWidth = 0.5)
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

private:
    uint32_t m_SampleRate;
    float m_Phase{0.0f};
};

#endif
