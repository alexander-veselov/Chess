#pragma once

#include "chess/core/board.h"
#include "chess/core/square.h"

#include <optional>

namespace chess {

struct State {
  Board board;
  Color turn;
  std::optional<Square> enPassant;
  bool blackLongCastleAllowed;
  bool blackShortCastleAllowed;
  bool whiteLongCastleAllowed;
  bool whiteShortCastleAllowed;
  uint32_t halfmoveClock;
  uint32_t fullmoveNumber;
};

} // namespace chess
