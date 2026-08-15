#include "chess/application/analysis.h"

#include "chess/engine/engine.h"

namespace chess {

Analysis::Analysis()
  : line_{},
    evaluation_{0},
    lastState_{kNullState},
    enabled_{false} {
}

void Analysis::SetEnabled(bool enabled) {
  enabled_ = enabled;
}

bool Analysis::IsReady() const {
  return future_.valid() ? future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready : true;
}

bool Analysis::GetEnabled() const {
  return enabled_;
}

Score Analysis::GetEvaluation() const {
  return evaluation_;
}

std::span<const Move> Analysis::GetLine() const {
  return line_;
}

void Analysis::Update(const State& state) {
  if (!enabled_) {
    return;
  }
  if (future_.valid()) {
    if (IsReady()) {
      std::tie(line_, evaluation_) = future_.get();
    } else {
      return;
    }
  }
  if (state == lastState_) {
    return;
  }
  lastState_ = state;
  future_ = std::async(std::launch::async, [state] { return BestMove(state); });
}

} // namespace chess