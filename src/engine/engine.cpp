#include "chess/engine/engine.h"

#include "chess/core/bits.h"
#include "chess/core/game.h"
#include "chess/core/random.h"
#include "chess/engine/pv_table.h"

#include <algorithm>
#include <array>

namespace chess {
namespace {

constexpr auto kPieceValues = [] {
  std::array<Score, kPieceCount> values{};
  values[kNone]         =    0;
  values[kWhiteKing]    =    0;
  values[kWhiteQueen]   = +900;
  values[kWhiteRook]    = +500;
  values[kWhiteBishop]  = +325;
  values[kWhiteKnight]  = +310;
  values[kWhitePawn]    = +100;
  values[kBlackKing]    =    0;
  values[kBlackQueen]   = -900;
  values[kBlackRook]    = -500;
  values[kBlackBishop]  = -325;
  values[kBlackKnight]  = -310;
  values[kBlackPawn]    = -100;
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

Score EvaluateState(const State& state) {
  const auto score = EvaluateBoard(state);
  return state.turn == Color::kWhite ? score : -score;
}

Score Quiesce(const State& state, Score alpha, Score beta, U32 ply) {
  auto moves = Moves{};
  GetCaptures(state, moves);
  if (moves.empty()) {
    if (IsInCheck(state, state.turn)) {
      return MatedIn(ply);
    } else {
      return kDrawScore;
    }
  }
  const auto staticEval = EvaluateState(state);
  auto bestValue = staticEval;
  if (bestValue >= beta) {
    return bestValue;
  }
  if (bestValue > alpha) {
    alpha = bestValue;
  }
  for (const auto& move : moves) {
    if (state.board[GetTo(move)] == Piece::kNone) {
      continue;
    }
    auto childState = State{state};
    MakeMove(childState, move);
    const auto score = -Quiesce(childState, -beta, -alpha, ply + 1);
    if (score >= beta) {
      return score;
    }
    if (score > bestValue) {
      bestValue = score;
    }
    if (score > alpha) {
      alpha = score;
    }
  }
  return bestValue;
}

void OrderMoves(const State& state, Moves& moves) {
  std::shuffle(moves.begin(), moves.end(), GetRNG());
  std::sort(moves.begin(), moves.end(), [&state](Move move1, Move move2) {
    const auto capture1 = state.board[GetTo(move1)] != Piece::kNone;
    const auto capture2 = state.board[GetTo(move2)] != Piece::kNone;
    return capture1 > capture2;
  });
}

Score Negamax(const State& state, Score alpha, Score beta, U32 ply, U32 depth, PVTable& pvTable) {
  if (depth == 0) {
    return Quiesce(state, alpha, beta, ply);
  }
  auto moves = Moves{};
  GetLegalMoves(state, moves);
  if (moves.empty()) {
    if (IsInCheck(state, state.turn)) {
      return MatedIn(ply);
    } else {
      return kDrawScore;
    }
  }
  OrderMoves(state, moves);
  auto value = Score(-kInfinity);
  for (const auto& move : moves) {
    auto childState = State{state};
    MakeMove(childState, move);
    value = std::max(value, -Negamax(childState, -beta, -alpha, ply + 1, depth - 1, pvTable));
    if (value > alpha) {
      alpha = value;
      pvTable.Update(ply, depth, move);
    }
    if (alpha >= beta) {
      break;
    }
  }
  return value;
}

} // namespace

std::pair<std::vector<Move>, Score> BestMove(const State& state, U32 depth) {
  static auto pvTable = PVTable{};
  const auto score = Negamax(state, -kInfinity, kInfinity, 0, depth, pvTable);
  const auto lineSize = IsMateInN(score) ? GetMatePly(score) : depth;
  return {pvTable.GetLine(lineSize), score};
}

} // namespace chess