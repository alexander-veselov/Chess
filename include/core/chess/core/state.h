#pragma once

#include "chess/core/bitboard.h"
#include "chess/core/board.h"
#include "chess/core/castling_rights.h"
#include "chess/core/color.h"
#include "chess/core/square.h"
#include "chess/core/types.h"

#include <array>

namespace chess {

struct State {
  Board board;
  std::array<Bitboard, static_cast<size_t>(Piece::kPieceCount)> bitboards;

  U16 halfmoveClock;
  U16 fullmoveNumber;

  Color turn;
  Square enPassant;
  CastlingRightsMask castlingRightsMask;
};

constexpr void FillBitboardsFromBoard(State& state) {
  state.bitboards = {};
  for (auto square = 0; square < Square::kSquareCount; ++square) {
    state.bitboards[static_cast<size_t>(state.board[square])] |= BBFromSquare(static_cast<Square>(square));
  }
}

} // namespace chess
