#ifndef BIQUAD_FILTER_HPP
#define BIQUAD_FILTER_HPP

#include <cstdint>

class BiquadFilter
{
public:
    void prepare(uint32_t sampleRate);
    float getNextSample(float inputSample);

    void setCutoffFrequency(float cutoffFrequency);
    void setQ(float q);

private:
    void calculateCoefficients();

    float m_a0, m_a1, m_a2;
    float m_b0, m_b1, m_b2;

    float m_xz1{0.0f}, m_xz2{0.0f};
    float m_yz1{0.0f}, m_yz2{0.0f};
    float m_sampleRate;

    float m_cutoffFrequency{20000.0f};
    float m_q{1.0f};

};

#endif
