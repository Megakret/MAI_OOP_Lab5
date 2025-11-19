#pragma once

#include <vector/vector.hpp>
#include <vector/vector_exceptions.hpp>

namespace vector {
template <typename T>
Vector<T>::Vector()
    : size_(0), capacity_(10),
      arr_(reinterpret_cast<T *>(new char[capacity_ * sizeof(T)])) {}
template <typename T>
Vector<T>::Vector(std::size_t size)
  requires std::is_default_constructible_v<T>
    : size_(size), capacity_(size),
      arr_(reinterpret_cast<T *>(new char[capacity_ * sizeof(T)])) {
  for (std::size_t i = 0; i < size; ++i) {
    new (arr_ + i) T();
  }
}
template <typename T>
Vector<T>::Vector(std::size_t size, const T &value)
  requires std::copy_constructible<T>
    : size_(size), capacity_(size),
      arr_(reinterpret_cast<T *>(new char[capacity_ * sizeof(T)])) {
  for (std::size_t i = 0; i < size; ++i) {
    new (arr_ + i) T(value);
  }
}
template <typename T>
Vector<T>::Vector(const std::initializer_list<T> &list)
    : size_(list.size()), capacity_(list.size()),
      arr_(reinterpret_cast<T *>(new char[list.size() * sizeof(T)])) {
  std::size_t i = 0;
  for (const T &val : list) {
    new (arr_ + i) T(val);
    ++i;
  }
}
template <typename T>
Vector<T>::Vector(const Vector<T> &vec)
  requires std::copy_constructible<T>
    : size_(vec.size_), capacity_(vec.capacity_),
      arr_(reinterpret_cast<T *>(new char[vec.capacity_ * sizeof(T)])) {
  for (std::size_t i = 0; i < vec.size_; ++i) {
    new (arr_ + i) T(vec.arr_[i]);
  }
}
template <typename T> void Vector<T>::Swap(Vector<T> &vec) noexcept {
  std::swap(arr_, vec.arr_);
  std::swap(size_, vec.size_);
  std::swap(capacity_, vec.capacity_);
}
template <typename T>
Vector<T> &Vector<T>::operator=(const Vector<T> &vec)
  requires std::copy_constructible<T>
{
  Vector<T> clone(vec);
  Swap(clone);
  return *this;
}
template <typename T>
Vector<T>::Vector(Vector<T> &&vec) noexcept
    : size_(vec.size_), capacity_(vec.capacity_), arr_(vec.arr_) {
  vec.arr_ = nullptr;
  vec.size_ = 0;
}
template <typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &&vec) noexcept {
  Vector<T> moved(std::move(vec));
  Swap(moved);
  return *this;
}
template <typename T> Vector<T>::~Vector() {
  for (std::size_t i = 0; i < size_; ++i) {
    (arr_ + i)->~T();
  }
  delete[] reinterpret_cast<char *>(arr_);
}
template <typename T> T &Vector<T>::operator[](std::size_t idx) noexcept {
  return arr_[idx];
}
template <typename T>
const T &Vector<T>::operator[](std::size_t idx) const noexcept {
  return arr_[idx];
}
template <typename T> T &Vector<T>::At(std::size_t idx) {
  if (idx >= size_) {
    throw vector::OutOfBounds();
  }
  return arr_[idx];
}
template <typename T> const T &Vector<T>::At(std::size_t idx) const {
  if (idx >= size_) {
    throw vector::OutOfBounds();
  }
  return arr_[idx];
}
template <typename T>
void Vector<T>::Reserve(std::size_t new_capacity)
  requires Escapable<T>
{
  if (capacity_ < new_capacity) {
    ReserveInternal(new_capacity);
  }
}
template <typename T>
void Vector<T>::ReserveInternal(std::size_t new_capacity) {
  T *new_arr = reinterpret_cast<T *>(new char[new_capacity * sizeof(T)]);
  for (std::size_t i = 0; i < size_; ++i) {
    new (new_arr + i) T(std::move_if_noexcept(arr_[i]));
  }
  for (std::size_t i = 0; i < size_; ++i) {
    (arr_ + i)->~T();
  }
  delete[] reinterpret_cast<char *>(arr_);
  arr_ = new_arr;
  capacity_ = new_capacity;
}

template <typename T>
void Vector<T>::PushBack(T &&value)
  requires std::move_constructible<T>
{
  PushBackInternal(std::move(value));
}
template <typename T>
void Vector<T>::PushBack(const T &value)
  requires std::copy_constructible<T>
{
  PushBackInternal(value);
}
template <typename T>
template <typename U>
void Vector<T>::PushBackInternal(U &&value) {
  if (size_ >= capacity_) {
    ReserveInternal(capacity_ * 2);
  }
  new (arr_ + size_) T(std::forward<U>(value));
  ++size_;
}
template <typename T> void Vector<T>::PopBack() noexcept {
  (arr_ + size_)->~T();
  --size_;
}
template <typename T> T &Vector<T>::Front() noexcept { return arr_[0]; }
template <typename T> T &Vector<T>::Back() noexcept { return arr_[size_ - 1]; }
template <typename T> const T &Vector<T>::Front() const noexcept {
  return arr_[0];
}
template <typename T> const T &Vector<T>::Back() const noexcept {
  return arr_[size_ - 1];
}
template <typename T> T *Vector<T>::Data() noexcept { return arr_; }
template <typename T> std::size_t Vector<T>::Size() const noexcept {
  return size_;
}
template <typename T> std::size_t Vector<T>::Capacity() const noexcept {
  return capacity_;
}
template <typename T>
void Vector<T>::Delete(std::size_t idx)
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
template <typename T>
void Vector<T>::Insert(std::size_t idx, const T &value)
  requires Escapable<T>
{
  InsertInternal(idx, value);
}
template <typename T>
void Vector<T>::Insert(std::size_t idx, T &&value)
  requires Escapable<T>
{
  InsertInternal(idx, std::move(value));
}

template <typename T>
template <typename U>
void Vector<T>::InsertInternal(std::size_t idx, U &&value) {
  if (idx > size_) {
    throw vector::OutOfBounds();
  }
  if (size_ == capacity_) {
    ReserveInternal(capacity_ * 2);
  }
  new (arr_ + size_) T(std::move_if_noexcept(arr_[size_ - 1]));
  for (std::size_t i = size_ - 1; i > idx; --i) {
    arr_[i] = std::move_if_noexcept(arr_[i - 1]);
  }
  arr_[idx] = std::forward<U>(value);
  ++size_;
}
template <typename T> void swap(Vector<T> &a, Vector<T> &b) { a.Swap(b); }
}; // namespace vector
