#include "Synth.hpp"

void Synth::prepare(uint32_t sampleRate)
{
    for (auto& voice : m_voices) {
        voice.prepare(sampleRate);
    }
}

void Synth::process(AudioBuffer &bufferToFill)
{
    for (auto& voice : m_voices) {
        voice.process(bufferToFill);
    }
}

void Synth::noteOn(uint8_t midiNote, uint8_t velocity) noexcept
{
    m_voices[m_lastVoiceIndex++].noteOn(midiNote, velocity);
    m_lastVoiceIndex %= 8;
}

void Synth::noteOff(uint8_t midiNote) noexcept
{
    for (auto& voice : m_voices) {
        if (voice.getCurrentNote() == midiNote) {
            voice.noteOff();
        }
    }
}

