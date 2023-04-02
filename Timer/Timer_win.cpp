#include "Timer.h"

#include <Windows.h>
//#include <mmsystem.h> // timeGetTime()のため
//#pragma comment(lib, "winmm.lib")

#define THRESHOLD_SLEEP_MSEC    2

struct Timer::Impl
{
    bool m_enabled;
    double m_freq;
    LARGE_INTEGER m_t;
    HANDLE m_waitable_timer;

    Timer::Impl::Impl()
        :m_enabled{false}
    {
        LARGE_INTEGER freq;

        BOOL success = ::QueryPerformanceFrequency(&freq);
        if (success)
        {
            m_freq = freq.QuadPart / 1000.0 / 1000.0;  // 内部的にはマイクロ秒単位

            if (::QueryPerformanceCounter(&m_t))
            {
                m_enabled = true;
            }
        }

        m_waitable_timer = ::CreateWaitableTimer(NULL, TRUE, NULL);
    }

    Timer::Impl::~Impl()
    {
        if (m_waitable_timer)
        {
            ::CloseHandle(m_waitable_timer);
        }
    }
};


Timer::Timer()
    :m_impl{std::make_unique<Impl>()}
{
}

Timer::~Timer()
{
    m_impl.reset();
}

bool
Timer::isEnabled() const
{
    return m_impl->m_enabled;
}

uint32_t
Timer::tick()
{
    LARGE_INTEGER t;
    ::QueryPerformanceCounter(&m_impl->m_t);

    return static_cast<uint32_t>(m_impl->m_t.QuadPart / m_impl->m_freq / 1000.0);
}

uint32_t
Timer::elapse(bool reset)
{
    LARGE_INTEGER t;
    ::QueryPerformanceCounter(&t);

    auto ret = (t.QuadPart - m_impl->m_t.QuadPart) / m_impl->m_freq;

    if (reset)
    {
        m_impl->m_t = t;
    }

    return static_cast<uint32_t>(ret / 1000.0);
}
void
Timer::wait(uint32_t msec) const
{
    auto us = msec * 1000.0;
    if (msec >= THRESHOLD_SLEEP_MSEC)
    {
        LARGE_INTEGER t;
        ::QueryPerformanceCounter(&t);
        auto t0 = t.QuadPart;

        LARGE_INTEGER interval;
        interval.QuadPart = (LONGLONG)(msec - 1) * 1000 * -10;  // 100 ナノ秒単位（負は相対時間を示す）
        ::timeBeginPeriod(1);
        ::SetWaitableTimer(m_impl->m_waitable_timer, &interval, 0, NULL, FALSE, FALSE);
        ::WaitForSingleObject(m_impl->m_waitable_timer, INFINITE);
        ::timeEndPeriod(1);

        ::QueryPerformanceCounter(&t);
        auto elapsed = (t.QuadPart - t0) / m_impl->m_freq;
        while (elapsed < us)
        {
            ::QueryPerformanceCounter(&t);
            elapsed = (t.QuadPart - t0) / m_impl->m_freq;
            ::YieldProcessor();
            ::Sleep(0);
        }
    }
    else
    {
        LARGE_INTEGER t;
        ::QueryPerformanceCounter(&t);
        {
            auto t0 = t.QuadPart;
            auto elapsed = 0.0;
            while (elapsed < us)
            {
                ::QueryPerformanceCounter(&t);
                elapsed = (t.QuadPart - t0) / m_impl->m_freq;
                ::YieldProcessor();
                ::Sleep(0);
            }
        }
    }
}

uint32_t
Timer::getTimestamp()
{
    uint32_t ret = 0;

    LARGE_INTEGER freq;

    BOOL success = ::QueryPerformanceFrequency(&freq);
    if (success)
    {
        auto f = freq.QuadPart / 1000;

        LARGE_INTEGER t;
        if (::QueryPerformanceCounter(&t))
        {
            ret = static_cast<uint32_t>(t.QuadPart / f);
        }
    }

    return ret;
}
