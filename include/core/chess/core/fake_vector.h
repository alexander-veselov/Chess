#pragma once

#include <array>
#include <assert.h>

namespace chess {

template <typename T, size_t MaxSize> class FakeVector {
private:
  using Data = std::array<T, MaxSize>;
  Data data_ = {};
  size_t size_ = 0ULL;

public:
  constexpr static auto max_size = MaxSize;

  FakeVector() = default;
  FakeVector(const FakeVector& other) = default;
  FakeVector& operator=(const FakeVector& other) = default;


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

  bool empty() const {
    return size_ == 0ULL;
  }

  Data::iterator begin() {
    return data_.begin();
  }

  Data::iterator end() {
    return data_.begin() + size_;
  }
};

} // namespace chess