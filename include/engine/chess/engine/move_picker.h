#pragma once

#include "chess/core/board.h"
#include "chess/core/move.h"
#include "chess/core/moves.h"

#include <optional>

namespace chess {

class MovePicker {

public:
  MovePicker(Moves& moves, const Board& board, Move ttMove);
  size_t Next();

private:
  Moves& moves_;
  const Board board_;
  Move ttMove_;
  size_t next_;
};

}