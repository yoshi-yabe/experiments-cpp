#pragma once

#include <mutex>

template<typename T>
class TripleBuffer
{
public:
    TripleBuffer():m_present{&m_bufs[0]},m_ready{&m_bufs[1]},m_busy{&m_bufs[2]} {}
    ~TripleBuffer() {}

    struct Exclusive
    {
    public:
        Exclusive(const Exclusive&) = delete;
        Exclusive& operator = (const Exclusive&) = delete;
        Exclusive(std::mutex& mtx, T* p):m_mtx{mtx},data{*p}
        {
        }
        ~Exclusive()
        {
            m_mtx.unlock();
        }
        T& data;
    private:
        std::mutex& m_mtx;
    };

    std::unique_ptr<Exclusive> LockBusy()
    {
        m_mtx_busy.lock();
        return std::make_unique<Exclusive>(m_mtx_busy, m_busy.load());
    }

    std::unique_ptr<Exclusive> LockPresent()
    {
        m_mtx_present.lock();
        return std::make_unique<Exclusive>(m_mtx_present, m_present.load());
    }

    void SwapBusy()
    {
        std::lock_guard<std::mutex> lock{m_mtx_busy};
        T* p = m_ready.exchange(m_busy);
        m_busy.store(p);
    }

    void SwapPresent()
    {
        std::lock_guard<std::mutex> lock{m_mtx_present};
        T* p = m_ready.exchange(m_present);
        m_present.store(p);

    }


private:
    T m_bufs[3];
    std::atomic<T*> m_present;
    std::atomic<T*> m_ready;
    std::atomic<T*> m_busy;
    std::mutex m_mtx_busy;
    std::mutex m_mtx_present;
};
