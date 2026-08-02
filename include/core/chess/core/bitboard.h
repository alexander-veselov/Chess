#pragma once

#include "chess/core/square.h"
#include "chess/core/types.h"

namespace chess {

using Bitboard = U64;

constexpr Bitboard AFile =
    (1ULL << A1) |
    (1ULL << A2) |
    (1ULL << A3) |
    (1ULL << A4) |
    (1ULL << A5) |
    (1ULL << A6) |
    (1ULL << A7) |
    (1ULL << A8);

constexpr Bitboard _1Rank = 0b11111111;

constexpr Bitboard FillFile(File file) {
  return AFile << file;
}

constexpr Bitboard FillRank(Rank rank) {
  return _1Rank << (8 * rank);
}

constexpr Bitboard _2Rank = FillRank(_2);
constexpr Bitboard _7Rank = FillRank(_7);
constexpr Bitboard NotAFile = ~FillFile(_A);
constexpr Bitboard NotHFile = ~FillFile(_H);
constexpr Bitboard NotABFile = ~(FillFile(_A) | FillFile(_B));
constexpr Bitboard NotGHFile = ~(FillFile(_G) | FillFile(_H));
constexpr Bitboard Edges = FillFile(_A) | FillFile(_H) | FillRank(_1) | FillRank(_8);
constexpr Bitboard Corners = (1ULL << A1) | (1ULL << A8) | (1ULL << H1) | (1ULL << H8);

} // namespace chess
