#pragma once

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>

namespace vector {
template <typename T>
concept Escapable =
    (std::is_nothrow_move_constructible_v<T> || std::copy_constructible<T>);

template <typename T, typename Allocator = std::pmr::polymorphic_allocator<T>>
class Vector {
private:
  std::size_t size_;
  std::size_t capacity_;
  Allocator allocator_;
  T *arr_;

public:
  Vector();
  explicit Vector(std::size_t size, const Allocator &allocator = Allocator())
    requires std::is_default_constructible_v<T>;
  Vector(std::size_t size, const T &value,
         const Allocator &allocator = Allocator())
    requires std::copy_constructible<T>;
  Vector(const std::initializer_list<T> &,
         const Allocator &allocator = Allocator());
  Vector(const Vector &)
    requires std::copy_constructible<T>;
  Vector &operator=(const Vector<T, Allocator> &)
    requires std::copy_constructible<T>;
  Vector(Vector<T, Allocator> &&) noexcept;
  Vector &operator=(Vector<T, Allocator> &&) noexcept;
  ~Vector();
  void Swap(Vector<T, Allocator> &) noexcept;
  T &operator[](std::size_t idx) noexcept;
  const T &operator[](std::size_t idx) const noexcept;
  T &At(std::size_t idx);
  const T &At(std::size_t idx) const;
  void PushBack(T &&value)
    requires std::move_constructible<T>;
  void PushBack(const T &value)
    requires std::copy_constructible<T>;
  void PopBack() noexcept;
  std::size_t Size() const noexcept;
  std::size_t Capacity() const noexcept;
  void Delete(std::size_t idx)
    requires Escapable<T>;
  void Insert(std::size_t idx, const T &value)
    requires Escapable<T>;
  void Insert(std::size_t idx, T &&value)
    requires Escapable<T>;
  void Reserve(std::size_t new_capacity)
    requires Escapable<T>;
  T *Data() noexcept;
  T &Front() noexcept;
  T &Back() noexcept;
  const T &Front() const noexcept;
  const T &Back() const noexcept;

private:
  void ReserveInternal(std::size_t new_capacity);
  template <typename U> void PushBackInternal(U &&value);
  template <typename U> void InsertInternal(std::size_t idx, U &&value);

private:
  template <bool IsConst> class Iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer_type = std::conditional_t<IsConst, const T *, T *>;
    using reference_type = std::conditional_t<IsConst, const T &, T &>;
    Iterator() : ptr_(nullptr) {}
    Iterator(T *ptr) : ptr_(ptr) {}
    Iterator(const Iterator &other) : ptr_(other.ptr_) {}
    Iterator &operator=(const Iterator &other) {
      ptr_ = other.ptr_;
      return (*this);
    }
    reference_type operator*() const { return *ptr_; }
    pointer_type operator->() { return ptr_; }
    Iterator &operator++() {
      ptr_++;
      return *this;
    }
    Iterator operator++(int) {
      Iterator copy = *this;
      ++copy;
      return copy;
    }
    Iterator &operator--() {
      ptr_--;
      return *this;
    }
    Iterator operator--(int) {
      Iterator copy(*this);
      --copy;
      return copy;
    }
    friend bool operator==(const Iterator a, const Iterator b) {
      return a.ptr_ == b.ptr_;
    }
    friend bool operator!=(const Iterator a, const Iterator b) {
      return !(a == b);
    }
    Iterator &operator+=(const difference_type diff) {
      ptr_ += diff;
      return *this;
    }
    friend Iterator operator+(const Iterator a, const difference_type diff) {
      Iterator copy(a);
      copy += diff;
      return copy;
    }
    friend Iterator operator+(const difference_type diff, const Iterator b) {
      return b + diff;
    }
    Iterator &operator-=(const difference_type diff) {
      ptr_ -= diff;
      return *this;
    }
    friend Iterator operator-(const Iterator a, difference_type diff) {
      Iterator copy(a);
      copy -= diff;
      return copy;
    }
    difference_type operator-(const Iterator other) const {
      return ptr_ - other.ptr_;
    }
    reference_type operator[](const difference_type diff) const {
      return ptr_[diff];
    }
    friend bool operator<(const Iterator a, const Iterator b) {
      return a.ptr_ < b.ptr_;
    }
    friend bool operator>(const Iterator a, const Iterator b) {
      return a.ptr_ > b.ptr_;
    }
    friend bool operator<=(const Iterator a, const Iterator b) {
      return !(a > b);
    }
    friend bool operator>=(const Iterator a, const Iterator b) {
      return !(a < b);
    }

  private:
    T *ptr_;
  };

public:
  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;
  iterator Begin() { return Iterator<false>(arr_); }
  iterator End() { return Iterator<false>(arr_ + size_); }
  const_iterator CBegin() const { return Iterator<true>(arr_); }
  const_iterator CEnd() const { return Iterator<true>(arr_ + size_); }
};
static_assert(std::random_access_iterator<Vector<int>::iterator>);
}; // namespace vector
#include <vector/vector.ipp>
