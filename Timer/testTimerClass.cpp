#include "gtest/gtest.h"
#include "Timer.h"

#include <algorithm>
#include <vector>

template <typename T>
class List
{
public:
    List() {}
    ~List() {}

    void Push(T v)
    {
        m_list.push_back(v);
    }
    T Min() {
        return *std::min_element(m_list.begin(), m_list.end());
    }
    T Max() {
        return *std::max_element(m_list.begin(), m_list.end());
    }

private:
    std::vector<T> m_list;
};


TEST(TimerClass, testTimer)
{
    uint32_t WAIT_MSEC = 1;
    {
        List<uint32_t> delta_list;
        auto timer = std::make_unique<Timer>();
        for (auto i = 0; i < 10; ++i)
        {
            auto t0 = timer->tick();
            timer->wait(WAIT_MSEC);
            auto delta = timer->elapse();
            delta_list.Push(delta);
        }
        EXPECT_EQ(delta_list.Min(), WAIT_MSEC);
        EXPECT_EQ(delta_list.Max(), WAIT_MSEC);
    }

    WAIT_MSEC = 2;
    {
        List<uint32_t> delta_list;
        auto timer = std::make_unique<Timer>();
        for (auto i = 0; i < 10; ++i)
        {
            auto t0 = timer->tick();
            timer->wait(WAIT_MSEC);
            auto delta = timer->elapse();
            delta_list.Push(delta);
        }
        EXPECT_EQ(delta_list.Min(), WAIT_MSEC);
        EXPECT_EQ(delta_list.Max(), WAIT_MSEC);
    }

    WAIT_MSEC = 3;
    {
        List<uint32_t> delta_list;
        auto timer = std::make_unique<Timer>();
        for (auto i = 0; i < 10; ++i)
        {
            auto t0 = timer->tick();
            timer->wait(WAIT_MSEC);
            auto delta = timer->elapse();
            delta_list.Push(delta);
        }
        EXPECT_EQ(delta_list.Min(), WAIT_MSEC);
        EXPECT_EQ(delta_list.Max(), WAIT_MSEC);
    }
}
