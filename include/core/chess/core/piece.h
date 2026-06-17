#pragma once

#include <cstdint>

namespace chess {

enum class Piece : uint8_t {
  kNone,
  kWhiteKing,
  kWhiteQueen,
  kWhiteRook,
  kWhiteBishop,
  kWhiteKnight,
  kWhitePawn,
  kBlackKing,
  kBlackQueen,
  kBlackRook,
  kBlackBishop,
  kBlackKnight,
  kBlackPawn
};

}