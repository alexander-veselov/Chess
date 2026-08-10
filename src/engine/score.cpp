#include "chess/engine/score.h"

#include <format>

namespace chess {

bool IsMateInN(Score score) {
  return std::abs(score) > kMaxScore;
}

U32 GetMatePly(Score score) {
  return kInfinity - std::abs(score);
}

U32 GetMateDepth(Score score) {
  return std::ceil(GetMatePly(score) / 2.);
}

float ScoreToFloat(Score score) {
  return score / 100.f;
}

std::string ScoreToString(Score score) {
  if (IsMateInN(score)) {
    const auto mateDepth = GetMateDepth(score);
    return "M" + (mateDepth == 0 ? "" : std::to_string(mateDepth));
  }
  return std::format("{:.1f}", std::fabs(ScoreToFloat(score)));
}

} // namespace chess