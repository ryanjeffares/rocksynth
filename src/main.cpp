#include "Synth/Synth.hpp"

#include <fmt/core.h>

#include <rtaudio/RtAudio.h>

#include <chrono>
#include <cstdint>
#include <thread>

static Synth s_synth;

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
    
    s_synth.prepare(sr);

    s_synth.setShape<0>(Oscillator::Shape::Saw);
    s_synth.setShape<1>(Oscillator::Shape::Pulse);

    s_synth.setPulseWidth<1>(0.4f);

    s_synth.setAdsrParam<Adsr::Phase::Attack>(2.0f);
    s_synth.setAdsrParam<Adsr::Phase::Decay>(2.0f);
    s_synth.setAdsrParam<Adsr::Phase::Sustain>(0.3f);
    s_synth.setAdsrParam<Adsr::Phase::Release>(2.0f);

    s_synth.noteOn(32);

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

    s_synth.noteOff(32);

    std::this_thread::sleep_for(10s);

    if (dac.isStreamRunning()) {
        dac.stopStream();
    }

    cleanup();
}

