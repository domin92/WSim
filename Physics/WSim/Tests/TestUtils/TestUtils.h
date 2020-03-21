#pragma once

#include <gtest/gtest.h>

#define EXPECT_MEM_EQ(expectedMem, actualMem, size) EXPECT_EQ(0, memcmp((expectedMem), (actualMem), size))
#define ASSERT_MEM_EQ(expectedMem, actualMem, size) ASSERT_EQ(0, memcmp((expectedMem), (actualMem), size))
