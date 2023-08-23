#ifndef DELAY_LINE_HPP
#define DELAY_LINE_HPP

#include <cstdint>
#include <cstdlib>

class DelayLine
{
public:
    DelayLine() = default;
    ~DelayLine();

    DelayLine(const DelayLine& other);
    DelayLine(DelayLine&& other) noexcept;

    DelayLine& operator=(const DelayLine& other);
    DelayLine& operator=(DelayLine&& other) noexcept;

    void setMaxDelaySamples(size_t size) noexcept;
    void setDelaySamples(int64_t delay) noexcept;

    void setMaxDelaySeconds(float seconds) noexcept;
    void setDelaySeconds(float seconds) noexcept;

    void prepare(uint32_t sampleRate) noexcept;
    float getNextSample(float inputSample) noexcept;

private:
    size_t m_maxSize{0};
    float* m_data{nullptr};

    float m_sampleRate{};
    int64_t m_delay{0};
    int64_t m_recordIndex{0};
};

#endif
