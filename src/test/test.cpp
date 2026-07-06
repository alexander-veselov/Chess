#include "gtest/gtest.h"

#include "chess/core/game.h"
#include "chess/core/move.h"
#include "chess/core/fen.h"

namespace chess {
namespace test {

static uint64_t GetAllLegalMoves(const State& state, std::vector<Move>& legalMoves) {
  for (auto rank = 0; rank < kBoardSize; ++rank) {
    for (auto file = 0; file < kBoardSize; ++file) {
      if (GetPieceColor(state.board[rank][file]) == state.turn) {
        GetLegalMoves(state, Square{static_cast<Rank>(rank), static_cast<File>(file)}, legalMoves);
      }
    }
  }
  return legalMoves.size();
}

static uint64_t Perft(const State& state, int32_t depth) {
  auto moves = std::vector<Move>{};
  const auto nMoves = GetAllLegalMoves(state, moves);

  if (depth == 1) {
    return nMoves;
  }

  auto nodes = uint64_t{0};
  for (const auto& move : moves) {
    auto newState = State{state};
    MakeMove(newState, move);
    nodes += Perft(newState, depth - 1);
  }

  return nodes;
}

} // namespace test
} // namespace chess

TEST(Chess, Perft1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 8;
  const auto state = chess::StateFromFEN("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2");
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}