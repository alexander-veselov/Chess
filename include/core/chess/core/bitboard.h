#pragma once

#include "chess/core/square.h"
#include "chess/core/types.h"

namespace chess {

using Bitboard = U64;

constexpr Bitboard BBFromSquare(Square square) {
  return 1ULL << square;
}

constexpr Bitboard kAFile =
    BBFromSquare(A1) |
    BBFromSquare(A2) |
    BBFromSquare(A3) |
    BBFromSquare(A4) |
    BBFromSquare(A5) |
    BBFromSquare(A6) |
    BBFromSquare(A7) |
    BBFromSquare(A8);

constexpr Bitboard k1Rank =
    BBFromSquare(A1) |
    BBFromSquare(B1) |
    BBFromSquare(C1) |
    BBFromSquare(D1) |
    BBFromSquare(E1) |
    BBFromSquare(F1) |
    BBFromSquare(G1) |
    BBFromSquare(H1);

constexpr Bitboard FillFile(File file) {
  return kAFile << file;
}

constexpr Bitboard FillRank(Rank rank) {
  return k1Rank << (8 * rank);
}

constexpr Bitboard kEmptyBoard = 0ULL;
constexpr Bitboard k2Rank = FillRank(_2);
constexpr Bitboard k7Rank = FillRank(_7);
constexpr Bitboard k18Rank = FillRank(_1) | FillRank(_8);
constexpr Bitboard kNotAFile = ~FillFile(_A);
constexpr Bitboard kNotHFile = ~FillFile(_H);
constexpr Bitboard kNotABFile = ~(FillFile(_A) | FillFile(_B));
constexpr Bitboard kNotGHFile = ~(FillFile(_G) | FillFile(_H));
constexpr Bitboard kEdges = FillFile(_A) | FillFile(_H) | FillRank(_1) | FillRank(_8);
constexpr Bitboard kCorners = BBFromSquare(A1) | BBFromSquare(A8) | BBFromSquare(H1) | BBFromSquare(H8);
constexpr Bitboard kF1G1 = BBFromSquare(F1) | BBFromSquare(G1);
constexpr Bitboard kF8G8 = BBFromSquare(F8) | BBFromSquare(G8);
constexpr Bitboard kB1C1D1 = BBFromSquare(B1) | BBFromSquare(C1) | BBFromSquare(D1);
constexpr Bitboard kB8C8D8 = BBFromSquare(B8) | BBFromSquare(C8) | BBFromSquare(D8);

} // namespace chess
