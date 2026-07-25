#pragma once

#include "chess/core/move.h"
#include "chess/core/moves.h"
#include "chess/core/piece.h"
#include "chess/core/square.h"
#include "chess/core/state.h"
#include "chess/core/bitboard.h"

#include <vector>

namespace chess {

enum class Status { kWhiteToMove, kBlackToMove, kWhiteWon, kBlackWon, kDraw };

State CreateDefaultState();
bool IsValidSquare(int rank, int file);
Square ShiftSquare(Square square, int rankShift, int fileShift);
void EnPassantCapture(Board& board, Square from, Square to);
Square EvaluateEnPassant(const Board& board, Square from, Square to);
void GetKingMovesWithoutCastling(const State& state, Square square, Moves& moves);
bool IsAttacked(const State& state, Color turn, Bitboard target);
bool IsInCheck(const State& state, Color turn);
bool CanMoveInTurn(const State& state, Square square);
void UpdateCastlingState(State& state, const Move& move);
void MakeMove(State& state, const Move& move);
void GetLegalMoves(const State& state, Square square, Moves& legalMoves);
void GetAllLegalMoves(const State& state, Moves& legalMoves);
bool CanMove(const State& state, Square square);
bool HasAvailableMoves(const State& state);
Status GetStatus(const State& state);
bool LegalMove(State& state, const Move& move);

class Game {
public:
  Game();
  Game(const State& state);

  const State& GetState() const;
  const Status GetStatus() const;
  bool CanMove(Square square) const;
  bool IsInCheck() const;
  bool MakeMove(const Move& move);
  Moves GetLegalMoves(Square square) const;

private:
  State state_;
};

} // namespace chess