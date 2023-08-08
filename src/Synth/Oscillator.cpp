#include "Oscillator.hpp"

#include "fmt/core.h"

#include <cmath>
#include <numbers>

void Oscillator::prepare(uint32_t sampleRate)
{
    m_sampleRate = static_cast<float>(sampleRate);
}

float Oscillator::getNextSample()
{
    if (m_phase >= 1.0f) {
        m_phase = 0.0f;
    }
    m_phase += 1.0f / (m_sampleRate / m_frequency);

    switch (m_shape) {
        case Shape::Sine:
            return std::sin(m_phase * std::numbers::pi_v<float> * 2.0f);
        case Shape::Saw:
            return 2.0f * (m_phase - std::floor(m_phase + 0.5f));
        case Shape::Pulse:
            return m_phase <= m_pulseWidth ? 1.0f : -1.0f;
        case Shape::Triangle:
            return 4.0f * std::abs(m_phase - std::floor(m_phase + 0.75f) + 0.25f) - 1.0f;
        default: {
            fmt::print(stderr, "Invalid shape\n");
            return 0.0f;
        }
    }
}

void Oscillator::setFrequency(float frequency)
{
    m_frequency = frequency;
}

void Oscillator::setPulseWidth(float pulseWidth)
{
    m_pulseWidth = pulseWidth;
}

void Oscillator::setShape(Shape shape)
{
    m_shape = shape;
}

