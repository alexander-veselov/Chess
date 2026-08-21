#pragma once

#include "chess/engine/score.h"

#include <array>

namespace chess {

constexpr auto kPieceValues = [] {
  std::array<Score, kPieceCount> values{};
  values[kNone] = 0;
  values[kWhiteKing] = 0;
  values[kWhiteQueen] = +900;
  values[kWhiteRook] = +500;
  values[kWhiteBishop] = +325;
  values[kWhiteKnight] = +310;
  values[kWhitePawn] = +100;
  values[kBlackKing] = 0;
  values[kBlackQueen] = -900;
  values[kBlackRook] = -500;
  values[kBlackBishop] = -325;
  values[kBlackKnight] = -310;
  values[kBlackPawn] = -100;
  return values;
}();

} // namespace chess