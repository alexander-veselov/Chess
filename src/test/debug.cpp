#include "gtest/gtest.h"

#include "chess/core/fen.h"
#include "chess/core/game.h"
#include "chess/core/move.h"
#include "chess/core/perft.h"
#include "chess/test/stockfish.h"

static std::map<std::string, uint64_t>
StorckfishDivide(const std::string& fen, const std::vector<chess::Move>& moves, uint32_t depth) {
  auto stockfish = chess::test::Stockfish("stockfish.exe");
  stockfish.Position(fen.data(), moves);
  return stockfish.Perft(depth);
}

static chess::Move MoveFromString(const std::string& string) {
  auto move = chess::Move{};
  chess::ParseMove(string, move);
  return move;
}

TEST(Chess, DISABLED_Debug) {
  constexpr auto kDepth = 1;
  constexpr auto fen = std::string_view{"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"};

  auto moves = std::vector<chess::Move>{};
  moves.push_back(MoveFromString("g2g4"));
  moves.push_back(MoveFromString("h4g3"));
  moves.push_back(MoveFromString("e2e4"));

  auto state = chess::StateFromFEN(fen.data());
  for (const auto& move : moves) {
    MakeMove(state, move);
  }

  const auto localPerft = chess::Divide(state, kDepth);
  const auto stockfishPerft = StorckfishDivide(fen.data(), moves, kDepth);

  for (const auto [move, count] : stockfishPerft) {
    auto it = localPerft.find(move);
    const auto found = it != localPerft.end();
    ASSERT_TRUE(found) << "Move: " << move;
    EXPECT_EQ(it->second, count) << "Move: " << move;
  }
}