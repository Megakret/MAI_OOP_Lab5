#pragma once

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <type_traits>

namespace vector {
template <typename T>
concept Escapable =
    (std::is_nothrow_move_constructible_v<T> || std::copy_constructible<T>);

template <typename T> class Vector {
private:
  std::size_t size_;
  std::size_t capacity_;
  T *arr_;

public:
  Vector();
  explicit Vector(std::size_t size)
    requires std::is_default_constructible_v<T>;
  Vector(std::size_t size, const T &value)
    requires std::copy_constructible<T>;
  Vector(const std::initializer_list<T> &);
  Vector(const Vector &)
    requires std::copy_constructible<T>;
  Vector &operator=(const Vector<T> &)
    requires std::copy_constructible<T>;
  Vector(Vector<T> &&) noexcept;
  Vector &operator=(Vector<T> &&) noexcept;
  ~Vector();
  void Swap(Vector<T> &) noexcept;
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
};
}; // namespace vector
#include <vector/vector.ipp>
