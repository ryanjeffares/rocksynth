#include "AudioBuffer.hpp"

#include <cstring>

AudioBuffer::AudioBuffer(size_t numChannels, size_t bufferSize)
    : m_numChannels{numChannels}
    , m_bufferSize{bufferSize}
    , m_data{new float[m_numChannels * m_bufferSize]}
{
    std::memset(m_data, 0, sizeof(float) * m_bufferSize * m_bufferSize);
}

AudioBuffer::AudioBuffer(const AudioBuffer& other)
    : m_numChannels{other.numChannels()}
    , m_bufferSize{other.bufferSize()}
    , m_data{new float[m_numChannels * m_bufferSize]}
{
    std::memcpy(m_data, other.m_data, sizeof(float) * m_numChannels * m_bufferSize);
}

AudioBuffer::AudioBuffer(AudioBuffer&& other) noexcept
    : m_numChannels{other.numChannels()}
    , m_bufferSize{other.bufferSize()}
    , m_data{other.m_data}
{
    other.m_numChannels = 0;
    other.m_bufferSize = 0;
    other.m_data = nullptr;
}

AudioBuffer& AudioBuffer::operator=(const AudioBuffer& other)
{
    if (&other != this) {
        delete[] m_data;
        m_numChannels = other.numChannels();
        m_bufferSize = other.bufferSize();
        m_data = new float[m_numChannels * m_bufferSize];
        std::memcpy(m_data, other.m_data, sizeof(float) * m_numChannels * m_bufferSize);
    }

    return *this;
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) noexcept
{
    if (&other != this) {
        delete[] m_data;
        m_numChannels = other.numChannels();
        m_bufferSize = other.bufferSize();
        m_data = other.m_data;
        other.m_numChannels = 0;
        other.m_bufferSize = 0;
        other.m_data = nullptr;
    }

    return *this;
}

AudioBuffer::~AudioBuffer()
{
    delete[] m_data;
}

void AudioBuffer::setSample(size_t channel, size_t sample, float value) noexcept
{
    m_data[channel * m_bufferSize + sample] = value;
}

void AudioBuffer::addSample(size_t channel, size_t sample, float value) noexcept
{
    m_data[channel * m_bufferSize + sample] += value;
}

float AudioBuffer::getSample(size_t channel, size_t sample) const noexcept
{
    return m_data[channel * m_bufferSize + sample];
}

size_t AudioBuffer::numChannels() const noexcept
{
    return m_numChannels;
}

size_t AudioBuffer::bufferSize() const noexcept
{
    return m_bufferSize;
}

