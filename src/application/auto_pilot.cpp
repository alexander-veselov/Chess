#include "chess/application/auto_pilot.h"

#include "chess/application/analysis.h"
#include "chess/core/state.h"

#include <iostream>

namespace chess {

AutoPilot::AutoPilot()
  : hash_{0}, pendingMove_{std::nullopt} {
}

std::optional<Move> AutoPilot::PopPendingMove() {
  auto move = pendingMove_;
  pendingMove_ = std::nullopt;
  return move;
}

void AutoPilot::Update(const State& state, const Analysis& analysis) {
  if (!analysis.GetEnabled() || pendingMove_.has_value() || state.hash == hash_) {
    return;
  }
  const auto searchDuration = analysis.GetSearchDuration();
  const auto searchInfo = analysis.GetSearchInfo();
  if (searchInfo.hash != 0 && searchInfo.hash != state.hash) {
    std::cout << "AutoPilot error" << std::endl;
  }
  if (searchDuration > std::chrono::milliseconds(300)) {
  //if (searchInfo.depth >= 6) {
    if (!searchInfo.line.empty()) {
      std::cout << searchInfo.hash << " " << searchInfo.depth << std::endl;
      pendingMove_ = searchInfo.line[0];
      hash_ = state.hash;
    }
  }
}


} // namespace chess