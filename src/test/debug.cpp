#include "gtest/gtest.h"

#include "chess/core/fen.h"
#include "chess/core/move.h"
#include "chess/core/perft.h"
#include "chess/core/random.h"
#include "chess/test/stockfish.h"

namespace {

chess::Stockfish& Stockfish() {
  static auto stockfish = chess::Stockfish("stockfish.exe");
  return stockfish;
}

std::map<std::string, U64>
StorckfishDivide(const std::string& fen, const std::vector<chess::Move>& moves, U32 depth) {
  Stockfish().Position(fen.data(), moves);
  return Stockfish().Perft(depth);
}

std::string StorckfishFEN() {
  return Stockfish().FEN();
}

chess::Move MoveFromString(const std::string& string) {
  auto move = chess::Move{};
  chess::ParseMove(string, move);
  return move;
}

chess::State GenerateRandomState(const std::string& fen, std::vector<chess::Move>& moves) {
  auto state = chess::StateFromFEN(fen);
  const auto moveCount = chess::RandomU32(0, 200);
  for (auto i = 0; i < moveCount; ++i) {
    auto legalMoves = chess::Moves{};
    chess::GetLegalMoves(state, legalMoves);
    if (legalMoves.empty()) {
      return state;
    }
    const auto move = legalMoves[chess::RandomU32(0, legalMoves.size() - 1)];
    moves.push_back(move);
    chess::MakeMove(state, move);
  }
  return state;
}

} // namespace

TEST(Chess, DISABLED_Debug) {
  constexpr auto kDepth = 1;
  constexpr auto fen = std::string_view{"2rk1bn1/p1p1p1p1/1Q1p1p2/5Pp1/PP4P1/4q3/3Bb3/2R1KB1r w K - 0 1"};

  auto moves = std::vector<chess::Move>{};
  moves.push_back(MoveFromString("b6e3"));
  moves.push_back(MoveFromString("e2a6"));
  //moves.push_back(MoveFromString("e1c1"));

  auto state = chess::StateFromFEN(fen.data());
  for (const auto& move : moves) {
    MakeMove(state, move);
  }

  const auto localPerft = chess::Divide(state, kDepth);
  const auto stockfishPerft = StorckfishDivide(fen.data(), moves, kDepth);

  auto localCount = 0ULL;
  for (const auto& [move, count] : localPerft) {
    localCount += count;
  }

  auto stockfishCount = 0ULL;
  for (const auto& [move, count] : stockfishPerft) {
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
  constexpr auto kPositions = 1000;
  constexpr auto kDepth = 4;
  constexpr auto kFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  for (auto i = 0; i < kPositions; ++i) {
    printf("%d/%d\n", i + 1, kPositions);
    auto moves = std::vector<chess::Move>{};
    const auto state = GenerateRandomState(kFen, moves);
    const auto localFen = chess::StateToFEN(state);
    const auto localPerft = chess::Divide(state, kDepth);
    const auto stockfishPerft = StorckfishDivide(kFen, moves, kDepth);
    const auto stockfishFen = StorckfishFEN();
    EXPECT_EQ(localFen, stockfishFen);
    EXPECT_EQ(localPerft, stockfishPerft) << localFen;
  }
}