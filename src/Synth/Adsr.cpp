#include "Adsr.hpp"

#include <cmath>

void Adsr::prepare(uint32_t sampleRate)
{
    m_sampleRate = static_cast<float>(sampleRate);
}

float Adsr::getNextValue()
{
    switch (m_state) {
        case Phase::Attack: {
            auto t = m_sampleCounter++ / m_sampleRate; // time in seconds we've been active
            auto level = t / m_attackTime; // get this as a proportion between 0 - 1
            if (level >= 1.0f) {
                // attack phase done, move to decay on the next call
                m_state = Phase::Decay;
                m_sampleCounter = 0;
            }
            return level;
        }
        case Phase::Decay: {
            auto t = m_sampleCounter++ / m_sampleRate;
            // invert the decay level since volume should be decreasing
            auto decayLevel = 1.0f - t / m_decayTime;
            // add this to the sustain level, but scaled within the headroom above sustain level
            auto scaledLevel = m_sustainLevel + decayLevel * (1.0f - m_sustainLevel);
            if (decayLevel <= 0.0f) {
                // decay phase done, move to sustain on the next call
                m_state = Phase::Sustain;
                m_sampleCounter = 0;
            }
            return std::max(scaledLevel, m_sustainLevel);
        }
        case Phase::Sustain:
            return m_sustainLevel;
        case Phase::Release: {
            auto t = m_sampleCounter++ / m_sampleRate;
            // invert release level since volume is decreasing
            auto level = 1.0f - t / m_releaseTime;
            if (level <= 0.0f) {
                m_state = Phase::Idle;
            }
            return level * m_sustainLevel;
            break;
        }
        case Phase::Idle:
        default:
            return 0.0f;
    }
}

void Adsr::noteOn()
{
    m_sampleCounter = 0;
    m_state = Phase::Attack;
}

void Adsr::noteOff()
{
    m_sampleCounter = 0;
    m_state = Phase::Release;
}

