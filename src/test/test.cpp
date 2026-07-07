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

static std::string MoveToString(const Move& move) {
  auto result = std::string{};

  result += static_cast<char>('a' + move.from.file);
  result += static_cast<char>('1' + move.from.rank);

  result += static_cast<char>('a' + move.to.file);
  result += static_cast<char>('1' + move.to.rank);

if (move.promotion.has_value()) {
    switch (*move.promotion) {
    case Piece::kWhiteKnight:
    case Piece::kBlackKnight:
      result += 'n';
      break;
    case Piece::kWhiteBishop:
    case Piece::kBlackBishop:
      result += 'b';
      break;
    case Piece::kWhiteRook:
    case Piece::kBlackRook:
      result += 'r';
      break;
    case Piece::kWhiteQueen:
    case Piece::kBlackQueen:
      result += 'q';
      break;
    default:
      break;
    }
  }

  return result;
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

static uint64_t PerftDebug(const State& state, int32_t depth) {
  auto moves = std::vector<Move>{};
  const auto nMoves = GetAllLegalMoves(state, moves);

  if (depth == 1) {
    return nMoves;
  }

  auto nodes = uint64_t{0};
  for (const auto& move : moves) {
    auto newState = State{state};
    MakeMove(newState, move);
    const auto newNodes = Perft(newState, depth - 1);
    nodes += newNodes;

    std::cout << MoveToString(move) << ": " << newNodes << '\n';
  }

  return nodes;
}

} // namespace test
} // namespace chess

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
  // Still fails
  constexpr auto kDepth = 5;
  constexpr auto kNodes = 193690690;
  const auto state =
      chess::StateFromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
  const auto result = chess::test::Perft(state, kDepth);

  EXPECT_EQ(result, kNodes);
}

TEST(Chess, DISABLED_Debug) {
  constexpr auto kDepth = 2;
  auto state = chess::StateFromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
  MakeMove(state, chess::Move{chess::Square{chess::_1, chess::_A}, chess::Square{chess::_1, chess::_B}});
  MakeMove(state, chess::Move{chess::Square{chess::_3, chess::_H}, chess::Square{chess::_2, chess::_G}});
  MakeMove(state, chess::Move{chess::Square{chess::_2, chess::_H}, chess::Square{chess::_4, chess::_H}});
  MakeMove(state, chess::Move{chess::Square{chess::_2, chess::_G}, chess::Square{chess::_1, chess::_H}, chess::Piece::kBlackBishop});
  const auto result = chess::test::PerftDebug(state, kDepth);
}