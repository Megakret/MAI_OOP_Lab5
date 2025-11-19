#pragma once

#include <memory>

#include <vector/vector.hpp>
#include <vector/vector_exceptions.hpp>

namespace vector {
template <typename T, typename Allocator>
Vector<T, Allocator>::Vector()
    : size_(0), capacity_(10), allocator_(Allocator()),
      arr_(std::allocator_traits<Allocator>::allocate(allocator_, capacity_)) {}
template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(std::size_t size, const Allocator &allocator)
  requires std::is_default_constructible_v<T>
    : size_(size), capacity_(size), allocator_(allocator),
      arr_(std::allocator_traits<Allocator>::allocate(allocator_, capacity_)) {
  for (std::size_t i = 0; i < size; ++i) {
    std::allocator_traits<Allocator>::construct(allocator_, arr_ + i);
  }
}
template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(std::size_t size, const T &value,
                             const Allocator &allocator)
  requires std::copy_constructible<T>
    : size_(size), capacity_(size), allocator_(allocator),
      arr_(std::allocator_traits<Allocator>::allocate(allocator_, capacity_)) {
  for (std::size_t i = 0; i < size; ++i) {
    std::allocator_traits<Allocator>::construct(allocator_, arr_ + i, value);
  }
}
template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(const std::initializer_list<T> &list,
                             const Allocator &allocator)
    : size_(list.size()), capacity_(list.size()), allocator_(allocator),
      arr_(std::allocator_traits<Allocator>::allocate(allocator_, capacity_)) {
  std::size_t i = 0;
  for (const T &val : list) {
    std::allocator_traits<Allocator>::construct(allocator_, arr_ + i, val);
    ++i;
  }
}
template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Vector<T, Allocator> &vec)
  requires std::copy_constructible<T>
    : size_(vec.size_), capacity_(vec.capacity_),
      allocator_(std::allocator_traits<
                 Allocator>::select_on_container_copy_construction(vec.allocator_)),
      arr_(std::allocator_traits<Allocator>::allocate(allocator_, capacity_)) {
  for (std::size_t i = 0; i < vec.size_; ++i) {
    std::allocator_traits<Allocator>::construct(allocator_, arr_ + i,
                                                vec.arr_[i]);
  }
}
template <typename T, typename Allocator>
void Vector<T, Allocator>::Swap(Vector<T, Allocator> &vec) noexcept {
  std::swap(arr_, vec.arr_);
  std::swap(size_, vec.size_);
  std::swap(capacity_, vec.capacity_);
  if constexpr (std::allocator_traits<
                    Allocator>::propagate_on_container_swap::value) {
    std::swap(allocator_, vec.allocator_);
  }
}
template <typename T, typename Allocator>
Vector<T, Allocator> &
Vector<T, Allocator>::operator=(const Vector<T, Allocator> &vec)
  requires std::copy_constructible<T>
{
  if (this == &vec) {
    return *this;
  }
  for (std::size_t i = 0; i < vec.size_; ++i) {
    std::allocator_traits<Allocator>::destroy(allocator_, arr_ + i);
  }
  std::allocator_traits<Allocator>::deallocate(allocator_, arr_, capacity_);
  if constexpr (std::allocator_traits<
                    Allocator>::propagate_on_container_copy_assignment::value) {
    if (allocator_ != vec.allocator_) {
      allocator_ = std::allocator_traits<
          Allocator>::select_on_container_copy_construction(vec.allocator_);
    }
  }
  arr_ = std::allocator_traits<Allocator>::allocate(allocator_, vec.capacity_);
  size_ = vec.size_;
  capacity_ = vec.capacity_;
  for (std::size_t i = 0; i < vec.size_; ++i) {
    std::allocator_traits<Allocator>::construct(allocator_, arr_ + i,
                                                vec.arr_[i]);
  }
  return *this;
}
template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(Vector<T, Allocator> &&vec) noexcept
    : size_(vec.size_), capacity_(vec.capacity_),
      allocator_(std::move(vec.allocator_)), arr_(vec.arr_) {
  vec.arr_ = nullptr;
  vec.size_ = 0;
  vec.capacity_ = 0;
}
template <typename T, typename Allocator>
Vector<T, Allocator> &
Vector<T, Allocator>::operator=(Vector<T, Allocator> &&vec) noexcept {
  if (this == &vec) {
    return *this;
  }
  for (std::size_t i = 0; i < vec.size_; ++i) {
    std::allocator_traits<Allocator>::destroy(allocator_, arr_ + i);
  }
  std::allocator_traits<Allocator>::deallocate(allocator_, arr_, capacity_);
  if constexpr (std::allocator_traits<
                    Allocator>::propagate_on_container_copy_assignment::value) {
    allocator_ = std::move(vec.allocator_);
  }
  size_ = vec.size_;
  capacity_ = vec.capacity_;
  arr_ = vec.arr_;
  vec.arr_ = nullptr;
  vec.size_ = 0;
  vec.capacity_ = 0;
  return *this;
}
template <typename T, typename Allocator> Vector<T, Allocator>::~Vector() {
  for (std::size_t i = 0; i < size_; ++i) {
    std::allocator_traits<Allocator>::destroy(allocator_, arr_ + i);
  }
  std::allocator_traits<Allocator>::deallocate(allocator_, arr_, capacity_);
}
template <typename T, typename Allocator>
T &Vector<T, Allocator>::operator[](std::size_t idx) noexcept {
  return arr_[idx];
}
template <typename T, typename Allocator>
const T &Vector<T, Allocator>::operator[](std::size_t idx) const noexcept {
  return arr_[idx];
}
template <typename T, typename Allocator>
T &Vector<T, Allocator>::At(std::size_t idx) {
  if (idx >= size_) {
    throw vector::OutOfBounds();
  }
  return arr_[idx];
}
template <typename T, typename Allocator>
const T &Vector<T, Allocator>::At(std::size_t idx) const {
  if (idx >= size_) {
    throw vector::OutOfBounds();
  }
  return arr_[idx];
}
template <typename T, typename Allocator>
void Vector<T, Allocator>::Reserve(std::size_t new_capacity)
  requires Escapable<T>
{
  if (capacity_ < new_capacity) {
    ReserveInternal(new_capacity);
  }
}
template <typename T, typename Allocator>
void Vector<T, Allocator>::ReserveInternal(std::size_t new_capacity) {
  T *new_arr =
      std::allocator_traits<Allocator>::allocate(allocator_, new_capacity);
  for (std::size_t i = 0; i < size_; ++i) {
    std::allocator_traits<Allocator>::construct(allocator_, new_arr + i,
                                                std::move_if_noexcept(arr_[i]));
  }
  for (std::size_t i = 0; i < size_; ++i) {
    std::allocator_traits<Allocator>::destroy(allocator_, arr_ + i);
  }
  std::allocator_traits<Allocator>::deallocate(allocator_, arr_, capacity_);
  arr_ = new_arr;
  capacity_ = new_capacity;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::PushBack(T &&value)
  requires std::move_constructible<T>
{
  PushBackInternal(std::move(value));
}
template <typename T, typename Allocator>
void Vector<T, Allocator>::PushBack(const T &value)
  requires std::copy_constructible<T>
{
  PushBackInternal(value);
}
template <typename T, typename Allocator>
template <typename U>
void Vector<T, Allocator>::PushBackInternal(U &&value) {
  if (size_ >= capacity_) {
    ReserveInternal(capacity_ * 2);
  }
  std::allocator_traits<Allocator>::construct(allocator_, arr_ + size_,
                                              std::forward<U>(value));
  ++size_;
}
template <typename T, typename Allocator>
void Vector<T, Allocator>::PopBack() noexcept {
  std::allocator_traits<Allocator>::destroy(allocator_, arr_ + size_);
  --size_;
}
template <typename T, typename Allocator>
T &Vector<T, Allocator>::Front() noexcept {
  return arr_[0];
}
template <typename T, typename Allocator>
T &Vector<T, Allocator>::Back() noexcept {
  return arr_[size_ - 1];
}
template <typename T, typename Allocator>
const T &Vector<T, Allocator>::Front() const noexcept {
  return arr_[0];
}
template <typename T, typename Allocator>
const T &Vector<T, Allocator>::Back() const noexcept {
  return arr_[size_ - 1];
}
template <typename T, typename Allocator>
T *Vector<T, Allocator>::Data() noexcept {
  return arr_;
}
template <typename T, typename Allocator>
std::size_t Vector<T, Allocator>::Size() const noexcept {
  return size_;
}
template <typename T, typename Allocator>
std::size_t Vector<T, Allocator>::Capacity() const noexcept {
  return capacity_;
}
template <typename T, typename Allocator>
void Vector<T, Allocator>::Delete(std::size_t idx)
  requires Escapable<T>
{
  if (idx >= size_) {
    throw vector::OutOfBounds();
  }
  for (std::size_t i = idx; i < size_ - 1; ++i) {
    arr_[i] = std::move_if_noexcept(arr_[i + 1]);
  }
  --size_;
}
template <typename T, typename Allocator>
void Vector<T, Allocator>::Insert(std::size_t idx, const T &value)
  requires Escapable<T>
{
  InsertInternal(idx, value);
}
template <typename T, typename Allocator>
void Vector<T, Allocator>::Insert(std::size_t idx, T &&value)
  requires Escapable<T>
{
  InsertInternal(idx, std::move(value));
}

template <typename T, typename Allocator>
template <typename U>
void Vector<T, Allocator>::InsertInternal(std::size_t idx, U &&value) {
  if (idx > size_) {
    throw vector::OutOfBounds();
  }
  if (size_ == capacity_) {
    ReserveInternal(capacity_ * 2);
  }
  std::allocator_traits<Allocator>::construct(
      allocator_, arr_ + size_, std::move_if_noexcept(arr_[size_ - 1]));
  for (std::size_t i = size_ - 1; i > idx; --i) {
    arr_[i] = std::move_if_noexcept(arr_[i - 1]);
  }
  arr_[idx] = std::forward<U>(value);
  ++size_;
}
template <typename T, typename Allocator>
void swap(Vector<T, Allocator> &a, Vector<T, Allocator> &b) {
  a.Swap(b);
}
}; // namespace vector
