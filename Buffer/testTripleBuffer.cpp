#include "pch.h"

#include "src/TripleBuffer.hpp"
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <format>


TEST(TestTripleBuffer, TestBasic)
{
    struct Data
    {
    int num;
    std::string name;
    };

    TripleBuffer<Data> buf;

    {
        auto exclusive = buf.LockBusy();
        exclusive->data.num = 10;
        exclusive->data.name = "first";
    }
    buf.SwapBusy();

    buf.SwapPresent();
    {
        auto exclusive = buf.LockPresent();
        EXPECT_EQ(exclusive->data.num, 10);
        EXPECT_STREQ(exclusive->data.name.c_str(), "first");
    }

    {
        auto exclusive = buf.LockBusy();
        exclusive->data.num = 20;
        exclusive->data.name = "second";
    }
    // not yet SwapBusy()
    {
        auto exclusive = buf.LockPresent();
        EXPECT_EQ(exclusive->data.num, 10);
        EXPECT_STREQ(exclusive->data.name.c_str(), "first");
    }
    buf.SwapBusy();
    // not yet SwapPresent()
    {
        auto exclusive = buf.LockPresent();
        EXPECT_EQ(exclusive->data.num, 10);
        EXPECT_STREQ(exclusive->data.name.c_str(), "first");
    }
    buf.SwapPresent();
    {
        auto exclusive = buf.LockPresent();
        EXPECT_EQ(exclusive->data.num, 20);
        EXPECT_STREQ(exclusive->data.name.c_str(), "second");
    }
}
