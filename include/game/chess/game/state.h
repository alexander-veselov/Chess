#pragma once

#include "chess/core/board.h"

#include <optional>

namespace chess {

struct State {
  Board board;
  Color turn;
  std::optional<Square> enPassant;
};

} // namespace chess
