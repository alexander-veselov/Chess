#pragma once

#include "chess/core/status.h"
#include "chess/core/state.h"
#include "chess/core/square.h"
#include "chess/core/moves.h"
#include "chess/core/move.h"

namespace chess {

struct Undo {
  Piece fromPiece;
  Piece toPiece;
  U16 halfmoveClock;
  U16 fullmoveNumber;
  Color turn;
  Square enPassant;
  CastlingRightsMask castlingRightsMask;
  Hash hash;
};

Status GetStatus(const State& state);
bool Is50MoveRuleDraw(const State& state);
bool IsThreefoldRepetition(const State& state);
bool IsInCheck(const State& state, Color turn);
void GetCaptures(const State& state, Moves& legalMoves);
void GetLegalMoves(const State& state, Moves& legalMoves);
void MakeMove(State& state, Move move);
void MakeMove(State& state, Move move, Undo& undo);
void UndoMove(State& state, Move move, const Undo& undo);

} // namespace chess