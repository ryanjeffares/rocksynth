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

void Synth::noteOn(uint8_t midiNote)
{
    auto& voice = m_voices[m_lastVoiceIndex++];
    m_lastVoiceIndex %= 8;
    voice.noteOn(midiNote);
}

void Synth::noteOff(uint8_t midiNote)
{
    auto voice = std::find_if(
            m_voices.begin(),
            m_voices.end(),
            [midiNote] (const SynthVoice& voice) {
                return voice.getCurrentNote() == midiNote;
            }
        );
    if (voice != m_voices.end()) {
        voice->noteOff();
    }
}

