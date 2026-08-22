#pragma once

#include "chess/core/board.h"
#include "chess/core/fake_vector.h"
#include "chess/core/move.h"
#include "chess/core/moves.h"
#include "chess/engine/score.h"

#include <optional>

namespace chess {

class MovePicker {

public:
  MovePicker(Moves& moves, const Board& board, Move ttMove);
  size_t Next();

private:
  using Scores = FakeVector<Score, Moves::max_size>;
  size_t next_;
  Moves& moves_;
  Scores scores_;
};

} // namespace chess