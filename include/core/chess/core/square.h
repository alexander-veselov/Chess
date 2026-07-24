#pragma once

#include "chess/core/constants.h"
#include "chess/core/types.h"

#include <string>

namespace chess {

enum File : U8 { _A, _B, _C, _D, _E, _F, _G, _H };
enum Rank : U8 { _1, _2, _3, _4, _5, _6, _7, _8 };

enum Square : U8 {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,

  kSquareCount, kInvalid
};

constexpr Square CreateSquare(File file, Rank rank) {
  return static_cast<Square>(rank << 3 | file);
}

constexpr File GetFile(Square square) {
  return static_cast<File>(square & 0b111);
}

constexpr Rank GetRank(Square square) {
  return static_cast<Rank>(square >> 3);
}

std::string SquareToString(Square square);
bool ParseSquare(const std::string& string, Square& square);


} // namespace chess
