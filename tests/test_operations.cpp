#include <gtest/gtest.h>
#include <shitcask/shitcask.h>

TEST(TestOperations, TestSetGet) {
  Shitcask shitcask{"TestSetGet"};

  const auto check_100x = [&shitcask](auto &key, auto &val) {
    shitcask.set(key, val);
    for (int i = 0; i < 100; i++) {
      EXPECT_EQ(shitcask.get(key), val);
    }
  };

  check_100x("shit", "cask");
  check_100x("shit", "ksac");
}

TEST(TestOperations, TestErase) {
  Shitcask shitcask{"TestErase"};
  shitcask.set("shit", "cask");
  EXPECT_EQ(shitcask.erase("shit"), true);
  EXPECT_EQ(shitcask.erase("shit"), false);
  EXPECT_EQ(shitcask.get("shit"), "");
}
