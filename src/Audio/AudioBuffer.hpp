#ifndef AUDIO_BUFFER_HPP
#define AUDIO_BUFFER_HPP

#include <cstdlib>

class AudioBuffer
{
public:
    // Constructor
    AudioBuffer(size_t numChannels, size_t bufferSize);

    // Copy + Move constructors
    AudioBuffer(const AudioBuffer& other);
    AudioBuffer(AudioBuffer&& other) noexcept;

    // Copy + Move assign operators
    AudioBuffer& operator=(const AudioBuffer& other);
    AudioBuffer& operator=(AudioBuffer&& other) noexcept;

    // Destructor
    ~AudioBuffer();

    void setSample(size_t channel, size_t sample, float value) noexcept;
    [[nodiscard]] float getSample(size_t channel, size_t sample) const noexcept;

    [[nodiscard]] size_t numChannels() const noexcept;
    [[nodiscard]] size_t bufferSize() const noexcept;

    float* getInterleavedWritePointer() const noexcept;
    const float* getInterleavedReadPointer() const noexcept;

private:
    size_t m_NumChannels;
    size_t m_BufferSize;
    float* m_Data;
};

#endif
