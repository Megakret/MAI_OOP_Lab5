#include <algorithm>

#include <gtest/gtest.h>

#include <vector/vector.hpp>

TEST(BidirectionalIteration, ForwardIteration) {
  vector::Vector<int> v{1, 2, 3};
  auto it = v.CBegin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(it, v.CEnd());
}
TEST(BidirectionalIteration, BackwardIteration) {
  vector::Vector<int> v{1, 2, 3};
  auto it = v.CEnd();
  --it;
  EXPECT_EQ(*it, 3);
  --it;
  EXPECT_EQ(*it, 2);
  --it;
  EXPECT_EQ(*it, 1);
}
TEST(RandomAccessIteration, RandomAccessIteration) {
  vector::Vector<int> v{1, 2, 3, 4, 5};
  auto it = v.CBegin();
  EXPECT_EQ(it[2], 3);
  EXPECT_EQ(*(it + 2), 3);
  EXPECT_EQ(*(2 + it), 3);
  it += 2;
  EXPECT_EQ(*(it - 2), 1);
  auto it_begin = v.CBegin();
  EXPECT_EQ(it - it_begin, 2);
}
TEST(StdAlgorithms, Sorting) {
  vector::Vector<int> v{3, 10, 0, -1, 5};
  std::sort(v.Begin(), v.End());
  EXPECT_EQ(v[0], -1);
  EXPECT_EQ(v[1], 0);
  EXPECT_EQ(v[2], 3);
  EXPECT_EQ(v[3], 5);
  EXPECT_EQ(v[4], 10);
}
