#include "chess/application/chess.h"

#include "chess/core/game.h"
#include "chess/core/fen.h"

namespace chess {
namespace {

bool CanMoveInTurn(const State& state, Square square) {
  return GetPieceColor(state.board[square]) == state.turn;
}

bool LegalMove(State& state, Move move) {
  if (GetFrom(move) == GetTo(move) || !CanMoveInTurn(state, GetFrom(move))) {
    return false;
  }
  auto legalMoves = Moves{};
  GetLegalMoves(state, legalMoves);
  for (const auto legalMove : legalMoves) {
    if (legalMove == move) {
      MakeMove(state, legalMove);
      return true;
    }
  }
  return false;
}

} // namespace

Chess::Chess()
  : Chess{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"} {
}

Chess::Chess(std::string_view fen)
  : Chess::Chess{chess::StateFromFEN(fen)} {
}

Chess::Chess(const State& state)
  : state_{state} {
}

const State& Chess::GetState() const {
  return state_;
}

const Status Chess::GetStatus() const {
  return chess::GetStatus(state_);
}

bool Chess::IsInCheck() const {
  return chess::IsInCheck(state_, state_.turn);
}

bool Chess::MakeMove(Move move) {
  return chess::LegalMove(state_, move);
}

Moves Chess::GetLegalMoves(Square square) const {
  // TODO: improve that function
  auto movesForSquare = Moves{};
  auto legalMoves = Moves{};
  chess::GetLegalMoves(state_, legalMoves);
  for (const auto move : legalMoves) {
    if (GetFrom(move) == square) {
      movesForSquare.push_back(move);
    }
  }
  return movesForSquare;
}

} // namespace chess