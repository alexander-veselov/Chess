#pragma once

#include "chess/core/bitboard.h"
#include "chess/core/board.h"
#include "chess/core/castling_rights.h"
#include "chess/core/color.h"
#include "chess/core/hash.h"
#include "chess/core/history.h"
#include "chess/core/square.h"
#include "chess/core/types.h"

#include <array>
#include <vector>

namespace chess {

struct State {
  Board board;
  std::array<Bitboard, static_cast<size_t>(Piece::kPieceCount)> bitboards;

  U16 halfmoveClock;
  U16 fullmoveNumber;

  Color turn;
  Square enPassant;
  CastlingRightsMask castlingRightsMask;

  Hash hash;
  History history;
};

constexpr bool operator==(const State& state1, const State& state2) {
  return state1.board == state2.board &&
         state1.bitboards == state2.bitboards &&
         state1.halfmoveClock == state2.halfmoveClock &&
         state1.fullmoveNumber == state2.fullmoveNumber &&
         state1.turn == state2.turn &&
         state1.enPassant == state2.enPassant &&
         state1.castlingRightsMask == state2.castlingRightsMask && 
         state1.hash == state2.hash &&
         state1.history == state2.history;
}

constexpr auto kNullState =
    State{Board{}, {}, U16{0}, U16{0}, Color::kWhite, Square::kInvalid, CastlingRightsMask{0}, Hash{0}, History{}};

constexpr void FillBitboardsFromBoard(State& state) {
  state.bitboards = {};
  for (auto square = 0; square < Square::kSquareCount; ++square) {
    state.bitboards[static_cast<size_t>(state.board[square])] |= BBFromSquare(static_cast<Square>(square));
  }
}

} // namespace chess
