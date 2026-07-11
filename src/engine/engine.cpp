#include "chess/engine/engine.h"
#include "chess/core/game.h"

namespace {

constexpr auto kMaxScore = 999.f;

}

namespace chess {

static float_t EvaluateBasePiece(BasePiece basePiece) {
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

static float_t EvaluatePiece(Piece piece) {
  const auto baseValue = EvaluateBasePiece(GetBasePiece(piece));
  return GetPieceColor(piece) == Color::kWhite ? +baseValue : -baseValue;
}

static float_t EvaluateBoard(const Board& board) {
  auto value = 0.f;
  for (const auto& piece : board) {
    value += EvaluatePiece(piece);
  }
  return value;
}

static bool IsGameOver(Status status) {
  switch (status) {
  case Status::kWhiteToMove:
  case Status::kBlackToMove:
    return false;
  case Status::kWhiteWon:
  case Status::kBlackWon:
  case Status::kDraw:
    return true;
  }
  return true;
}

static float_t GameOverScore(Status status) {
  switch (status) {
  case Status::kWhiteWon:
    return +kMaxScore;
  case Status::kBlackWon:
    return -kMaxScore;
  }
  return 0.f;
}

static float_t ScorePenalty(float_t score, uint32_t depth, uint32_t maxDepth) {
  const auto penalty = static_cast<float_t>(maxDepth) - static_cast<float_t>(depth);
  return score > 0.f ? -penalty : +penalty;
}

static float_t EvaluateState(const State& state, Status status, uint32_t depth, uint32_t maxDepth) {
  auto score = 0.f;
  if (IsGameOver(status)) {
    score = GameOverScore(status);
    score += ScorePenalty(score, depth, maxDepth);
  } else {
    score = EvaluateBoard(state.board);
  }
  return score;
}

static float_t DebugMinimax(const State& state, float_t alpha, float_t beta, uint32_t depth,
                       uint32_t maxDepth, std::vector<Move>& outMoves) {
  const auto status = GetStatus(state);
  if (depth == 0 || IsGameOver(status)) {
    return EvaluateState(state, status, depth, maxDepth);
  }
  auto moves = std::vector<Move>{};
  GetAllLegalMoves(state, moves);
  auto bestValue = 0.f;
  auto bestMove = moves[0]; // Should be safe because of IsGameOver check
  auto bestMoves = std::vector<Move>{};
  if (state.turn == Color::kWhite) {
    bestValue = -kMaxScore;
    for (const auto& move : moves) {
      auto childState = State{state};
      MakeMove(childState, move);
      auto currentBestMoves = std::vector<Move>{};
      const auto value =
          DebugMinimax(childState, alpha, beta, depth - 1, maxDepth, currentBestMoves);
      if (value > bestValue) {
        bestValue = value;
        bestMove = move;
        bestMoves = currentBestMoves;
      }
      if (bestValue >= beta) {
        break;
      }
      alpha = std::max(alpha, bestValue);
    }
  } else {
    bestValue = +kMaxScore;
    for (const auto& move : moves) {
      auto childState = State{state};
      MakeMove(childState, move);
      auto currentBestMoves = std::vector<Move>{};
      const auto value =
          DebugMinimax(childState, alpha, beta, depth - 1, maxDepth, currentBestMoves);
      if (value < bestValue) {
        bestValue = value;
        bestMove = move;
        bestMoves = currentBestMoves;
      }
      if (bestValue <= alpha) {
        break;
      }
      beta = std::min(beta, bestValue);
    }
  }
  outMoves = bestMoves;
  outMoves.push_back(bestMove);
  return bestValue;
}

static float_t Minimax(const State& state, float_t alpha, float_t beta, uint32_t depth,
                       uint32_t maxDepth) {
  const auto status = GetStatus(state);
  if (depth == 0 || IsGameOver(status)) {
    return EvaluateState(state, status, depth, maxDepth);
  }
  auto value = 0.f;
  auto moves = std::vector<Move>{};
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

Move BestMove(const State& state, uint32_t depth) {
  auto bestValue = 0.f;
  auto bestMove = Move{};
  auto moves = std::vector<Move>{};
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