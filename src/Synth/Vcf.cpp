#include "Vcf.hpp"

Vcf::Vcf(size_t numChannels) : m_filters{numChannels}
{

}

void Vcf::prepare(uint32_t sampleRate)
{
    for (auto& filter : m_filters) {
        filter.prepare(sampleRate);
    }

    m_adsr.prepare(sampleRate);
}

void Vcf::process(AudioBuffer& bufferToFill)
{
    for (size_t sample = 0; sample < bufferToFill.bufferSize(); sample++) {
        auto adsrLevel = m_adsr.getNextValue();
        auto currentCutoff = std::min(m_cutoffFrequency * adsrLevel, 20.0f);

        for (size_t channel = 0; channel < bufferToFill.numChannels(); channel++) {
            m_filters[channel].setCutoffFrequency(currentCutoff);
            auto input = bufferToFill.getSample(channel, sample);
            auto result = m_filters[channel].getNextSample(input);
            bufferToFill.setSample(channel, sample, result);
        }
    }
}

void Vcf::setCutoffFrequency(float cutoffFrequency)
{
    m_cutoffFrequency = cutoffFrequency;
}

void Vcf::setQ(float q)
{
    for (auto& filter : m_filters) {
        filter.setQ(q);
    }
}

void Vcf::noteOn()
{
    m_adsr.noteOn();
}

void Vcf::noteOff()
{
    m_adsr.noteOff();
}

