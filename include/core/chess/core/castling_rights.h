#pragma once

#include "chess/core/types.h"

#include <string>

namespace chess {

enum CastlingRight : U8 {
  kWhiteKingSide  = 1 << 0,
  kWhiteQueenSide = 1 << 1,
  kBlackKingSide  = 1 << 2,
  kBlackQueenSide = 1 << 3
};

using CastlingRightsMask = U8;

constexpr void AddCastlingRight(CastlingRightsMask& mask, CastlingRight right) {
  mask |= static_cast<U8>(right);
}

constexpr void RemoveCastlingRight(CastlingRightsMask& mask, CastlingRight right) {
  mask &= ~static_cast<U8>(right);
}

constexpr bool CanCastle(CastlingRightsMask mask, CastlingRight castlingRight) {
  return (mask & castlingRight) != 0;
}

std::string CastlingRightToString(CastlingRight castlingRight);
bool ParseCastlingRight(std::string_view string, CastlingRight& castlingRight);

std::string CastlingRightsMaskToString(CastlingRightsMask mask);

} // namespace chess
