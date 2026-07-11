#pragma once

#include "chess/core/constants.h"

#include <cstdint>

namespace chess {

enum File : uint8_t { _A, _B, _C, _D, _E, _F, _G, _H };
enum Rank : uint8_t { _1, _2, _3, _4, _5, _6, _7, _8 };

enum Square : uint8_t {
  A1, A2, A3, A4, A5, A6, A7, A8,
  B1, B2, B3, B4, B5, B6, B7, B8,
  C1, C2, C3, C4, C5, C6, C7, C8,
  D1, D2, D3, D4, D5, D6, D7, D8,
  E1, E2, E3, E4, E5, E6, E7, E8,
  F1, F2, F3, F4, F5, F6, F7, F8,
  G1, G2, G3, G4, G5, G6, G7, G8,
  H1, H2, H3, H4, H5, H6, H7, H8,
};

constexpr Square MakeSquare(File file, Rank rank) {
  return static_cast<Square>(file << 3 | rank);
}

constexpr File GetFile(Square square) {
  return static_cast<File>(square >> 3);
}

constexpr Rank GetRank(Square square) {
  return static_cast<Rank>(square & 0b111);
}


} // namespace chess
