#include "BiquadFilter.hpp"

#include <cmath>
#include <numbers>

void BiquadFilter::prepare(uint32_t sampleRate)
{
    m_sampleRate = static_cast<float>(sampleRate);
    calculateCoefficients();
}

float BiquadFilter::getNextSample(float inputSample)
{
    auto result = m_b0*inputSample + m_b1*m_xz1 + m_b2*m_xz2 - m_a1*m_yz1 - m_a2*m_yz2;
    result /= m_a0;

    m_xz2 = m_xz1;
    m_xz1 = inputSample;
    m_yz2 = m_yz1;
    m_yz1 = result;

    return result;
}

void BiquadFilter::setCutoffFrequency(float cutoffFrequency)
{
    m_cutoffFrequency = cutoffFrequency;
    calculateCoefficients();
}

void BiquadFilter::setQ(float q)
{
    m_q = q;
    calculateCoefficients();
}

void BiquadFilter::calculateCoefficients()
{
    auto w = 2.0f * std::numbers::pi_v<float> * (m_cutoffFrequency / m_sampleRate);
    auto a = std::sin(w) / (2.0f * m_q);
    auto cosw = std::cos(w);

    m_a0 = 1.0f + a;
    m_a1 = -2.0f * cosw;
    m_a2 = 1.0f - a;

    m_b0 = (1.0f - cosw) / 2.0f;
    m_b1 = 1.0f - cosw;
    m_b2 = (1.0f - cosw) / 2.0f;
}

