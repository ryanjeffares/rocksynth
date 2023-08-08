#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

#include <cstdint>

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
    
    void prepare(uint32_t sampleRate) noexcept;
    [[nodiscard]] float getNextSample();

    void setFrequency(float frequency) noexcept;
    void setPulseWidth(float pulseWidth) noexcept;
    void setShape(Shape shape) noexcept;

private:
    float m_sampleRate;
    float m_phase{0.0f};
    float m_frequency{1.0f};
    float m_pulseWidth{0.5f};
    Shape m_shape{Shape::Sine};
};

#endif
