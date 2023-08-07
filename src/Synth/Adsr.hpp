#ifndef ADSR_HPP
#define ADSR_HPP

#include <cstdint>

class Adsr
{
public:
    enum class Phase
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release,
    };

    void prepare(uint32_t sampleRate);
    float getNextValue();

    void noteOn();
    void noteOff();

    template<Phase ParamType> requires (ParamType != Phase::Idle)
    void setParam(float value)
    {
        if constexpr (ParamType == Phase::Attack) {
            m_attackTime = value;
        } else if constexpr (ParamType == Phase::Decay) {
            m_decayTime = value;
        } else if constexpr (ParamType == Phase::Sustain) {
            m_sustainLevel = value;
        } else if constexpr (ParamType == Phase::Release) {
            m_releaseTime = value;
        }
    }

private:
    Phase m_state{Phase::Idle};
    
    float m_sampleRate;
    float m_sampleCounter{0.0f};

    float m_attackTime{0.0f}, m_decayTime{0.0f}, m_releaseTime{0.0f};
    float m_sustainLevel{1.0f};
};

#endif
