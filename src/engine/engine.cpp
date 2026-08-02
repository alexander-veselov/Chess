#include "chess/engine/engine.h"
#include "chess/core/game.h"

namespace chess {
namespace {

constexpr auto kMaxScore = 999.f;

float_t EvaluateBasePiece(BasePiece basePiece) {
  switch (basePiece) {
  case BasePiece::kPawn:
    return 1.f;
  case BasePiece::kBishop:
  case BasePiece::kKnight:
    return 3.f;
  case BasePiece::kRook:
    return 5.f;
  case BasePiece::kQueen:
    return 9.f;
  }
  return 0.f;
}

float_t EvaluatePiece(Piece piece) {
  const auto baseValue = EvaluateBasePiece(GetBasePiece(piece));
  return GetPieceColor(piece) == Color::kWhite ? +baseValue : -baseValue;
}

float_t EvaluateBoard(const Board& board) {
  auto value = 0.f;
  for (const auto& piece : board) {
    value += EvaluatePiece(piece);
  }
  return value;
}

float_t GameOverScore(Status status) {
  switch (status) {
  case Status::kWhiteWon:
    return +kMaxScore;
  case Status::kBlackWon:
    return -kMaxScore;
  }
  return 0.f;
}

float_t ScorePenalty(float_t score, U32 depth, U32 maxDepth) {
  if (score == 0.f) {
    return 0.f;
  }
  const auto penalty = static_cast<float_t>(maxDepth) - static_cast<float_t>(depth);
  return score > 0.f ? -penalty : +penalty;
}

float_t EvaluateState(const State& state, Status status, U32 depth, U32 maxDepth) {
  auto score = 0.f;
  if (IsGameOver(status)) {
    score = GameOverScore(status);
    score += ScorePenalty(score, depth, maxDepth);
  } else {
    score = EvaluateBoard(state.board);
  }
  return score;
}

float_t Quiesce(const State& state, float_t alpha, float_t beta, U32 depth, U32 maxDepth) {
  const auto status = GetStatus(state);
  auto value = EvaluateState(state, status, depth, maxDepth);
  if (depth == 0 || IsGameOver(status)) {
    return value;
  }
  auto moves = Moves{};
  GetAllLegalMoves(state, moves);
  if (state.turn == Color::kWhite) {
    for (const auto& move : moves) {
      if (state.board[GetTo(move)] == Piece::kNone) {
        continue;
      }
      auto childState = State{state};
      MakeMove(childState, move);
      value = std::max(value, Quiesce(childState, alpha, beta, depth - 1, maxDepth));
      if (value >= beta) {
        break;
      }
      alpha = std::max(alpha, value);
    }
  } else {
    for (const auto& move : moves) {
      if (state.board[GetTo(move)] == Piece::kNone) {
        continue;
      }
      auto childState = State{state};
      MakeMove(childState, move);
      value = std::min(value, Quiesce(childState, alpha, beta, depth - 1, maxDepth));
      if (value <= alpha) {
        break;
      }
      beta = std::min(beta, value);
    }
  }
  return value;
}

float_t Minimax(const State& state, float_t alpha, float_t beta, U32 depth, U32 maxDepth) {
  const auto status = GetStatus(state);
  if (IsGameOver(status)) {
    return EvaluateState(state, status, depth, maxDepth);
  }
  if (depth == 0) {
    return Quiesce(state, alpha, beta, maxDepth, maxDepth);
  }
  auto value = 0.f;
  auto moves = Moves{};
  GetAllLegalMoves(state, moves);
  if (state.turn == Color::kWhite) {
    value = -kMaxScore;
    for (const auto& move : moves) {
      auto childState = State{state};
      MakeMove(childState, move);
      value = std::max(value, Minimax(childState, alpha, beta, depth - 1, maxDepth));
      if (value >= beta) {
        break;
      }
      alpha = std::max(alpha, value);
    }
  } else {
    value = +kMaxScore;
    for (const auto& move : moves) {
      auto childState = State{state};
      MakeMove(childState, move);
      value = std::min(value, Minimax(childState, alpha, beta, depth - 1, maxDepth));
      if (value <= alpha) {
        break;
      }
      beta = std::min(beta, value);
    }
  }
  return value;
}

} // namespace

Move BestMove(const State& state, U32 depth) {
  auto bestValue = 0.f;
  auto bestMove = Move{};
  auto moves = Moves{};
  auto alpha = -kMaxScore;
  auto beta = +kMaxScore;
  GetAllLegalMoves(state, moves);
  if (!moves.empty()) {
    bestMove = moves[0];
  }
  if (state.turn == Color::kWhite) {
    bestValue = -kMaxScore;
    for (const auto& move : moves) {
      auto childState = State{state};
      MakeMove(childState, move);
      auto newValue = Minimax(childState, alpha, beta, depth - 1, depth - 1);
      if (newValue > bestValue) {
        bestValue = newValue;
        bestMove = move;
      }
      if (newValue >= beta) {
        break;
      }
      alpha = std::max(alpha, newValue);
    }
  } else {
    bestValue = +kMaxScore;
    for (const auto& move : moves) {
      auto childState = State{state};
      MakeMove(childState, move);
      auto newValue = Minimax(childState, alpha, beta, depth - 1, depth - 1);
      if (newValue < bestValue) {
        bestValue = newValue;
        bestMove = move;
      }
      if (newValue <= alpha) {
        break;
      }
      beta = std::min(beta, newValue);
    }
  }
  return bestMove;
}

} // namespace chess