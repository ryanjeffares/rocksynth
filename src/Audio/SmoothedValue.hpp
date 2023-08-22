#ifndef SMOOTHED_VALUE_HPP
#define SMOOTHED_VALUE_HPP

#include <cstdint>
#include <cstdlib>

class SmoothedValue
{
public:
    void prepare(uint32_t sampleRate) noexcept;
    float getNextValue() noexcept;

    void setSmoothingTime(float seconds) noexcept;
    void setTargetValue(float value) noexcept;
    void setCurrentAndTargetValue(float value) noexcept;

private:
    float m_sampleRate{};
    float m_current{0.0f};
    float m_target{0.0f};
    float m_stepSize{0.0f};

    size_t m_stepsToTarget{0};
    size_t m_stepCounter{0};
};

#endif
