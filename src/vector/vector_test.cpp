#include <string>

#include <gtest/gtest.h>

#include <vector/vector.ipp>

class VectorTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

struct TestType {
  int value;
  TestType() : value(0) {}
  TestType(int v) : value(v) {}
  bool operator==(const TestType &other) const { return value == other.value; }
};
template class vector::Vector<TestType>;

struct TrackedType {
  static int constructor_count;
  static int destructor_count;
  static int copy_count;
  static int move_count;
  int value;

  TrackedType() : value(0) { constructor_count++; }
  TrackedType(int v) : value(v) { constructor_count++; }
  TrackedType(const TrackedType &other) : value(other.value) {
    copy_count++;
    constructor_count++;
  }
  TrackedType(TrackedType &&other) noexcept : value(other.value) {
    move_count++;
    constructor_count++;
    other.value = -1;
  }
  TrackedType &operator=(const TrackedType &other) {
    value = other.value;
    copy_count++;
    return *this;
  }
  TrackedType &operator=(TrackedType &&other) noexcept {
    value = other.value;
    move_count++;
    other.value = -1;
    return *this;
  }
  ~TrackedType() { destructor_count++; }

  static void reset_counts() {
    constructor_count = destructor_count = copy_count = move_count = 0;
  }

  bool operator==(const TrackedType &other) const {
    return value == other.value;
  }
};

int TrackedType::constructor_count = 0;
int TrackedType::destructor_count = 0;
int TrackedType::copy_count = 0;
int TrackedType::move_count = 0;

// Tests for default constructor
TEST_F(VectorTest, DefaultConstructor) {
  vector::Vector<int> vec;
  EXPECT_EQ(vec.Size(), 0);
}

TEST_F(VectorTest, SizeConstructor) {
  vector::Vector<int> vec(5);
  EXPECT_EQ(vec.Size(), 5);
  for (std::size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(vec[i], 0);
  }
}

TEST_F(VectorTest, SizeValueConstructor) {
  vector::Vector<int> vec(3, 42);
  EXPECT_EQ(vec.Size(), 3);
  for (std::size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(vec[i], 42);
  }
}

TEST_F(VectorTest, InitializerListConstructor) {
  vector::Vector<int> vec{1, 2, 3, 4, 5};
  EXPECT_EQ(vec.Size(), 5);
  for (std::size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(vec[i], static_cast<int>(i + 1));
  }
}

TEST_F(VectorTest, CopyConstructor) {
  vector::Vector<int> original{1, 2, 3};
  vector::Vector<int> copy(original);
  EXPECT_EQ(original.Size(), 3);
  EXPECT_EQ(copy.Size(), 3);
  for (std::size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(copy[i], original[i]);
  }
}
TEST_F(VectorTest, MoveConstructor) {
  vector::Vector<int> original{1, 2, 3};
  vector::Vector<int> moved(std::move(original));
  EXPECT_EQ(moved.Size(), 3);
  for (std::size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(moved[i], static_cast<int>(i + 1));
  }
}

TEST_F(VectorTest, CopyAssignment) {
  vector::Vector<int> original{1, 2, 3};
  vector::Vector<int> copy;
  copy = original;
  EXPECT_EQ(original.Size(), 3);
  EXPECT_EQ(copy.Size(), 3);
  for (std::size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(copy[i], original[i]);
  }
}

TEST_F(VectorTest, MoveAssignment) {
  vector::Vector<int> original{1, 2, 3};
  vector::Vector<int> moved;
  moved = std::move(original);
  EXPECT_EQ(moved.Size(), 3);
  for (std::size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(moved[i], static_cast<int>(i + 1));
  }
}

TEST_F(VectorTest, SubscriptOperator) {
  vector::Vector<int> vec{10, 20, 30};
  EXPECT_EQ(vec[0], 10);
  EXPECT_EQ(vec[1], 20);
  EXPECT_EQ(vec[2], 30);
  vec[1] = 25;
  EXPECT_EQ(vec[1], 25);
}

TEST_F(VectorTest, AtMethod) {
  vector::Vector<int> vec{10, 20, 30};
  EXPECT_EQ(vec.At(0), 10);
  EXPECT_EQ(vec.At(1), 20);
  EXPECT_EQ(vec.At(2), 30);
  vec.At(1) = 25;
  EXPECT_EQ(vec.At(1), 25);
}

TEST_F(VectorTest, AtMethodOutOfRange) {
  vector::Vector<int> vec{10, 20, 30};
  EXPECT_THROW(vec.At(3), vector::OutOfBounds);
  EXPECT_THROW(vec.At(100), vector::OutOfBounds);
}

TEST_F(VectorTest, ConstAtMethod) {
  const vector::Vector<int> vec{10, 20, 30};
  EXPECT_EQ(vec.At(0), 10);
  EXPECT_EQ(vec.At(1), 20);
  EXPECT_EQ(vec.At(2), 30);
}

TEST_F(VectorTest, PushBack) {
  vector::Vector<int> vec;
  vec.PushBack(1);
  EXPECT_EQ(vec.Size(), 1);
  EXPECT_EQ(vec[0], 1);
  vec.PushBack(2);
  EXPECT_EQ(vec.Size(), 2);
  EXPECT_EQ(vec[1], 2);
  vec.PushBack(3);
  EXPECT_EQ(vec.Size(), 3);
  EXPECT_EQ(vec[2], 3);
}

TEST_F(VectorTest, PushBackWithReallocation) {
  vector::Vector<int> vec;
  for (int i = 0; i < 20; ++i) {
    vec.PushBack(i);
  }
  EXPECT_EQ(vec.Size(), 20);
  for (int i = 0; i < 20; ++i) {
    EXPECT_EQ(vec[i], i);
  }
}

TEST_F(VectorTest, PopBack) {
  vector::Vector<int> vec{1, 2, 3};
  vec.PopBack();
  EXPECT_EQ(vec.Size(), 2);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  vec.PopBack();
  EXPECT_EQ(vec.Size(), 1);
  EXPECT_EQ(vec[0], 1);
  vec.PopBack();
  EXPECT_EQ(vec.Size(), 0);
}

TEST_F(VectorTest, Delete) {
  vector::Vector<int> vec{1, 2, 3, 4, 5};
  vec.Delete(2);
  EXPECT_EQ(vec.Size(), 4);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 4);
  EXPECT_EQ(vec[3], 5);
  vec.Delete(0);
  EXPECT_EQ(vec.Size(), 3);
  EXPECT_EQ(vec[0], 2);
  EXPECT_EQ(vec[1], 4);
  EXPECT_EQ(vec[2], 5);
  vec.Delete(2);
  EXPECT_EQ(vec.Size(), 2);
  EXPECT_EQ(vec[0], 2);
  EXPECT_EQ(vec[1], 4);
}

TEST_F(VectorTest, Insert) {
  vector::Vector<int> vec{1, 3, 4};
  vec.Insert(1, 2);
  EXPECT_EQ(vec.Size(), 4);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 3);
  EXPECT_EQ(vec[3], 4);
  vec.Insert(0, 0);
  EXPECT_EQ(vec.Size(), 5);
  EXPECT_EQ(vec[0], 0);
  EXPECT_EQ(vec[1], 1);
  vec.Insert(5, 5);
  EXPECT_EQ(vec.Size(), 6);
  EXPECT_EQ(vec[5], 5);
}

TEST_F(VectorTest, Reserve) {
  vector::Vector<int> vec{1, 2, 3};
  vec.Reserve(10);
  EXPECT_GE(vec.Size(), 3); // Size should remain the same
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 3);
  vec.PushBack(4);
  EXPECT_EQ(vec.Size(), 4);
  EXPECT_EQ(vec[3], 4);
}

TEST_F(VectorTest, FrontAndBack) {
  vector::Vector<int> vec{1, 2, 3, 4, 5};
  EXPECT_EQ(vec.Front(), 1);
  EXPECT_EQ(vec.Back(), 5);
  vec.Front() = 10;
  vec.Back() = 50;
  EXPECT_EQ(vec.Front(), 10);
  EXPECT_EQ(vec.Back(), 50);
}

TEST_F(VectorTest, Data) {
  vector::Vector<int> vec{1, 2, 3};

  auto data = vec.Data();
  EXPECT_NE(data, nullptr);
  EXPECT_EQ(data[0], 1);
  EXPECT_EQ(data[1], 2);
  EXPECT_EQ(data[2], 3);
}

TEST_F(VectorTest, Swap) {
  vector::Vector<int> vec1{1, 2, 3};
  vector::Vector<int> vec2{4, 5, 6, 7};

  vec1.Swap(vec2);

  EXPECT_EQ(vec1.Size(), 4);
  EXPECT_EQ(vec2.Size(), 3);

  EXPECT_EQ(vec1[0], 4);
  EXPECT_EQ(vec1[3], 7);
  EXPECT_EQ(vec2[0], 1);
  EXPECT_EQ(vec2[2], 3);
}

// Tests with custom types
TEST_F(VectorTest, CustomType) {
  vector::Vector<TestType> vec;
  vec.PushBack(TestType(1));
  vec.PushBack(TestType(2));
  vec.PushBack(TestType(3));

  EXPECT_EQ(vec.Size(), 3);
  EXPECT_EQ(vec[0].value, 1);
  EXPECT_EQ(vec[1].value, 2);
  EXPECT_EQ(vec[2].value, 3);
}

TEST_F(VectorTest, StringType) {
  vector::Vector<std::string> vec;
  vec.PushBack("hello");
  vec.PushBack("world");

  EXPECT_EQ(vec.Size(), 2);
  EXPECT_EQ(vec[0], "hello");
  EXPECT_EQ(vec[1], "world");
}

TEST_F(VectorTest, ResourceManagement) {
  TrackedType::reset_counts();
  {
    vector::Vector<TrackedType> vec;
    vec.PushBack(TrackedType(1));
    vec.PushBack(TrackedType(2));
    vec.PushBack(TrackedType(3));

    EXPECT_EQ(vec.Size(), 3);
  }
  EXPECT_EQ(TrackedType::constructor_count, TrackedType::destructor_count);
}

TEST_F(VectorTest, MoveSemantics) {
  TrackedType::reset_counts();
  {
    vector::Vector<TrackedType> vec1;
    vec1.PushBack(TrackedType(1));
    vec1.PushBack(TrackedType(2));
    vector::Vector<TrackedType> vec2 = std::move(vec1);
    EXPECT_EQ(vec2.Size(), 2);
  }
  EXPECT_EQ(TrackedType::constructor_count, TrackedType::destructor_count);
}

// Edge cases
TEST_F(VectorTest, SelfAssignment) {
  vector::Vector<int> vec{1, 2, 3};
  vec = vec;
  EXPECT_EQ(vec.Size(), 3);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 3);
  vec = std::move(vec);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
