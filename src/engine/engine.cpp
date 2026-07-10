#include "chess/engine/engine.h"
#include "chess/core/game.h"

namespace chess {

static float_t EvaluateBasePiece(BasePiece basePiece) {
  switch (basePiece) {
  case BasePiece::kPawn:
    return 1;
  case BasePiece::kBishop:
  case BasePiece::kKnight:
    return 3;
  case BasePiece::kRook:
    return 5;
  case BasePiece::kQueen:
    return 9;
  }
  return 0.f;
}

static float_t EvaluatePiece(Piece piece) {
  const auto baseValue = EvaluateBasePiece(GetBasePiece(piece));
  return GetPieceColor(piece) == Color::kWhite ? +baseValue : -baseValue;
}

static float_t EvaluateBoard(const State& state) {
  auto value = 0.f;
  for (const auto& row : state.board) {
    for (const auto piece : row) {
      value += EvaluatePiece(piece);
    }
  }
  return value;
}

static float_t EvaluateState(const State& state) {
  const auto status = GetStatus(state);
  switch (status) {
  case Status::kWhiteToMove:
  case Status::kBlackToMove:
    return EvaluateBoard(state);
  case Status::kWhiteWon:
    return +999.f;
  case Status::kBlackWon:
    return -999.f;
  case Status::kDraw:
    return 0.f;
  }
  return 0.f;
}

static float_t Minimax(const State& state, uint32_t depth) {
  if (depth == 0) {
    return EvaluateState(state);
  }
  auto value = 0.f;
  auto moves = std::vector<Move>{};
  GetAllLegalMoves(state, moves);
  if (state.turn == Color::kWhite) {
    value = -999.f;
    for (const auto& move : moves) {
      auto childState = State{state};
      MakeMove(childState, move);
      value = std::max(value, Minimax(childState, depth - 1));
    }
  } else {
    value = +999.f;
    for (const auto& move : moves) {
      auto childState = State{state};
      MakeMove(childState, move);
      value = std::min(value, Minimax(childState, depth - 1));
    }
  }
  return value;
}

Move BestMove(const State& state) {
  auto bestValue = 0.f;
  auto bestMove = Move{};
  auto moves = std::vector<Move>{};
  GetAllLegalMoves(state, moves);
  if (state.turn == Color::kWhite) {
    bestValue = -999.f;
    for (const auto& move : moves) {
      auto childState = State{state};
      MakeMove(childState, move);
      auto newValue = Minimax(childState, 3);
      if (newValue > bestValue) {
        bestValue = newValue;
        bestMove = move;
      }
    }
  } else {
    bestValue = +999.f;
    for (const auto& move : moves) {
      auto childState = State{state};
      MakeMove(childState, move);
      auto newValue = Minimax(childState, 3);
      if (newValue < bestValue) {
        bestValue = newValue;
        bestMove = move;
      }
    }
  }
  return bestMove;
}

} // namespace chess