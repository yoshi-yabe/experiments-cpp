#include "pch.h"

#include "src/BlockingQueue.hpp"
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <format>


TEST(TestBlockingQueue, TestBasic) {
  struct Data
  {
    int num;
    std::string name;
  };

  {
    BlockingQueue<Data> q{3};

    std::thread th{[&]
    {
      for (int i = 0; i < 5; ++i)
      {
        Data data{i, std::format("item{}", i)};
        q.Push(data);
      }
    }};

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(q.GetCount(), 3);

    {
      Data data = q.Pop();
      EXPECT_EQ(data.num, 0);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(q.GetCount(), 3);
    {
      Data data = q.Pop();
      EXPECT_EQ(data.num, 1);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(q.GetCount(), 3);
    {
      Data data = q.Pop();
      EXPECT_EQ(data.num, 2);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(q.GetCount(), 2);

    th.join();
  }
  EXPECT_TRUE(true);
}

TEST(TestBlockingQueue, TestTerminate) {
    struct Data
    {
        int num;
        std::string name;
    };

    {
        BlockingQueue<Data> q{ 3 };

        std::thread th{ [&]
        {
            q.Pop();
        } };

        q.Terminate();

        th.join();
    }
    EXPECT_TRUE(true);
}
