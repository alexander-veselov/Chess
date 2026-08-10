#pragma once

#include "chess/core/color.h"
#include "chess/core/move.h"
#include "chess/core/state.h"
#include "chess/engine/score.h"

#include <utility>
#include <vector>

namespace chess {

class Chess;

class Analysis {
public:
  Analysis();

  bool GetEnabled() const;
  const std::vector<Move>& GetLine() const;
  std::pair<Color, Score> GetEvaluation() const;

  void Update(const State& state);
  void SetEnabled(bool enabled);

private:
  std::pair<Color, Score> evaluation_;
  std::vector<Move> line_;
  State lastState_;
  bool enabled_;
};

}; // namespace chess