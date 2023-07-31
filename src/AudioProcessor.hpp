#ifndef AUDIO_PROCESSOR_HPP
#define AUDIO_PROCESSOR_HPP

#include "AudioBuffer.hpp"

#include <concepts>
#include <cstdint>

template<std::floating_point SampleType>
class AudioProcessor
{
public:
    virtual ~AudioProcessor() = default;

    virtual void prepare(uint32_t sampleRate) = 0;
    virtual void process(AudioBuffer<SampleType>& bufferToFill) = 0;
};

#endif
