#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class BlockingQueue
{
public:
    BlockingQueue(size_t size):m_size{size},m_terminated{false} {}
    ~BlockingQueue() { m_terminated = true; }

    void Terminate()
    {
        m_terminated = true;
    }

    size_t GetCount() const
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_queue.size();
    }

    void Push(T data)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cv_not_full.wait(lock, [this]{return m_terminated || m_queue.size() < m_size;});
        if (m_terminated)
        {
            return;
        }
        m_queue.push(std::move(data));
        m_cv_not_empty.notify_all();
    }

    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cv_not_empty.wait(lock, [this]{return m_terminated || !m_queue.empty();});
        if (m_terminated)
        {
            return std::move(T{});
        }
        T data = std::move(m_queue.front());
        m_queue.pop();
        m_cv_not_full.notify_all();
        return data;
    }

private:
    const size_t m_size;
    bool m_terminated;
    std::queue<T> m_queue;
    mutable std::mutex m_mtx;
    std::condition_variable m_cv_not_empty;
    std::condition_variable m_cv_not_full;
};
