#pragma once

#include "chess/core/bitboard.h"
#include "chess/core/board.h"
#include "chess/core/square.h"

#include <array>

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

  std::array<Bitboard, static_cast<size_t>(Piece::kPieceCount)> bitboards;
};

constexpr void FillBitboardsFromBoard(State& state) {
  state.bitboards = {};
  for (auto square = 0; square < Square::kSquareCount; ++square) {
    state.bitboards[static_cast<size_t>(state.board[square])] |= BBFromSquare(static_cast<Square>(square));
  }
}

} // namespace chess
