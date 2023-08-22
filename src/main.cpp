#include "Synth/Synth.hpp"

#include <fmt/core.h>

#include <rtaudio/RtAudio.h>
#include <rtmidi/RtMidi.h>

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <thread>
#include <vector>

static std::optional<Synth> s_synth;
static std::optional<RtMidiIn> s_midiIn;

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
    while (true) {
        [[maybe_unused]] double timeStamp = s_midiIn->getMessage(&midiMessage);
        if (!midiMessage.empty()) {
#ifdef ROCKSYNTH_DEBUG
            fmt::print("MIDI message with timestamp {}\n", timeStamp);
            for (size_t i = 0; i < midiMessage.size(); i++) {
                fmt::print("Byte {} = {:#b}\n", i, (int)midiMessage[i]);
            }
#endif

            auto statusByte = midiMessage[0] & 0b11110000;
            switch (statusByte) {
                case s_controlChangeStatusByte: {
                    uint8_t controllerNumber = midiMessage[1];
                    uint8_t value = midiMessage[2];
#ifdef ROCKSYNTH_DEBUG
                    fmt::print("Control Change: controller number = {}, value = {}\n\n", controllerNumber, value);
#endif
                    switch (controllerNumber) {
                        case 14:
                            // max of 5 seconds on timed ADSR params
                            s_synth->setAdsrParam<Adsr::Phase::Attack>(static_cast<float>(value) / 127.0f * 5.0f);
                            break;
                        case 15:
                            s_synth->setAdsrParam<Adsr::Phase::Decay>(static_cast<float>(value) / 127.0f * 5.0f);
                            break;
                        case 16:
                            // scale sustain to be 0 - 1
                            s_synth->setAdsrParam<Adsr::Phase::Sustain>(static_cast<float>(value) / 127.0f);
                            break;
                        case 17:
                            s_synth->setAdsrParam<Adsr::Phase::Release>(static_cast<float>(value) / 127.0f * 5.0f);
                            break;
                        case 18:
                            // scale this to be 0 - 3
                            s_synth->setShape<0>(static_cast<Oscillator::Shape>(value / 42));
                            break;
                        case 19:
                            s_synth->setShape<1>(static_cast<Oscillator::Shape>(value / 42));
                            break;
                        case 20:
                            // scale pulsewidth to be 0 - 1
                            s_synth->setPulseWidth<0>(static_cast<float>(value) / 127.0f);
                            break;
                        case 21:
                            s_synth->setPulseWidth<1>(static_cast<float>(value) / 127.0f);
                            break;
                        case 22:
                            s_synth->setVolume<0>(static_cast<float>(value) / 127.0f);
                            break;
                        case 23:
                            s_synth->setVolume<1>(static_cast<float>(value) / 127.0f);
                            break;
                        case 24:
                            // scale cf to be 20 - 20000
                            s_synth->setCutoffFrequency((static_cast<float>(value) / 127.0f * 19980.0f) + 20.0f);
                            break;
                        case 25:
                            // scale q to be 1 - 10
                            s_synth->setQ((static_cast<float>(value / 127.0f) * 9.0f) + 1.0f);
                            break;
                        case 26:
                            // max of 5 seconds on timed ADSR params
                            s_synth->setVcfAdsrParam<Adsr::Phase::Attack>(static_cast<float>(value) / 127.0f * 5.0f);
                            break;
                        case 27:
                            s_synth->setVcfAdsrParam<Adsr::Phase::Decay>(static_cast<float>(value) / 127.0f * 5.0f);
                            break;
                        case 28:
                            // scale sustain to be 0 - 1
                            s_synth->setVcfAdsrParam<Adsr::Phase::Sustain>(static_cast<float>(value) / 127.0f);
                            break;
                        case 29:
                            s_synth->setVcfAdsrParam<Adsr::Phase::Release>(static_cast<float>(value) / 127.0f * 5.0f);
                            break;
                        case 102:
                            s_synth->setChorusParam<Chorus::Param::Depth>(static_cast<float>(value) / 127.0f);
                            break;
                        case 103:
                            // scale this to be 0.1 - 20
                            s_synth->setChorusParam<Chorus::Param::Rate>((static_cast<float>(value) / 127.0f * 19.9f) + 0.1f);
                            break;
                        case 104:
                            s_synth->setChorusParam<Chorus::Param::Feedback>(static_cast<float>(value) / 127.0f);
                            break;
                        case 105:
                            s_synth->setChorusParam<Chorus::Param::DryWet>(static_cast<float>(value) / 127.0f);
                            break;
                    }
                    break;
                }
                case s_noteOnStatusByte: {
                    uint8_t note = midiMessage[1];
                    uint8_t velocity = midiMessage[2];
#ifdef ROCKSYNTH_DEBUG
                    fmt::print("Note on: note number = {}, velocity = {}\n\n", note, velocity);
#endif
                    s_synth->noteOn(note, velocity);
                    break;
                }
                case s_noteOffStatusByte: {
                    uint8_t note = midiMessage[1];
#ifdef ROCKSYNTH_DEBUG
                    fmt::print("Note off: note number = {}\n\n", note);
#endif
                    s_synth->noteOff(note);
                    break;
                }
            }
        } else {
            break;
        }
    }

    float* buffer = (float*)outBuffer;

    if (status) {
        fmt::print(stderr, "Stream underflow\n");
        return 0;
    }

    AudioBuffer bufferToFill{2, static_cast<size_t>(bufferFrames)};
    s_synth->process(bufferToFill);

    for (size_t sample = 0; sample < bufferFrames; sample++) {
        for (size_t channel = 0; channel < 2; channel++) {
            *buffer++ = bufferToFill.getSample(channel, sample);
        }
    }

    return 0;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    try {
        s_synth = Synth(2);
    } catch (const std::exception& e) {
        fmt::print(stderr, "Error constructing Synth: {}\n", e.what());
        std::exit(1);
    }

    try {
        s_midiIn.emplace();
    } catch (const std::exception& e) {
        fmt::print(stderr, "Error constructing RtMidiIn: {}\n", e.what());
        std::exit(1);
    }

    uint32_t midiDeviceNum = 0;
    if (argc > 1) {
        try {
            midiDeviceNum = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            fmt::print(stderr, "Expected integer for MIDI device number: {}\n", e.what());
        }
    }

    auto numMidiPorts = s_midiIn->getPortCount();
    if (numMidiPorts == 0) {
        fmt::print("No MIDI input devices found.\n\n");
    } else {
        for (uint32_t i = 0; i < numMidiPorts; i++) {
            fmt::print("Found MIDI device #{}: {}\n", i, s_midiIn->getPortName(i));
        }
        s_midiIn->openPort(midiDeviceNum);
        fmt::print("Using MIDI device: {}\n", s_midiIn->getPortName(midiDeviceNum));
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
    
    s_synth->prepare(sr);

    s_synth->setShape<0>(Oscillator::Shape::Saw);
    s_synth->setShape<1>(Oscillator::Shape::Pulse);

    s_synth->setPulseWidth<1>(0.4f);

    s_synth->setAdsrParam<Adsr::Phase::Attack>(0.0f);
    s_synth->setAdsrParam<Adsr::Phase::Decay>(0.1f);
    s_synth->setAdsrParam<Adsr::Phase::Sustain>(1.0f);
    s_synth->setAdsrParam<Adsr::Phase::Release>(0.2f);

    s_synth->setVcfAdsrParam<Adsr::Phase::Attack>(0.5f);
    s_synth->setVcfAdsrParam<Adsr::Phase::Decay>(0.2f);
    s_synth->setVcfAdsrParam<Adsr::Phase::Sustain>(0.3f);
    s_synth->setVcfAdsrParam<Adsr::Phase::Release>(0.2f);

    s_synth->setChorusParam<Chorus::Param::Depth>(0.1f);
    s_synth->setChorusParam<Chorus::Param::Rate>(0.5f);
    s_synth->setChorusParam<Chorus::Param::Feedback>(0.3f);
    s_synth->setChorusParam<Chorus::Param::DryWet>(0.5f);

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

