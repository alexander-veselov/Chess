#include "chess/application/analysis.h"

#include "chess/engine/engine.h"

namespace chess {

Analysis::Analysis()
  : line_{},
    evaluation_{Color::kWhite, 0},
    lastState_{kNullState} {
}

void Analysis::SetEnabled(bool enabled) {
  enabled_ = enabled;
}

bool Analysis::GetEnabled() const {
  return enabled_;
}

std::pair<Color, Score> Analysis::GetEvaluation() const {
  return evaluation_;
}

const std::vector<Move>& Analysis::GetLine() const {
  return line_;
}

void Analysis::Update(const State& state) {
  if (state == lastState_) {
    return;
  } else {
    lastState_ = state;
  }
  evaluation_.first = state.turn;
  std::tie(line_, evaluation_.second) = BestMove(state);
}

} // namespace chess