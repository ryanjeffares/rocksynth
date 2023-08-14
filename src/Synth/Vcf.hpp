#ifndef VCF_HPP
#define VCF_HPP

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

private:
    std::vector<BiquadFilter> m_filters;
};

#endif
