#include "chess/core/zobrist_hash.h"

#include "chess/core/bits.h"
#include "chess/core/piece.h"
#include "chess/core/random.h"
#include "chess/core/state.h"

#include <array>

namespace chess {
namespace {

class ZobristHash {
public:
  ZobristHash() {
    turnHash_ = RandomU64();
    for (auto& row : boardHash_) {
      for (auto& element : row) {
        element = RandomU64();
      }
    }
    for (auto& element : enPassantHash_) {
      element = RandomU64();
    }
    for (auto& element : castlingRightsMaskHash_) {
      element = RandomU64();
    }
  }

  Hash CalculateHash(const State& state) const {
    auto hash = Hash{0};
    if (state.turn == Color::kBlack) {
      hash ^= turnHash_;
    }
    if (state.enPassant != Square::kInvalid) {
      hash ^= enPassantHash_[GetFile(state.enPassant)];
    }
    hash ^= castlingRightsMaskHash_[state.castlingRightsMask];
    for (auto pieceIndex = 0; pieceIndex < kPieceCount; ++pieceIndex) {
      auto bitboard = state.bitboards[pieceIndex];
      while (bitboard) {
        const auto square = PopLSB(bitboard);
        hash ^= boardHash_[square][pieceIndex];
      }
    }
    return hash;
  }

private:
  Hash turnHash_;
  std::array<Hash, 16> castlingRightsMaskHash_;
  std::array<Hash, kBoardSize> enPassantHash_;
  std::array<std::array<Hash, kPieceCount>, kSquareCount> boardHash_;
} g_ZobristHash; // Should be only one instance

} // namespace

Hash CalculateHash(const State& state) {
  return g_ZobristHash.CalculateHash(state);
}

} // namespace chess