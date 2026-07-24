#pragma once

#include "chess/core/board.h"
#include "chess/core/square.h"

namespace chess {

struct State {
  Board board;
  Color turn;
  Square enPassant;
  bool blackLongCastleAllowed;
  bool blackShortCastleAllowed;
  bool whiteLongCastleAllowed;
  bool whiteShortCastleAllowed;
  U32 halfmoveClock;
  U32 fullmoveNumber;
};

} // namespace chess
