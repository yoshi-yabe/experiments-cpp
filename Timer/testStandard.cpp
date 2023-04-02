#include "gtest/gtest.h"
#include <windows.h> // ::Sleep()
#include <chrono>
#include <thread>
#include <time.h>  // clock()
// #include <unistd.h> // sleep() for linux
#include <mmsystem.h>

#include <algorithm>
#include <vector>

#pragma comment(lib, "winmm.lib")


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

/*
    std::this_thread::sleep_for()
    分解能： 10 ミリ秒程度
*/
TEST(StdSleepFor, testClock)
{   // 計測： clock()
    List<clock_t> delta_list;
    for (auto i = 0; i < 10; ++i)
    {
        clock_t t0 = clock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        clock_t t1 = clock();
        auto delta = t1 - t0;
        delta_list.Push(delta);
    }
    EXPECT_GE(delta_list.Min(), 10 - 2);
    EXPECT_LE(delta_list.Max(), 10 + 8);
}
TEST(StdSleepFor, testGetTickCount)
{   // 計測： GetTickCount64()
    List<ULONGLONG> delta_list;
    for (auto i = 0; i < 10; ++i)
    {
        ULONGLONG t0 = ::GetTickCount64();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        ULONGLONG t1 = ::GetTickCount64();
        auto delta = t1 - t0;
        delta_list.Push(delta);
    }
    EXPECT_GE(delta_list.Min(), 10 - 2);
    EXPECT_LE(delta_list.Max(), 10 + 8);
}

/*
    ::Sleep()
    分解能： 10 ミリ秒程度
*/
TEST(WinSleep, testClock)
{   // 計測： clock()
    List<clock_t> delta_list;
    for(auto i = 0; i < 10; ++i)
    {
        clock_t t0 = clock();
        ::Sleep(1);
        clock_t t1 = clock();
        auto delta = t1 - t0;
        delta_list.Push(delta);
    }
    // 分解能は 10 ミリ程度
    EXPECT_GE(delta_list.Min(), 10 - 2);
    EXPECT_LE(delta_list.Max(), 10 + 8);
}
TEST(WinSleep, testGetTickCount)
{   // 計測： GetTickCount64()
    List<ULONGLONG> delta_list;
    for (auto i = 0; i < 10; ++i)
    {
        ULONGLONG t0 = ::GetTickCount64();
        ::Sleep(1);
        ULONGLONG t1 = ::GetTickCount64();
        auto delta = t1 - t0;
        delta_list.Push(delta);
    }
    // 分解能は 10 ミリ程度
    EXPECT_GE(delta_list.Min(), 10 - 2);
    EXPECT_LE(delta_list.Max(), 10 + 8);
}

/*
    ::Sleep() + ::timeBeginPeriod() + ::timeEndPeriod();
    分解能： 2 ミリ秒程度
    ただし、GetTickCount64() での計測は注意
*/
TEST(WinSleepAndPeriod, testClock)
{   // 計測： clock()
    List<clock_t> delta_list;
    for(auto i = 0; i < 10; ++i)
    {
        clock_t t0 = clock();
        ::timeBeginPeriod(1);
        ::Sleep(1);
        ::timeEndPeriod(1);
        clock_t t1 = clock();
        auto delta = t1 - t0;
        delta_list.Push(delta);
    }
    // 分解能は 2 ミリ程度
    EXPECT_GE(delta_list.Min(), 1);
    EXPECT_LE(delta_list.Max(), 3);
}
TEST(WinSleepAndPeriod, testGetTickCount)
{   // 計測： GetTickCount64()
    List<ULONGLONG> delta_list;
    for (auto i = 0; i < 10; ++i)
    {
        ULONGLONG t0 = ::GetTickCount64();
        ::timeBeginPeriod(1);
        ::Sleep(1);
        ::timeEndPeriod(1);
        ULONGLONG t1 = ::GetTickCount64();
        auto delta = t1 - t0;
        delta_list.Push(delta);
    }
    // 最小は 0 ミリ、最大は 15 ミリ程度となってしまう
    EXPECT_EQ(delta_list.Min(), 0);
    EXPECT_GE(delta_list.Max(), 10);
}
TEST(WinSleepAndPeriod, testTimeGetTime)
{   // 計測： timeGetTime()
    List<DWORD> delta_list;
    for (auto i = 0; i < 10; ++i)
    {
        DWORD t0 = ::timeGetTime();
        ::timeBeginPeriod(1);
        ::Sleep(1);
        ::timeEndPeriod(1);
        DWORD t1 = ::timeGetTime();
        auto delta = t1 - t0;
        delta_list.Push(delta);
    }
    EXPECT_GE(delta_list.Min(), 1); // 1 ミリで計れる場合もある
    EXPECT_LE(delta_list.Max(), 3);
}
