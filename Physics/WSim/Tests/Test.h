#pragma once

#include <cmath>
#include <gtest/gtest.h>
#include <iostream>

#define EXPECT_MEM_EQ(expectedMem, actualMem, size) EXPECT_EQ(0, memcmp((expectedMem), (actualMem), size))
#define ASSERT_MEM_EQ(expectedMem, actualMem, size) ASSERT_EQ(0, memcmp((expectedMem), (actualMem), size))

#define EXPECT_FLOATS_EQ(expectedFloats, actualFloats, count, epsilon)        \
    {                                                                         \
        for (auto i = 0u; i < count; i++) {                                   \
            const auto diff = std::fabs(expectedFloats[i] - actualFloats[i]); \
            if (diff > epsilon) {                                             \
                std::cerr << "Float index = " << i << "\n";                   \
                EXPECT_LE(diff, epsilon);                                     \
                break;                                                        \
            }                                                                 \
        }                                                                     \
    }
