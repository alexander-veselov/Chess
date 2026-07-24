#pragma once

#include "chess/core/board.h"
#include "chess/core/square.h"
#include "chess/core/bitboard.h"

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

  Bitboard whitePawns = 0ULL;
  Bitboard whiteKnights = 0ULL;
  Bitboard whiteBishops = 0ULL;
  Bitboard whiteRooks = 0ULL;
  Bitboard whiteQueens = 0ULL;
  Bitboard whiteKings = 0ULL;
  Bitboard blackPawns = 0ULL;
  Bitboard blackKnights = 0ULL;
  Bitboard blackBishops = 0ULL;
  Bitboard blackRooks = 0ULL;
  Bitboard blackQueens = 0ULL;
  Bitboard blackKings = 0ULL;
};

} // namespace chess
