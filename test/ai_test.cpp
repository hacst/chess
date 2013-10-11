#include <gtest/gtest.h>

TEST(WorldSanityChecks, CheckOneEqualsOne) {
    EXPECT_EQ(1,1);
}

TEST(WorldSanityChecks, CheckOneNotEqualsTwo) {
    EXPECT_NE(1,2);
}
