#pragma once

#include "chess/core/types.h"

#include <string>

namespace chess {

using Score = I32;

constexpr auto kInfinity = Score{99999};
constexpr auto kMaxScore = Score{88888};
constexpr auto kDrawScore = Score{0};

constexpr Score MatedIn(U32 ply) {
  return -kInfinity + ply;
}

constexpr Score MateIn(U32 ply) {
  return kInfinity - ply;
}

bool IsMateInN(Score score);
U32 GetMatePly(Score score);
U32 GetMateDepth(Score score);
float ScoreToFloat(Score score);
std::string ScoreToString(Score score);

}