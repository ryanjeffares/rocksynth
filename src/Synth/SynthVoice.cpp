#include "SynthVoice.hpp"

#include <cmath>
#include <numeric>

static float mtof(uint8_t midiNote)
{
    return std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f) * 440.0f;
}

void SynthVoice::prepare(uint32_t sampleRate)
{
    m_adsr.prepare(sampleRate);
    for (auto& osc : m_oscillators) {
        osc.prepare(sampleRate);
    }
}

void SynthVoice::process(AudioBuffer& bufferToFill)
{
    for (size_t sample = 0; sample < bufferToFill.bufferSize(); sample++) {
        auto adsrLevel = m_adsr.getNextValue();
        auto oscOutput = std::accumulate(
                m_oscillators.begin(),
                m_oscillators.end(),
                0.0f,
                [&] (float value, Oscillator& osc) {
                    return value + osc.getNextSample() * adsrLevel * m_currentVelocity;
                }
            );

        for (size_t channel = 0; channel < bufferToFill.numChannels(); channel++) {
            bufferToFill.addSample(channel, sample, oscOutput);
        }
    }
}

void SynthVoice::noteOn(uint8_t midiNote, uint8_t velocity) noexcept
{
    m_currentNote = midiNote;
    m_currentVelocity = static_cast<float>(velocity) / 127.0f;

    for (auto& osc : m_oscillators) {
        osc.setFrequency(mtof(midiNote));
    }
    m_adsr.noteOn();
}

void SynthVoice::noteOff() noexcept
{
    m_adsr.noteOff();
}

uint8_t SynthVoice::getCurrentNote() const noexcept
{
    return m_currentNote;
}

