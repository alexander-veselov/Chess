#pragma once

#include "chess/core/board.h"

namespace chess {

struct State {
  Board board;
  Color turn;
};

} // namespace chess
