#ifndef AUDIO_BUFFER_HPP
#define AUDIO_BUFFER_HPP

#include <fmt/core.h>

#include <concepts>
#include <cstdlib>
#include <cstring>

template<std::floating_point SampleType>
class AudioBuffer
{
public:
    AudioBuffer(size_t numChannels, size_t bufferSize)
        : m_NumChannels{numChannels}
        , m_BufferSize{bufferSize}
        , m_Data{new SampleType[m_NumChannels * m_BufferSize]}
    {
        
    }

    AudioBuffer(const AudioBuffer<SampleType>& other)
        : m_NumChannels{other.numChannels()}
        , m_BufferSize{other.bufferSize()}
        , m_Data{new SampleType[m_NumChannels * m_BufferSize]}
    {
        std::memcpy(m_Data, other.m_Data, m_NumChannels * m_BufferSize);
    }

    AudioBuffer(AudioBuffer<SampleType>&& other)
        : m_NumChannels{other.numChannels()}
        , m_BufferSize{other.bufferSize()}
        , m_Data{other.m_Data}
    {
        other.m_NumChannels = 0;
        other.m_BufferSize = 0;
        other.m_Data = nullptr;
    }

    AudioBuffer<SampleType>& operator=(const AudioBuffer<SampleType> other)
    {
        if (&other != this) {
            delete[] m_Data;
            m_NumChannels = other.numChannels();
            m_BufferSize = other.bufferSize();
            m_Data = new SampleType[m_NumChannels * m_BufferSize];
            std::memcpy(m_Data, other.m_Data, m_NumChannels * m_BufferSize);
        }

        return *this;
    }

    AudioBuffer<SampleType>& operator=(AudioBuffer<SampleType>&& other)
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

    ~AudioBuffer()
    {
        delete[] m_Data;
    }

    void setSample(size_t channel, size_t sample, SampleType value) noexcept
    {
        m_Data[channel * m_BufferSize + sample] = value;
    }

    [[nodiscard]] SampleType getSample(size_t channel, size_t sample) const noexcept
    {
        return m_Data[channel * m_BufferSize + sample];
    }

    [[nodiscard]] size_t numChannels() const noexcept
    {
        return m_NumChannels;
    }

    [[nodiscard]] size_t bufferSize() const noexcept
    {
        return m_BufferSize;
    }

    SampleType* getInterleavedWritePointer() const noexcept
    {
        return m_Data;
    }
    
    const SampleType* getInterleavedReadPointer() const noexcept
    {
        return m_Data;
    }

    void printContents() const
    {
        for (size_t channel = 0; channel < m_NumChannels; channel++) {
            fmt::print("Channel {}: [", channel);
            for (size_t sample = 0; sample < m_BufferSize; sample++) {
                fmt::print("{}", getSample(channel, sample));
                if (sample < m_BufferSize - 1) {
                    fmt::print(", ");
                }
            }
            fmt::print("]\n");
        } 
    }

private:
    size_t m_NumChannels;
    size_t m_BufferSize;
    SampleType* m_Data;
};

#endif
