#include "gtest/gtest.h"

#include "chess/core/fen.h"
#include "chess/core/state.h"

#include "chess/test/perft.h"

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

TEST(Chess, Perft1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 8;
  const auto state = chess::StateFromFEN("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2");
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, Perft2) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 8;
  const auto state = chess::StateFromFEN("8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3");
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, Perft3) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 19;
  const auto state = chess::StateFromFEN("r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2");
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, Perft4) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 2812;
  const auto state = chess::StateFromFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, DISABLED_Perft999) {
  // Still fails
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 193690690;
  const auto state =
      chess::StateFromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}