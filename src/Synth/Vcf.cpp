#include "Vcf.hpp"

Vcf::Vcf(size_t numChannels) : m_filters{numChannels}
{

}

void Vcf::prepare(uint32_t sampleRate)
{
    for (auto& filter : m_filters) {
        filter.prepare(sampleRate);
    }
}

void Vcf::process(AudioBuffer& bufferToFill)
{
    for (size_t sample = 0; sample < bufferToFill.bufferSize(); sample++) {
        for (size_t channel = 0; channel < bufferToFill.numChannels(); channel++) {
            auto input = bufferToFill.getSample(channel, sample);
            auto result = m_filters[channel].getNextSample(input);
            bufferToFill.setSample(channel, sample, result);
        }
    }
}

void Vcf::setCutoffFrequency(float cutoffFrequency)
{
    for (auto& filter : m_filters) {
        filter.setCutoffFrequency(cutoffFrequency);
    }
}

void Vcf::setQ(float q)
{
    for (auto& filter : m_filters) {
        filter.setQ(q);
    }
}

