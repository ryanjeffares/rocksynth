#include "Synth.hpp"

Synth::Synth(size_t numChannels)
    : m_voices{
        SynthVoice{numChannels},
        SynthVoice{numChannels},
        SynthVoice{numChannels},
        SynthVoice{numChannels},
        SynthVoice{numChannels},
        SynthVoice{numChannels},
        SynthVoice{numChannels},
        SynthVoice{numChannels},
    }
    , m_chorus{numChannels}
{

}

void Synth::prepare(uint32_t sampleRate)
{
    for (auto& voice : m_voices) {
        voice.prepare(sampleRate);
    }

    m_chorus.prepare(sampleRate);
}

void Synth::process(AudioBuffer& bufferToFill)
{
    for (auto& voice : m_voices) {
        voice.process(bufferToFill);
    }

    m_chorus.process(bufferToFill);
}

void Synth::noteOn(uint8_t midiNote, uint8_t velocity) noexcept
{
    bool voiceFound = false;
    for (size_t i = 0; i < m_voices.size(); i++) {
        if (!m_voices[i].getIsNotePlaying()) {
            m_voices[i].noteOn(midiNote, velocity);
            m_lastVoiceIndex = i;
            voiceFound = true;
            break;
        }
    }

    if (!voiceFound) {
        m_voices[m_lastVoiceIndex].noteOn(midiNote, velocity);
    }
}

void Synth::noteOff(uint8_t midiNote) noexcept
{
    for (auto& voice : m_voices) {
        if (voice.getCurrentNote() == midiNote) {
            voice.noteOff();
        }
    }
}

void Synth::setCutoffFrequency(float cutoffFrequency)
{
    for (auto& voice : m_voices) {
        voice.setCutoffFrequency(cutoffFrequency);
    }
}

void Synth::setQ(float q)
{
    for (auto& voice : m_voices) {
        voice.setQ(q);
    }
}

