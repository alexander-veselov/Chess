#pragma once

#include "chess/core/move.h"

#include <array>

namespace chess {

class Moves {
private:
  constexpr static auto kMaxMoves = 256;
  using Container = std::array<Move, kMaxMoves>;
  Container moves_ = {};
  size_t moveCount_ = 0ULL;

public:
  Moves() = default;
  Moves(const Moves& moves) = default;
  Moves& operator=(const Moves& moves) = default;

  Move operator[](size_t index) const;

  size_t size() const;
  bool empty() const;

  void push_back(Move move);

  Container::const_iterator begin() const;
  Container::const_iterator end() const;
};

} // namespace chess