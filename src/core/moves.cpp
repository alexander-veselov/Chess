#include "chess/core/moves.h"

#include <assert.h>

namespace chess {

Move Moves::operator[](size_t index) const {
  return moves_[index];
}

void Moves::push_back(Move move) {
  assert(moveCount_ < kMaxMoves);
  moves_[moveCount_++] = move;
}

size_t Moves::size() const {
  return moveCount_;
}

bool Moves::empty() const {
  return moveCount_ == 0ULL;
}

Moves::Container::const_iterator Moves::begin() const {
  return moves_.begin();
}

Moves::Container::const_iterator Moves::end() const {
  return moves_.begin() + moveCount_;
}

} // namespace chess