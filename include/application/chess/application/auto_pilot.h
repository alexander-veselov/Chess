#pragma once

#include "chess/core/hash.h"
#include "chess/core/move.h"

#include <optional>

namespace chess {

class Analysis;
struct State;

class AutoPilot {
public:
  AutoPilot();

  std::optional<Move> PopPendingMove();
  void Update(const State& state, const Analysis& analysis);

private:
  Hash hash_;
  std::optional<Move> pendingMove_;
};

}; // namespace chess