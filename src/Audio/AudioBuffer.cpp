#include "AudioBuffer.hpp"

#include <cstring>

AudioBuffer::AudioBuffer(size_t numChannels, size_t bufferSize)
    : m_NumChannels{numChannels}
    , m_BufferSize{bufferSize}
    , m_Data{new float[m_NumChannels * m_BufferSize]}
{
    
}

AudioBuffer::AudioBuffer(const AudioBuffer& other)
    : m_NumChannels{other.numChannels()}
    , m_BufferSize{other.bufferSize()}
    , m_Data{new float[m_NumChannels * m_BufferSize]}
{
    std::memcpy(m_Data, other.m_Data, m_NumChannels * m_BufferSize);
}

AudioBuffer::AudioBuffer(AudioBuffer&& other)
    : m_NumChannels{other.numChannels()}
    , m_BufferSize{other.bufferSize()}
    , m_Data{other.m_Data}
{
    other.m_NumChannels = 0;
    other.m_BufferSize = 0;
    other.m_Data = nullptr;
}

AudioBuffer& AudioBuffer::operator=(const AudioBuffer& other)
{
    if (&other != this) {
        delete[] m_Data;
        m_NumChannels = other.numChannels();
        m_BufferSize = other.bufferSize();
        m_Data = new float[m_NumChannels * m_BufferSize];
        std::memcpy(m_Data, other.m_Data, m_NumChannels * m_BufferSize);
    }

    return *this;
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other)
{
    if (&other != this) {
        delete[] m_Data;
        m_NumChannels = other.numChannels();
        m_BufferSize = other.bufferSize();
        m_Data = other.m_Data;
        other.m_NumChannels = 0;
        other.m_BufferSize = 0;
        other.m_Data = nullptr;
    }

    return *this;
}

AudioBuffer::~AudioBuffer()
{
    delete[] m_Data;
}

void AudioBuffer::setSample(size_t channel, size_t sample, float value) noexcept
{
    m_Data[channel * m_BufferSize + sample] = value;
}

float AudioBuffer::getSample(size_t channel, size_t sample) const noexcept
{
    return m_Data[channel * m_BufferSize + sample];
}

size_t AudioBuffer::numChannels() const noexcept
{
    return m_NumChannels;
}

size_t AudioBuffer::bufferSize() const noexcept
{
    return m_BufferSize;
}

float* AudioBuffer::getInterleavedWritePointer() const noexcept
{
    return m_Data;
}

const float* AudioBuffer::getInterleavedReadPointer() const noexcept
{
    return m_Data;
}

