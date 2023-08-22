#ifndef ADSR_HPP
#define ADSR_HPP

#include <cmath>
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

    void prepare(uint32_t sampleRate) noexcept;
    [[nodiscard]] float getNextValue() noexcept;

    void noteOn() noexcept;
    void noteOff() noexcept;

    template<Phase ParamType> requires (ParamType != Phase::Idle)
    void setParam(float value) noexcept
    {
        if constexpr (ParamType == Phase::Attack) {
            m_attackTime = std::fmax(value, 0.001f);
        } else if constexpr (ParamType == Phase::Decay) {
            m_decayTime = std::fmax(value, 0.001f);
        } else if constexpr (ParamType == Phase::Sustain) {
            if (m_state == Phase::Sustain) {
                m_maxLevel = value;
            }
            m_sustainLevel = value;
        } else if constexpr (ParamType == Phase::Release) {
            m_releaseTime = std::fmax(value, 0.001f);
        }
    }

    [[nodiscard]] Phase getCurrentPhase() const noexcept;

private:
    Phase m_state{Phase::Idle};
    
    float m_sampleRate;
    float m_sampleCounter{0.0f};

    float m_attackTime{0.001f}, m_decayTime{0.001f}, m_releaseTime{0.001f};
    float m_sustainLevel{1.0f};

    float m_maxLevel{0.0f};

public:
};

#endif
