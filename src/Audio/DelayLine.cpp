#include "DelayLine.hpp"

#include <cmath>
#include <cstring>

DelayLine::~DelayLine()
{
    delete[] m_data;
}

DelayLine::DelayLine(const DelayLine& other)
    : m_maxSize{other.m_maxSize}
    , m_data{new float[m_maxSize]}
{
    std::memcpy(m_data, other.m_data, sizeof(float) * m_maxSize);
}

DelayLine::DelayLine(DelayLine&& other) noexcept
    : m_maxSize{other.m_maxSize}
    , m_data{other.m_data}
{
    other.m_maxSize = 0;
    other.m_data = nullptr;
}

DelayLine& DelayLine::operator=(const DelayLine& other)
{
    if (this != &other) {
        delete[] m_data;
        m_maxSize = other.m_maxSize;
        m_data = new float[m_maxSize];
        std::memcpy(m_data, other.m_data, sizeof(float) * m_maxSize);
    }

    return *this;
}

DelayLine& DelayLine::operator=(DelayLine&& other) noexcept
{
    if (this != &other) {
        delete[] m_data;
        m_maxSize = other.m_maxSize;
        m_data = other.m_data;
        other.m_maxSize = 0;
        other.m_data = nullptr;
    }

    return *this;
}

void DelayLine::setMaxDelaySamples(size_t size) noexcept
{
    delete[] m_data;
    m_maxSize = size;
    m_data = new float[m_maxSize];
    std::memset(m_data, 0, sizeof(float) * m_maxSize);
}

void DelayLine::setDelaySamples(size_t delay) noexcept
{
    m_delay = std::fmin(delay, m_maxSize);
    if (m_dataIndex >= static_cast<int64_t>(delay)) {
        m_dataIndex = delay - 1;
    }
}

void DelayLine::setMaxDelaySeconds(float seconds) noexcept
{
    setMaxDelaySamples(static_cast<size_t>(seconds * m_sampleRate));
}

void DelayLine::setDelaySeconds(float seconds) noexcept
{
    setDelaySamples(static_cast<size_t>(seconds * m_sampleRate));
}

void DelayLine::prepare(uint32_t sampleRate) noexcept
{
    m_sampleRate = sampleRate;
}

float DelayLine::getNextSample(float inputSample) noexcept
{
    if (m_data == nullptr) {
        return 0.0f;
    }

    auto read = m_data[m_dataIndex];
    m_data[m_dataIndex] = inputSample;
    m_dataIndex++;
    if (m_dataIndex >= static_cast<int64_t>(m_delay)) {
        m_dataIndex = 0;
    }

    return read;
}
