#include "Synth.hpp"

#include <algorithm>
#include <cmath>

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

void Synth::noteOn(uint8_t midiNote, uint8_t velocity)
{
    auto& voice = m_voices[m_lastVoiceIndex++];
    m_lastVoiceIndex %= 8;
    voice.noteOn(midiNote, velocity);
}

void Synth::noteOff(uint8_t midiNote)
{
    for (auto& voice : m_voices) {
        if (voice.getCurrentNote() == midiNote) {
            voice.noteOff();
        }
    }
}

