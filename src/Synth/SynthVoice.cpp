#include "SynthVoice.hpp"

#include <cmath>

static float mtof(uint8_t midiNote)
{
    return std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f) * 440.0f;
}

SynthVoice::SynthVoice(size_t numChannels) : m_vcf{numChannels}
{

}

void SynthVoice::prepare(uint32_t sampleRate)
{
    m_adsr.prepare(sampleRate);
    m_vcf.prepare(sampleRate);
    for (auto& osc : m_oscillators) {
        osc.prepare(sampleRate);
    }
}

void SynthVoice::process(AudioBuffer& bufferToFill)
{
    auto numChannels = bufferToFill.numChannels();
    auto bufferSize = bufferToFill.bufferSize();
    AudioBuffer mix{numChannels, bufferSize};

    for (size_t sample = 0; sample < bufferSize; sample++) {
        auto adsrLevel = m_adsr.getNextValue();
        auto oscOutput = 0.0f;
        for (size_t i = 0; i < 2; i++) {
            oscOutput += m_oscillators[i].getNextSample() * adsrLevel * m_currentVelocity * m_oscillatorVolumes[i];
        }

        for (size_t channel = 0; channel < numChannels; channel++) {
            mix.addSample(channel, sample, oscOutput);
        }
    }

    m_vcf.process(mix);

    for (size_t sample = 0; sample < bufferSize; sample++) {
        for (size_t channel = 0; channel < numChannels; channel++) {
            bufferToFill.addSample(channel, sample, mix.getSample(channel, sample));
        }
    }
}

void SynthVoice::noteOn(uint8_t midiNote, uint8_t velocity) noexcept
{
    m_currentNote = midiNote;
    m_currentVelocity = static_cast<float>(velocity) / 127.0f;
    m_isNotePlaying = true;

    for (auto& osc : m_oscillators) {
        osc.setFrequency(mtof(midiNote));
    }
    
    m_adsr.noteOn();
    m_vcf.noteOn();
}

void SynthVoice::noteOff() noexcept
{
    m_isNotePlaying = false;
    m_adsr.noteOff();
    m_vcf.noteOff();
}

void SynthVoice::setCutoffFrequency(float cutoffFrequency)
{
    m_vcf.setCutoffFrequency(cutoffFrequency);
}

void SynthVoice::setQ(float q)
{
    m_vcf.setQ(q);
}

uint8_t SynthVoice::getCurrentNote() const noexcept
{
    return m_currentNote;
}

bool SynthVoice::getIsNotePlaying() const noexcept
{
    return m_isNotePlaying;
}
