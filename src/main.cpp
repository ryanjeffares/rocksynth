#include "Synth/Oscillator.hpp"

#include <fmt/core.h>

#include <rtaudio/RtAudio.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <thread>

static Oscillator s_oscillator;

int audioCallback(
        void* outBuffer,
        [[maybe_unused]] void* inBuffer,
        uint32_t bufferFrames,
        [[maybe_unused]] double streamTime,
        RtAudioStreamStatus status,
        [[maybe_unused]] void* userData
)
{
    float* buffer = (float*)outBuffer;

    if (status) {
        fmt::print(stderr, "Stream underflow\n");
        return 0;
    }

    for (size_t sample = 0; sample < bufferFrames; sample++) {
        auto value = s_oscillator.getNextSample();
        for (size_t channel = 0; channel < 2; channel++) {
            *buffer++ = value * 0.5f;
        }
    }

    return 0;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
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
    
    RtAudio::StreamParameters streamParameters {
        .deviceId = defaultDevice,
        .nChannels = 2,
        .firstChannel = 0
    };

    uint32_t sr = 44100;
    uint32_t bufferSize = 256; 
    
    s_oscillator.prepare(sr);
    s_oscillator.setShape(Oscillator::Shape::Saw);
    s_oscillator.setFrequency(220.0f);

    if (dac.openStream(&streamParameters, nullptr, RTAUDIO_FLOAT32, sr, &bufferSize, &audioCallback)) {
        fmt::print(stderr, "{}\n", dac.getErrorText());
        std::exit(1);
    }

    if (dac.startStream()) {
        fmt::print(stderr, "{}\n", dac.getErrorText());
        cleanup();
        std::exit(1);
    }

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10s);

    if (dac.isStreamRunning()) {
        dac.stopStream();
    }

    cleanup();
}

