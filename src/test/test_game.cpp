#include <gtest/gtest.h>

#include "chess/core/fen.h"
#include "chess/core/perft.h"
#include "chess/core/state.h"
#include "chess/core/zobrist_hash.h"

using namespace chess;

namespace {
constexpr auto kPosition1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr auto kPosition2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
constexpr auto kPosition3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
constexpr auto kPosition4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
constexpr auto kPosition5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
constexpr auto kPosition6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
}

TEST(Chess, FEN_Empty) {
  const auto state = StateFromFEN("8/8/8/8/8/8/8/8 w - - 0 0");
  EXPECT_EQ(state.enPassant, Square::kInvalid);
  EXPECT_EQ(state.turn, Color::kWhite);
  EXPECT_EQ(state.castlingRightsMask, 0);
  EXPECT_EQ(state.halfmoveClock, 0);
  EXPECT_EQ(state.fullmoveNumber, 0);
}

TEST(Chess, FEN_Default) {
  const auto state = StateFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  EXPECT_EQ(state.enPassant, Square::kInvalid);
  EXPECT_EQ(state.turn, Color::kWhite);
  EXPECT_EQ(state.castlingRightsMask, 0b1111);
  EXPECT_EQ(state.halfmoveClock, 0);
  EXPECT_EQ(state.fullmoveNumber, 1);
}

TEST(Chess, FEN_ViseVersa1) {
  const auto expectedFen = kPosition1;
  const auto state = StateFromFEN(expectedFen);
  const auto actualFen = StateToFEN(state);
  EXPECT_EQ(expectedFen, actualFen);
}

TEST(Chess, FEN_ViseVersa2) {
  const auto expectedFen = kPosition2;
  const auto state = StateFromFEN(expectedFen);
  const auto actualFen = StateToFEN(state);
  EXPECT_EQ(expectedFen, actualFen);
}

TEST(Chess, FEN_ViseVersa3) {
  const auto expectedFen = kPosition3;
  const auto state = StateFromFEN(expectedFen);
  const auto actualFen = StateToFEN(state);
  EXPECT_EQ(expectedFen, actualFen);
}

TEST(Chess, FEN_ViseVersa4) {
  const auto expectedFen = kPosition4;
  const auto state = StateFromFEN(expectedFen);
  const auto actualFen = StateToFEN(state);
  EXPECT_EQ(expectedFen, actualFen);
}

TEST(Chess, FEN_ViseVersa5) {
  const auto expectedFen = kPosition5;
  const auto state = StateFromFEN(expectedFen);
  const auto actualFen = StateToFEN(state);
  EXPECT_EQ(expectedFen, actualFen);
}

TEST(Chess, FEN_ViseVersa6) {
  const auto expectedFen = kPosition6;
  const auto state = StateFromFEN(expectedFen);
  const auto actualFen = StateToFEN(state);
  EXPECT_EQ(expectedFen, actualFen);
}

TEST(Chess, FEN_EnPassant) {
  const auto state = StateFromFEN("8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3");
  EXPECT_EQ(state.enPassant, D3);
  EXPECT_EQ(state.turn, Color::kBlack);
  EXPECT_EQ(state.castlingRightsMask, 0);
  EXPECT_EQ(state.halfmoveClock, 0);
  EXPECT_EQ(state.fullmoveNumber, 3);
}

// Position 1

TEST(Chess, PerftPosition1Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 20;
  const auto state = StateFromFEN(kPosition1);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition1Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 400;
  const auto state = StateFromFEN(kPosition1);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition1Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 8902;
  const auto state = StateFromFEN(kPosition1);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition1Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 197281;
  const auto state = StateFromFEN(kPosition1);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition1Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 4865609;
  const auto state = StateFromFEN(kPosition1);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition1Depth6) {
  constexpr auto kDepth = 6;
  constexpr auto kNodes = 119060324;
  const auto state = StateFromFEN(kPosition1);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

// Position 2

TEST(Chess, PerftPosition2Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 48;
  const auto state = StateFromFEN(kPosition2);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition2Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 2039;
  const auto state = StateFromFEN(kPosition2);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition2Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 97862;
  const auto state = StateFromFEN(kPosition2);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition2Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 4085603;
  const auto state = StateFromFEN(kPosition2);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition2Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 193690690;
  const auto state = StateFromFEN(kPosition2);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

// Position 3

TEST(Chess, PerftPosition3Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 14;
  const auto state = StateFromFEN(kPosition3);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 191;
  const auto state = StateFromFEN(kPosition3);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 2812;
  const auto state = StateFromFEN(kPosition3);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 43238;
  const auto state = StateFromFEN(kPosition3);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 674624;
  const auto state = StateFromFEN(kPosition3);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth6) {
  constexpr auto kDepth = 6;
  constexpr auto kNodes = 11030083;
  const auto state = StateFromFEN(kPosition3);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition3Depth7) {
  constexpr auto kDepth = 7;
  constexpr auto kNodes = 178633661;
  const auto state = StateFromFEN(kPosition3);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, DISABLED_PerftPosition3Depth8) {
  constexpr auto kDepth = 8;
  constexpr auto kNodes = 3009794393;
  const auto state = StateFromFEN(kPosition3);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

// Position 4

TEST(Chess, PerftPosition4Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 6;
  const auto state = StateFromFEN(kPosition4);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition4Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 264;
  const auto state = StateFromFEN(kPosition4);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition4Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 9467;
  const auto state = StateFromFEN(kPosition4);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition4Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 422333;
  const auto state = StateFromFEN(kPosition4);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition4Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 15833292;
  const auto state = StateFromFEN(kPosition4);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition4Depth6) {
  constexpr auto kDepth = 6;
  constexpr auto kNodes = 706045033;
  const auto state = StateFromFEN(kPosition4);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

// Position 5

TEST(Chess, PerftPosition5Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 44;
  const auto state = StateFromFEN(kPosition5);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition5Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 1486;
  const auto state = StateFromFEN(kPosition5);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition5Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 62379;
  const auto state = StateFromFEN(kPosition5);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition5Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 2103487;
  const auto state = StateFromFEN(kPosition5);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition5Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 89941194;
  const auto state = StateFromFEN(kPosition5);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

// Position 6

TEST(Chess, PerftPosition6Depth1) {
  constexpr auto kDepth = 1;
  constexpr auto kNodes = 46;
  const auto state = StateFromFEN(kPosition6);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition6Depth2) {
  constexpr auto kDepth = 2;
  constexpr auto kNodes = 2079;
  const auto state = StateFromFEN(kPosition6);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition6Depth3) {
  constexpr auto kDepth = 3;
  constexpr auto kNodes = 89890;
  const auto state = StateFromFEN(kPosition6);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition6Depth4) {
  constexpr auto kDepth = 4;
  constexpr auto kNodes = 3894594;
  const auto state = StateFromFEN(kPosition6);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftPosition6Depth5) {
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 164075551;
  const auto state = StateFromFEN(kPosition6);
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, PerftTwoKings) {
  constexpr auto kDepth = 6;
  constexpr auto kNodes = 903346;
  const auto state = StateFromFEN("8/3pp3/5k2/8/8/5K2/3PP3/8 w - - 0 1");
  const auto result = Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, ZobristHashMovePiece) {
  auto state = StateFromFEN(kPosition1);
  const auto originalHash = state.hash;
  MakeMove(state, CreateMove(G1, F3));
  const auto newHash = state.hash;
  EXPECT_NE(originalHash, newHash);
  MakeMove(state, CreateMove(F3, G1));
  const auto backwardsHash = state.hash;
  EXPECT_EQ(originalHash, backwardsHash);
}

static bool TestZobristHash(std::string_view fen, U32 depth) {
  const auto state = StateFromFEN(fen);
  auto success = true;
  const auto result = PerftF(state, depth, [&success](const State& state, Move move) {
    const auto hash = CalculateHash(state);
    if (state.hash != hash) {
      std::cout << "Hash mismatch!\n";
      std::cout << "Incremental: " << state.hash << '\n';
      std::cout << "Calculated:  " << hash << '\n';
      std::cout << "FEN: " << StateToFEN(state) << '\n';
      std::cout << "Move: " << MoveToString(move) << '\n';
      success = false;
    }
  });
  return success;
}

TEST(Chess, ZobristHashPosition1) {
  EXPECT_TRUE(TestZobristHash(kPosition1, 6));
}

TEST(Chess, ZobristHashPosition2) {
  EXPECT_TRUE(TestZobristHash(kPosition2, 5));
}

TEST(Chess, ZobristHashPosition3) {
  EXPECT_TRUE(TestZobristHash(kPosition3, 7));
}

TEST(Chess, ZobristHashPosition4) {
  EXPECT_TRUE(TestZobristHash(kPosition4, 5));
}

TEST(Chess, ZobristHashPosition5) {
  EXPECT_TRUE(TestZobristHash(kPosition5, 5));
}

TEST(Chess, ZobristHashPosition6) {
  EXPECT_TRUE(TestZobristHash(kPosition6, 5));
}

static bool TestUndo(State& state, I32 depth) {
  auto moves = Moves{};
  GetLegalMoves(state, moves);

  if (depth == 1) {
    return true;
  }

  auto nodes = U64{0};
  for (const auto& move : moves) {
    auto expectedState = State{state};
    auto undo = Undo{};
    MakeMove(state, move, undo);
    nodes += TestUndo(state, depth - 1);
    UndoMove(state, move, undo);
    if (state != expectedState) {
      std::cout << "Expected: " << StateToFEN(expectedState) << '\n';
      std::cout << "Actual:   " << StateToFEN(state) << '\n';
      return false;
    }
  }

  return true;
}

TEST(Chess, UndoPosition1) {
  const auto expectedState = StateFromFEN(kPosition1);
  auto actualState = State{expectedState};
  EXPECT_TRUE(TestUndo(actualState, 6));
  EXPECT_EQ(expectedState, actualState);
}

TEST(Chess, UndoPosition2) {
  const auto expectedState = StateFromFEN(kPosition2);
  auto actualState = State{expectedState};
  EXPECT_TRUE(TestUndo(actualState, 5));
  EXPECT_EQ(expectedState, actualState);
}

TEST(Chess, UndoPosition3) {
  const auto expectedState = StateFromFEN(kPosition3);
  auto actualState = State{expectedState};
  EXPECT_TRUE(TestUndo(actualState, 7));
  EXPECT_EQ(expectedState, actualState);
}

TEST(Chess, UndoPosition4) {
  const auto expectedState = StateFromFEN(kPosition4);
  auto actualState = State{expectedState};
  EXPECT_TRUE(TestUndo(actualState, 5));
  EXPECT_EQ(expectedState, actualState);
}

TEST(Chess, UndoPosition5) {
  const auto expectedState = StateFromFEN(kPosition5);
  auto actualState = State{expectedState};
  EXPECT_TRUE(TestUndo(actualState, 5));
  EXPECT_EQ(expectedState, actualState);
}

TEST(Chess, UndoPosition6) {
  const auto expectedState = StateFromFEN(kPosition6);
  auto actualState = State{expectedState};
  EXPECT_TRUE(TestUndo(actualState, 5));
  EXPECT_EQ(expectedState, actualState);
}