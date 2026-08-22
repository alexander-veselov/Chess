#pragma once

#include "chess/core/hash.h"

#include <vector>

namespace chess {

class History : public std::vector<Hash> {
public:
  History() {
    reserve(kDefaultReserveSize);
  }

  History(const History& other)
    : std::vector<Hash>(other) {
    reserve(kDefaultReserveSize);
  }

  History(History&& other) noexcept
    : std::vector<Hash>(std::move(other)) {
    reserve(kDefaultReserveSize);
  }

  History& operator=(const History&) = default;
  History& operator=(History&&) noexcept = default;

private:
  constexpr static auto kDefaultReserveSize = 128;
};

} // namespace chess