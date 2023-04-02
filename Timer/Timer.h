#pragma once

#include <stdint.h>
#include <memory>

class Timer
{
public:
    Timer();
    ~Timer();

    bool isEnabled() const;
    uint32_t tick();
    uint32_t elapse(bool reset = false);
    void wait(uint32_t msec) const;

    static uint32_t getTimestamp();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
