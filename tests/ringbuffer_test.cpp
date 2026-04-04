#include <gtest/gtest.h>
#include "ringbuffer.hpp"
#include <string>
#include <thread>
#include <vector>

TEST(RingBufferTest, PushAndPop) {
    ringbuffer<int, 4> rb;
    int val = 0;

    EXPECT_TRUE(rb.push(10));
    EXPECT_TRUE(rb.push(20));
    EXPECT_TRUE(rb.push(30));

    EXPECT_TRUE(rb.pop(val));
    EXPECT_EQ(val, 10);
    EXPECT_TRUE(rb.pop(val));
    EXPECT_EQ(val, 20);
    EXPECT_TRUE(rb.pop(val));
    EXPECT_EQ(val, 30);
}

TEST(RingBufferTest, PopFromEmptyReturnsFalse) {
    ringbuffer<int, 4> rb;
    int val = 0;
    EXPECT_FALSE(rb.pop(val));
}

TEST(RingBufferTest, FullBufferReturnsFalse) {
    // Size 4 means 3 usable slots (one slot reserved to distinguish full from empty)
    ringbuffer<int, 4> rb;

    EXPECT_TRUE(rb.push(1));
    EXPECT_TRUE(rb.push(2));
    EXPECT_TRUE(rb.push(3));
    EXPECT_FALSE(rb.push(4));
}

TEST(RingBufferTest, WrapAround) {
    ringbuffer<int, 4> rb;
    int val = 0;

    EXPECT_TRUE(rb.push(1));
    EXPECT_TRUE(rb.push(2));
    EXPECT_TRUE(rb.pop(val));
    EXPECT_EQ(val, 1);

    // After popping, there's room again
    EXPECT_TRUE(rb.push(3));
    EXPECT_TRUE(rb.push(4));

    EXPECT_TRUE(rb.pop(val));
    EXPECT_EQ(val, 2);
    EXPECT_TRUE(rb.pop(val));
    EXPECT_EQ(val, 3);
    EXPECT_TRUE(rb.pop(val));
    EXPECT_EQ(val, 4);
    EXPECT_FALSE(rb.pop(val));
}

TEST(RingBufferTest, StringType) {
    ringbuffer<std::string, 8> rb;
    std::string val;

    EXPECT_TRUE(rb.push("hello"));
    EXPECT_TRUE(rb.push("world"));

    EXPECT_TRUE(rb.pop(val));
    EXPECT_EQ(val, "hello");
    EXPECT_TRUE(rb.pop(val));
    EXPECT_EQ(val, "world");
}

TEST(RingBufferTest, ConcurrentPushPop) {
    ringbuffer<int, 64> rb;
    const int count = 1000;
    std::vector<int> results;
    results.reserve(count);

    std::thread producer([&]() {
        for (int i = 0; i < count; ++i) {
            while (!rb.push(i)) {}
        }
    });

    std::thread consumer([&]() {
        int val;
        for (int i = 0; i < count; ++i) {
            while (!rb.pop(val)) {}
            results.push_back(val);
        }
    });

    producer.join();
    consumer.join();

    ASSERT_EQ(results.size(), count);
    for (int i = 0; i < count; ++i) {
        EXPECT_EQ(results[i], i);
    }
}
