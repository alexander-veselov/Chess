#pragma once

#include <array>
#include <assert.h>
#include <initializer_list>

namespace chess {

template <typename T, size_t MaxSize> class FakeVector {
public:
  constexpr static auto max_size = MaxSize;

  FakeVector() = default;
  FakeVector(const FakeVector& other) = default;
  FakeVector& operator=(const FakeVector& other) = default;

  FakeVector(std::initializer_list<T> values) {
    assert(values.size() <= MaxSize);
    for (const auto& value : values) {
      data_[size_++] = value;
    }
  }

  T operator[](size_t index) const {
    return data_[index];
  }

  T& operator[](size_t index) {
    return data_[index];
  }

  void push_back(T value) {
    assert(size_ < MaxSize);
    data_[size_++] = value;
  }

  size_t size() const {
    return size_;
  }

  void clear() {
    size_ = 0ULL;
  }

  bool empty() const {
    return size_ == 0ULL;
  }

  auto begin() {
    return data_.begin();
  }

  auto end() {
    return data_.begin() + size_;
  }

  auto rbegin() const {
    return data_.rbegin() + (MaxSize - size_);
  }

  auto rend() const {
    return data_.rbegin() + MaxSize;
  }

private:
  std::array<T, MaxSize> data_ = {};
  size_t size_ = 0ULL;
};

} // namespace chess