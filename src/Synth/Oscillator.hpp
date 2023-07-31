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
    
    Oscillator(uint32_t sampleRate);

    float getNextSample(Shape shape, float frequency, float pulseWidth = 0.5);

private:
    uint32_t m_SampleRate;
    float m_Phase{0.0f};
};

#endif
