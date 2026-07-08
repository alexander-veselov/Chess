#include <gtest/gtest.h>

#include "chess/core/fen.h"
#include "chess/core/state.h"

#include "chess/test/perft.h"

namespace {
constexpr auto kPosition1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr auto kPosition2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
constexpr auto kPosition3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ";
constexpr auto kPosition4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
constexpr auto kPosition5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ";
constexpr auto kPosition6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ";
}

TEST(Chess, FEN_Empty) {
  const auto state = chess::StateFromFEN("8/8/8/8/8/8/8/8 w - - 0 0");
  EXPECT_EQ(state.enPassant, std::nullopt);
  EXPECT_EQ(state.turn, chess::Color::kWhite);
  EXPECT_EQ(state.whiteShortCastleAllowed, false);
  EXPECT_EQ(state.whiteLongCastleAllowed, false);
  EXPECT_EQ(state.blackShortCastleAllowed, false);
  EXPECT_EQ(state.blackLongCastleAllowed, false);
  EXPECT_EQ(state.halfmoveClock, 0);
  EXPECT_EQ(state.fullmoveNumber, 0);
}

TEST(Chess, FEN_Default) {
  const auto state = chess::StateFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  EXPECT_EQ(state.enPassant, std::nullopt);
  EXPECT_EQ(state.turn, chess::Color::kWhite);
  EXPECT_EQ(state.whiteShortCastleAllowed, true);
  EXPECT_EQ(state.whiteLongCastleAllowed, true);
  EXPECT_EQ(state.blackShortCastleAllowed, true);
  EXPECT_EQ(state.blackLongCastleAllowed, true);
  EXPECT_EQ(state.halfmoveClock, 0);
  EXPECT_EQ(state.fullmoveNumber, 1);
}

TEST(Chess, FEN_EnPassant) {
  const auto state = chess::StateFromFEN("8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3");
  ASSERT_NE(state.enPassant, std::nullopt);
  EXPECT_EQ(state.enPassant->file, chess::File::_D);
  EXPECT_EQ(state.enPassant->rank, chess::Rank::_3);
  EXPECT_EQ(state.turn, chess::Color::kBlack);
  EXPECT_EQ(state.whiteShortCastleAllowed, false);
  EXPECT_EQ(state.whiteLongCastleAllowed, false);
  EXPECT_EQ(state.blackShortCastleAllowed, false);
  EXPECT_EQ(state.blackLongCastleAllowed, false);
  EXPECT_EQ(state.halfmoveClock, 0);
  EXPECT_EQ(state.fullmoveNumber, 3);
}

// Position 1

TEST(Chess, PerftPosition1Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 20;
  const auto state = chess::StateFromFEN(kPosition1);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition1Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 400;
  const auto state = chess::StateFromFEN(kPosition1);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition1Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 8902;
  const auto state = chess::StateFromFEN(kPosition1);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition1Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 197281;
  const auto state = chess::StateFromFEN(kPosition1);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition1Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 4865609;
  const auto state = chess::StateFromFEN(kPosition1);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, DISABLED_PerftPosition1Depth6) {
  constexpr auto kDepth = 6;
  constexpr auto kNodes = 119060324;
  const auto state = chess::StateFromFEN(kPosition1);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

// Position 2

TEST(Chess, PerftPosition2Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 48;
  const auto state = chess::StateFromFEN(kPosition2);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition2Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 2039;
  const auto state = chess::StateFromFEN(kPosition2);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition2Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 97862;
  const auto state = chess::StateFromFEN(kPosition2);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition2Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 4085603;
  const auto state = chess::StateFromFEN(kPosition2);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, DISABLED_PerftPosition2Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 193690690;
  const auto state = chess::StateFromFEN(kPosition2);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

// Position 3

TEST(Chess, PerftPosition3Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 14;
  const auto state = chess::StateFromFEN(kPosition3);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 191;
  const auto state = chess::StateFromFEN(kPosition3);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 2812;
  const auto state = chess::StateFromFEN(kPosition3);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 43238;
  const auto state = chess::StateFromFEN(kPosition3);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 674624;
  const auto state = chess::StateFromFEN(kPosition3);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth6) {
  constexpr auto kDepth = 6;
  constexpr auto kNodes = 11030083;
  const auto state = chess::StateFromFEN(kPosition3);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, DISABLED_PerftPosition3Depth7) {
  constexpr auto kDepth = 7;
  constexpr auto kNodes = 178633661;
  const auto state = chess::StateFromFEN(kPosition3);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, DISABLED_PerftPosition3Depth8) {
  constexpr auto kDepth = 8;
  constexpr auto kNodes = 3009794393;
  const auto state = chess::StateFromFEN(kPosition3);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

// Position 4

TEST(Chess, PerftPosition4Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 6;
  const auto state = chess::StateFromFEN(kPosition4);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition4Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 264;
  const auto state = chess::StateFromFEN(kPosition4);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition4Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 9467;
  const auto state = chess::StateFromFEN(kPosition4);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition4Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 422333;
  const auto state = chess::StateFromFEN(kPosition4);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition4Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 15833292;
  const auto state = chess::StateFromFEN(kPosition4);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, DISABLED_PerftPosition4Depth6) {
  constexpr auto kDepth = 6;
  constexpr auto kNodes = 706045033;
  const auto state = chess::StateFromFEN(kPosition4);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

// Position 5

TEST(Chess, PerftPosition5Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 44;
  const auto state = chess::StateFromFEN(kPosition5);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition5Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 1486;
  const auto state = chess::StateFromFEN(kPosition5);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition5Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 62379;
  const auto state = chess::StateFromFEN(kPosition5);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition5Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 2103487;
  const auto state = chess::StateFromFEN(kPosition5);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, DISABLED_PerftPosition5Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 89941194;
  const auto state = chess::StateFromFEN(kPosition5);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

// Position 6

TEST(Chess, PerftPosition6Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 46;
  const auto state = chess::StateFromFEN(kPosition6);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition6Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 2079;
  const auto state = chess::StateFromFEN(kPosition6);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition6Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 89890;
  const auto state = chess::StateFromFEN(kPosition6);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition6Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 3894594;
  const auto state = chess::StateFromFEN(kPosition6);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, DISABLED_PerftPosition6Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 164075551;
  const auto state = chess::StateFromFEN(kPosition6);
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}