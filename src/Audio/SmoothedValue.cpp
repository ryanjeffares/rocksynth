#include "SmoothedValue.hpp"

#include <cmath>
#include <limits>

void SmoothedValue::prepare(uint32_t sampleRate) noexcept
{
    m_sampleRate = sampleRate;
}

float SmoothedValue::getNextValue() noexcept
{
    if (m_stepCounter == 0) {
        return m_target;
    }

    m_stepCounter--;
    if (m_stepCounter == 0) {
        m_current = m_target;
    } else {
        m_current += m_stepSize;
    }

    return m_current;
}

void SmoothedValue::setSmoothingTime(float seconds) noexcept
{
    m_stepsToTarget = m_sampleRate * seconds;
    setCurrentAndTargetValue(m_target);
}

void SmoothedValue::setTargetValue(float value) noexcept
{
    if (std::abs(value - m_target) <= std::numeric_limits<float>::epsilon()) {
        return;
    }

    m_target = value;
    if (m_stepsToTarget == 0) {
        m_current = value;
    }

    m_stepCounter = m_stepsToTarget;
    m_stepSize = (m_target - m_current) / static_cast<float>(m_stepsToTarget);
}

void SmoothedValue::setCurrentAndTargetValue(float value) noexcept
{
    m_target = value;
    m_current = value;
    m_stepCounter = 0;
}
