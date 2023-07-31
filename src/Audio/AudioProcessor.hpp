#ifndef AUDIO_PROCESSOR_HPP
#define AUDIO_PROCESSOR_HPP

#include "AudioBuffer.hpp"

#include <cstdint>

class AudioProcessor
{
public:
    virtual ~AudioProcessor() = default;

    virtual void prepare(uint32_t sampleRate) = 0;
    virtual void process(AudioBuffer& bufferToFill) = 0;
};

#endif
