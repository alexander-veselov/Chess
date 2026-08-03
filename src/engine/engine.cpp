#include "chess/engine/engine.h"

#include "chess/core/bits.h"
#include "chess/core/game.h"
#include "chess/core/types.h"

#include <algorithm>
#include <array>

namespace chess {
namespace {

using Score = I32;
constexpr auto kMaxScore = Score(999);

constexpr auto kPieceValues = [] {
  std::array<Score, kPieceCount> values{};
  values[kNone]         =  0;
  values[kWhiteKing]    =  0;
  values[kWhiteQueen]   = +9;
  values[kWhiteRook]    = +5;
  values[kWhiteBishop]  = +3;
  values[kWhiteKnight]  = +3;
  values[kWhitePawn]    = +1;
  values[kBlackKing]    =  0;
  values[kBlackQueen]   = -9;
  values[kBlackRook]    = -5;
  values[kBlackBishop]  = -3;
  values[kBlackKnight]  = -3;
  values[kBlackPawn]    = -1;
  return values;
}();

Score EvaluatePiece(Piece piece, Bitboard bitboard) {
  const auto baseValue = kPieceValues[piece];
  const auto pieceCount = PopCount(bitboard);
  const auto value = baseValue * pieceCount;
  return value;
}

Score EvaluateBoard(const State& state) {
  auto value = 0;
  for (auto pieceIndex = 1; pieceIndex < kPieceCount; ++pieceIndex) {
    const auto piece = static_cast<Piece>(pieceIndex);
    value += EvaluatePiece(piece, state.bitboards[piece]);
  }
  return value;
}

Score GameOverScore(Status status) {
  switch (status) {
  case Status::kWhiteWon:
    return +kMaxScore;
  case Status::kBlackWon:
    return -kMaxScore;
  }
  return 0;
}

Score ScorePenalty(Score score, U32 depth, U32 maxDepth) {
  if (score == 0) {
    return 0;
  }
  const auto penalty = static_cast<Score>(maxDepth) - static_cast<Score>(depth);
  return score > 0 ? -penalty : +penalty;
}

Score EvaluateState(const State& state, Status status, U32 depth, U32 maxDepth) {
  auto score = 0;
  if (IsGameOver(status)) {
    score = GameOverScore(status);
    score += ScorePenalty(score, depth, maxDepth);
  } else {
    score = EvaluateBoard(state);
  }
  return state.turn == Color::kWhite ? score : -score;
}

Score Quiesce(const State& state, Score alpha, Score beta, U32 depth, U32 maxDepth) {
  const auto status = GetStatus(state);
  const auto staticEval = EvaluateState(state, status, depth, maxDepth);
  if (depth == 0 || IsGameOver(status)) {
    return staticEval;
  }
  if (staticEval >= beta) {
    return beta;
  }
  alpha = std::max(alpha, staticEval);

  auto value = staticEval;
  auto moves = Moves{};
  GetLegalMoves(state, moves);
  for (const auto& move : moves) {
    if (state.board[GetTo(move)] == Piece::kNone) {
      continue;
    }
    auto childState = State{state};
    MakeMove(childState, move);
    value = std::max(value, -Quiesce(childState, -beta, -alpha, depth - 1, maxDepth));
    if (value >= beta) {
      return beta;
    }
    alpha = std::max(alpha, value);
  }
  return value;
}

void OrderMoves(const State& state, Moves& moves) {
  std::sort(moves.begin(), moves.end(), [&state](Move move1, Move move2) {
    const auto capture1 = state.board[GetTo(move1)] != Piece::kNone;
    const auto capture2 = state.board[GetTo(move2)] != Piece::kNone;
    return capture1 > capture2;
  });
}

Score Negamax(const State& state, Score alpha, Score beta, U32 depth, U32 maxDepth) {
  const auto status = GetStatus(state);
  if (IsGameOver(status)) {
    return EvaluateState(state, status, depth, maxDepth);
  }
  if (depth == 0) {
    return Quiesce(state, alpha, beta, maxDepth, maxDepth);
  }
  auto moves = Moves{};
  GetLegalMoves(state, moves);
  OrderMoves(state, moves);
  auto value = Score(-kMaxScore);
  for (const auto& move : moves) {
    auto childState = State{state};
    MakeMove(childState, move);
    value = std::max(value, -Negamax(childState, -beta, -alpha, depth - 1, maxDepth));
    alpha = std::max(alpha, value);
    if (alpha >= beta) {
      break;
    }
  }
  return value;
}

} // namespace

Move BestMove(const State& state, U32 depth) {
  auto moves = Moves{};
  GetLegalMoves(state, moves);
  OrderMoves(state, moves);
  auto bestMove = moves.empty() ? Move{} : moves[0];
  auto bestValue = -kMaxScore;
  auto alpha = -kMaxScore;
  auto beta = kMaxScore;
  for (const auto& move : moves) {
    auto childState = State{state};
    MakeMove(childState, move);
    auto newValue = -Negamax(childState, -beta, -alpha, depth - 1, depth - 1);
    if (newValue > bestValue) {
      bestValue = newValue;
      bestMove = move;
    }
    alpha = std::max(alpha, newValue);
    if (alpha >= beta) {
      break;
    }
  }
  return bestMove;
}

} // namespace chess