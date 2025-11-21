#include <gtest/gtest.h>

#include <allocators/dynamic_allocator.hpp>
#include <vector/vector.hpp>

class VectorInt : public ::testing::Test {
protected:
  allocators::DynamicMemoryResource allocator_;
  vector::Vector<int, std::pmr::polymorphic_allocator<int>> v;
  VectorInt()
      : v(vector::Vector<int, std::pmr::polymorphic_allocator<int>>(
            0, std::pmr::polymorphic_allocator<int>(&allocator_))) {}
};
class VectorStruct : public ::testing::Test {
protected:
  struct SomeStruct {
    int x;
    double y;
  };
  friend bool operator==(const SomeStruct &a, const SomeStruct &b) {
    return a.x == b.x && a.y == b.y;
  }
  allocators::DynamicMemoryResource allocator_;
  vector::Vector<SomeStruct, std::pmr::polymorphic_allocator<SomeStruct>> v;
  VectorStruct()
      : v(vector::Vector<SomeStruct,
                         std::pmr::polymorphic_allocator<SomeStruct>>(
            0, &allocator_)) {}
};
TEST_F(VectorInt, PushBack) {
  for (int i = 0; i < 100; ++i) {
    v.PushBack(i);
  }
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(v[i], i);
  }
}
TEST_F(VectorInt, CopyingVector) {
  for (int i = 0; i < 100; ++i) {
    v.PushBack(i);
  }
  auto v_copy = v;
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(v_copy[i], i);
  }
}
TEST_F(VectorInt, MovingVector) {
  for (int i = 0; i < 100; ++i) {
    v.PushBack(i);
  }
  auto v_moved = std::move(v);
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(v_moved[i], i);
  }
}
TEST_F(VectorStruct, PushBack) {
  for (int i = 0; i < 100; ++i) {
    SomeStruct obj{i, static_cast<double>(i)};
    v.PushBack(obj);
  }
  for (int i = 0; i < 100; ++i) {
    SomeStruct obj{i, static_cast<double>(i)};
    EXPECT_EQ(v[i], obj);
  }
}
TEST_F(VectorStruct, CopyingVector) {
  for (int i = 0; i < 100; ++i) {
    SomeStruct obj{i, static_cast<double>(i)};
    v.PushBack(obj);
  }
  auto v_copy = v;
  for (int i = 0; i < 100; ++i) {
    SomeStruct obj{i, static_cast<double>(i)};
    EXPECT_EQ(v_copy[i], obj);
  }
}
TEST_F(VectorStruct, MovingVector) {
  for (int i = 0; i < 100; ++i) {
    SomeStruct obj{i, static_cast<double>(i)};
    v.PushBack(obj);
  }
  auto v_moved = std::move(v);
  for (int i = 0; i < 100; ++i) {
    SomeStruct obj{i, static_cast<double>(i)};
    EXPECT_EQ(v_moved[i], obj);
  }
}
