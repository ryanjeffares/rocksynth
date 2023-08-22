#include "Adsr.hpp"

#include <cmath>

void Adsr::prepare(uint32_t sampleRate) noexcept
{
    m_sampleRate = static_cast<float>(sampleRate);
}

float Adsr::getNextValue() noexcept
{
    switch (m_state) {
        case Phase::Attack: {
            // time in seconds the attack phase has been active
            auto t = m_sampleCounter++ / m_sampleRate;
            // get this as a proportion between 0 - 1
            // this will be the maximum level reached during the attack phase
            m_maxLevel = t / m_attackTime;

            if (m_maxLevel >= 1.0f) {
                // if we reached max amplitude of 1, the attack phase is over
                // move to decay on the next call
                m_state = Phase::Decay;
                m_sampleCounter = 0;
            }

            return m_maxLevel;
        }
        case Phase::Decay: {
            // time in seconds the decay phase has been active
            auto t = m_sampleCounter++ / m_sampleRate;
            // invert the decay level since volume should be decreasing
            auto decayLevel = 1.0f - t / m_decayTime;
            // add this to the sustain level, but scaled within the headroom above sustain level
            // treat this as the new maximum level reached in case release is triggered early
            m_maxLevel = m_sustainLevel + decayLevel * (1.0f - m_sustainLevel);
            
            if (decayLevel <= 0.0f) {
                // decay phase done, move to sustain on the next call
                m_state = Phase::Sustain;
                m_sampleCounter = 0;
            }

            return m_maxLevel;
        }
        case Phase::Sustain:
            return m_sustainLevel;
        case Phase::Release: {
            // time in seconds the release phase has been active
            auto t = m_sampleCounter++ / m_sampleRate;
            // invert release level since volume is decreasing
            auto level = 1.0f - t / m_releaseTime;

            if (level <= 0.0f) {
                // release phase done, go back to idle
                m_state = Phase::Idle;
            }

            // scale the maximum level reached in case of early note off
            return level * m_maxLevel;
        }
        case Phase::Idle:
        default:
            return 0.0f;
    }
}

void Adsr::noteOn() noexcept
{
    // reset everything when a new note is activated
    m_sampleCounter = 0;
    m_state = Phase::Attack;
}

void Adsr::noteOff() noexcept
{
    // don't do anything if the whole process has already finished
    if (m_state != Phase::Release && m_state != Phase::Idle) {
        m_sampleCounter = 0;
        m_state = Phase::Release;
    }
}

Adsr::Phase Adsr::getCurrentPhase() const noexcept
{
    return m_state;
}
