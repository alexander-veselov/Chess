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
    for (auto pieceIndex = 1; pieceIndex < kPieceCount; ++pieceIndex) {
      auto bitboard = state.bitboards[pieceIndex];
      while (bitboard) {
        const auto square = PopLSB(bitboard);
        hash ^= boardHash_[square][pieceIndex];
      }
    }
    return hash;
  }

  void UpdatePiece(Hash& hash, Square square, Piece piece) const {
    hash ^= boardHash_[square][piece];
  }

  void UpdateTurn(Hash& hash) const {
    hash ^= turnHash_;
  }

  void UpdateCastlingRights(Hash& hash, CastlingRightsMask mask) const {
    hash ^= castlingRightsMaskHash_[mask];
  }

  void UpdateEnPassant(Hash& hash, File file) const {
    hash ^= enPassantHash_[file];
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

void UpdatePiece(Hash& hash, Square square, Piece piece) {
  g_ZobristHash.UpdatePiece(hash, square, piece);
}

void UpdateTurn(Hash& hash) {
  g_ZobristHash.UpdateTurn(hash);
}

void UpdateCastlingRights(Hash& hash, CastlingRightsMask mask) {
  g_ZobristHash.UpdateCastlingRights(hash, mask);
}

void UpdateEnPassant(Hash& hash, File file) {
  g_ZobristHash.UpdateEnPassant(hash, file);
}

} // namespace chess