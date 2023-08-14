#ifndef VCF_HPP
#define VCF_HPP

#include "Adsr.hpp"
#include "../Audio/AudioProcessor.hpp"
#include "BiquadFilter.hpp"

#include <vector>

class Vcf : public AudioProcessor
{
public:
    Vcf(size_t numChannels);
    ~Vcf() override = default;

    void prepare(uint32_t sampleRate) override;
    void process(AudioBuffer& bufferToFill) override;

    void setCutoffFrequency(float cutoffFrequency);
    void setQ(float q);

    void noteOn();
    void noteOff();

    template<Adsr::Phase ParamType> requires (ParamType != Adsr::Phase::Idle)
    void setAdsrParam(float value)
    {
        m_adsr.setParam<ParamType>(value);
    }

private:
    float m_cutoffFrequency{20000.0f};

    Adsr m_adsr;
    std::vector<BiquadFilter> m_filters;
};

#endif
