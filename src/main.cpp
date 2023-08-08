#include "Synth/Synth.hpp"

#include <fmt/core.h>

#include <rtaudio/RtAudio.h>
#include <rtmidi/RtMidi.h>

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>

static Synth s_synth;
static RtMidiIn s_midiIn;

static constexpr uint8_t s_noteOnStatusByte        = 0b10010000;
static constexpr uint8_t s_noteOffStatusByte       = 0b10000000;
static constexpr uint8_t s_controlChangeStatusByte = 0b10110000;

int audioCallback(
        void* outBuffer,
        [[maybe_unused]] void* inBuffer,
        uint32_t bufferFrames,
        [[maybe_unused]] double streamTime,
        RtAudioStreamStatus status,
        [[maybe_unused]] void* userData
)
{
    std::vector<uint8_t> midiMessage;
    s_midiIn.getMessage(&midiMessage);
    if (!midiMessage.empty()) {
#ifdef ROCKSYNTH_DEBUG
        fmt::print("MIDI message with timestamp {}\n", timeStamp);
        for (size_t i = 0; i < midiMessage.size(); i++) {
            fmt::print("Byte {} = {:b}\n", i, (int)midiMessage[i]);
        }
#endif

        auto statusByte = midiMessage[0] & 0b11110000;
        switch (statusByte) {
            case s_noteOnStatusByte: {
                uint8_t note = midiMessage[1];
                uint8_t velocity = midiMessage[2];
#ifdef ROCKSYNTH_DEBUG
                fmt::print("Note on: note number = {}, velocity = {}\n", note, velocity);
#endif
                s_synth.noteOn(note, velocity);
                break;
            }
            case s_noteOffStatusByte: {
                uint8_t note = midiMessage[1];
#ifdef ROCKSYNTH_DEBUG
                fmt::print("Note off: note number = {}\n", note);
#endif
                s_synth.noteOff(note);
                break;
            }
        }
    }

    float* buffer = (float*)outBuffer;

    if (status) {
        fmt::print(stderr, "Stream underflow\n");
        return 0;
    }

    AudioBuffer bufferToFill{2, static_cast<size_t>(bufferFrames)};
    s_synth.process(bufferToFill);

    for (size_t sample = 0; sample < bufferFrames; sample++) {
        for (size_t channel = 0; channel < 2; channel++) {
            *buffer++ = bufferToFill.getSample(channel, sample);
        }
    }

    return 0;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    uint32_t midiDeviceNum = 0;
    if (argc > 1) {
        try {
            midiDeviceNum = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            fmt::print(stderr, "Expected integer for MIDI device number: {}\n", e.what());
        }
    }

    auto numMidiPorts = s_midiIn.getPortCount();
    if (numMidiPorts == 0) {
        fmt::print("No MIDI input devices found.\n\n");
    } else {
        for (uint32_t i = 0; i < numMidiPorts; i++) {
            fmt::print("Found MIDI device #{}: {}\n", i, s_midiIn.getPortName(i));
        }
        s_midiIn.openPort(midiDeviceNum);
        fmt::print("Using MIDI device: {}\n", s_midiIn.getPortName(midiDeviceNum));
    }
 
    RtAudio dac;

    auto cleanup = [&] {
        if (dac.isStreamOpen()) {
            dac.closeStream();
        }
    };

    auto deviceIds = dac.getDeviceIds();
    if (deviceIds.empty()) {
        fmt::print(stderr, "No audio devices found.\n");
        std::exit(1);
    }

    auto defaultDevice = dac.getDefaultOutputDevice();
    auto deviceInfo = dac.getDeviceInfo(defaultDevice);
    fmt::print("Using audio output device: {}\n", deviceInfo.name);
   
    RtAudio::StreamParameters streamParameters {
        .deviceId = defaultDevice,
        .nChannels = 2,
        .firstChannel = 0
    };

    uint32_t sr = 44100;
    uint32_t bufferSize = 256; 
    
    s_synth.prepare(sr);

    s_synth.setShape<0>(Oscillator::Shape::Saw);
    s_synth.setShape<1>(Oscillator::Shape::Pulse);

    s_synth.setPulseWidth<1>(0.4f);

    s_synth.setAdsrParam<Adsr::Phase::Attack>(0.0f);
    s_synth.setAdsrParam<Adsr::Phase::Decay>(1.0f);
    s_synth.setAdsrParam<Adsr::Phase::Sustain>(0.3f);
    s_synth.setAdsrParam<Adsr::Phase::Release>(2.0f);

    if (dac.openStream(&streamParameters, nullptr, RTAUDIO_FLOAT32, sr, &bufferSize, &audioCallback)) {
        fmt::print(stderr, "{}\n", dac.getErrorText());
        std::exit(1);
    }

    if (dac.startStream()) {
        fmt::print(stderr, "{}\n", dac.getErrorText());
        cleanup();
        std::exit(1);
    }

    std::cin.get();

    if (dac.isStreamRunning()) {
        dac.stopStream();
    }

    cleanup();
}

