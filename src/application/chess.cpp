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

// TODO: add test: 5rk1/4n1p1/8/2b3Pp/2p1p3/2PnP2P/2NB2R1/3K2N1 b - - 8 28 quicence
// TODO: add test: 8/8/8/5K2/1b6/1k6/p2n4/8 w - - 0 154 mate in 6

Chess::Chess()
  : Chess{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"} {
}

Chess::Chess(std::string_view fen)
  : Chess::Chess{chess::StateFromFEN(fen)} {
}

Chess::Chess(const State& state)
  : state_{state},
    status_{chess::GetStatus(state)},
    isInCheck_{chess::IsInCheck(state, state.turn)},
    history_{} {
}

const State& Chess::GetState() const {
  return state_;
}

const Status Chess::GetStatus() const {
  return status_;
}

bool Chess::IsInCheck() const {
  return isInCheck_;
}

bool Chess::MakeMove(Move move) {
  const auto isLegal = chess::LegalMove(state_, move);
  if (isLegal) {
    history_.push_back(move);
    status_ = chess::GetStatus(state_);
    isInCheck_ = chess::IsInCheck(state_, state_.turn);
  }
  return isLegal;
}

std::vector<Move> Chess::GetLegalMoves(Square square) const {
  // TODO: improve that function
  auto movesForSquare = std::vector<Move>{};
  auto legalMoves = Moves{};
  chess::GetLegalMoves(state_, legalMoves);
  for (const auto move : legalMoves) {
    if (GetFrom(move) == square) {
      movesForSquare.push_back(move);
    }
  }
  return movesForSquare;
}

std::vector<Move> Chess::GetHistory() const {
  return history_;
}

} // namespace chess