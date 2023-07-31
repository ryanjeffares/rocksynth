#include <fmt/core.h>

#include <rtaudio/RtAudio.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <thread>

static std::random_device s_Rd;
static std::mt19937 s_Gen{s_Rd()};
static std::uniform_real_distribution<float> s_Dist(-1.0f, 1.0f);

int audio_callback(void* outBuffer, [[maybe_unused]] void* inBuffer, uint32_t bufferFrames, [[maybe_unused]] double streamTime, RtAudioStreamStatus status, [[maybe_unused]] void* userData)
{
    float* buffer = (float*)outBuffer;

    if (status) {
        fmt::print(stderr, "Stream underflow\n");
        return 0;
    }

    for (size_t sample = 0; sample < bufferFrames; sample++) {
        for (size_t channel = 0; channel < 2; channel++) {
            *buffer++ = s_Dist(s_Gen) * 0.5f;
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

    auto device_ids = dac.getDeviceIds();
    if (device_ids.empty()) {
        fmt::print(stderr, "No audio devices found.\n");
        std::exit(1);
    }

    auto default_device = dac.getDefaultOutputDevice();
    
    RtAudio::StreamParameters stream_parameters {
        .deviceId = default_device,
        .nChannels = 2,
        .firstChannel = 0
    };

    uint32_t sr = 44100;
    uint32_t buffer_size = 256; 

    if (dac.openStream(&stream_parameters, nullptr, RTAUDIO_FLOAT32, sr, &buffer_size, &audio_callback)) {
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

