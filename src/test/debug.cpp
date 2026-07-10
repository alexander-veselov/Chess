#include "gtest/gtest.h"

#include "chess/core/fen.h"
#include "chess/core/game.h"
#include "chess/core/move.h"
#include "chess/core/perft.h"
#include "chess/test/stockfish.h"

#include <random>

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

uint32_t GenerateRandomNumber(uint32_t min, uint32_t max) {
  static auto rng = std::mt19937{std::random_device{}()};
  auto dist = std::uniform_int_distribution<uint32_t>{min, max};
  return dist(rng);
}

static chess::State GenerateRandomState() {
  auto state = chess::StateFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  const auto moveCount = GenerateRandomNumber(0, 200);
  for (auto i = 0; i < moveCount; ++i) {
    auto legalMoves = std::vector<chess::Move>{};
    chess::GetAllLegalMoves(state, legalMoves);
    if (legalMoves.empty()) {
      return state;
    }
    const auto move = legalMoves[GenerateRandomNumber(0, legalMoves.size() - 1)];
    chess::MakeMove(state, move);
  }
  return state;
}

TEST(Chess, DISABLED_Debug) {
  constexpr auto kDepth = 6;
  constexpr auto fen = std::string_view{"8/3pp3/5k2/8/8/5K2/3PP3/8 w - - 0 1"};

  auto moves = std::vector<chess::Move>{};
  //moves.push_back(MoveFromString("g2g4"));
  //moves.push_back(MoveFromString("h4g3"));
  //moves.push_back(MoveFromString("e2e4"));

  auto state = chess::StateFromFEN(fen.data());
  for (const auto& move : moves) {
    MakeMove(state, move);
  }

  const auto localPerft = chess::Divide(state, kDepth);
  const auto stockfishPerft = StorckfishDivide(fen.data(), moves, kDepth);

  auto localCount = 0ull;
  for (const auto [move, count] : localPerft) {
    localCount += count;
  }

  auto stockfishCount = 0ull;
  for (const auto [move, count] : stockfishPerft) {
    stockfishCount += count;
  }

  EXPECT_EQ(localCount, stockfishCount);

  for (const auto [move, count] : stockfishPerft) {
    auto it = localPerft.find(move);
    const auto found = it != localPerft.end();
    ASSERT_TRUE(found) << "Move: " << move;
    EXPECT_EQ(it->second, count) << "Move: " << move;
  }
}

TEST(Chess, DISABLED_CompareRandomPosition) {
  constexpr auto kPositions = 100;
  constexpr auto kDepth = 1;
  for (auto i = 0; i < kPositions; ++i) {
    printf("%d/%d\n", i + 1, kPositions);
    const auto state = GenerateRandomState();
    const auto fen = chess::FENFromState(state);
    const auto localPerft = chess::Divide(state, kDepth);
    const auto stockfishPerft = StorckfishDivide(fen, {}, kDepth);
    EXPECT_EQ(localPerft, stockfishPerft) << fen;
  }
}