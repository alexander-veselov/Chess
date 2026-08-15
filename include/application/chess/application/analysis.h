#pragma once

#include "chess/core/color.h"
#include "chess/core/move.h"
#include "chess/core/state.h"
#include "chess/engine/score.h"

#include <future>
#include <span>
#include <utility>
#include <vector>

namespace chess {

class Chess;

class Analysis {
public:
  Analysis();

  bool IsReady() const;
  bool GetEnabled() const;
  std::span<const Move> GetLine() const;
  Score GetEvaluation() const;

  void Update(const State& state);
  void SetEnabled(bool enabled);

private:
  Score evaluation_;
  std::span<const Move> line_;
  State lastState_;
  bool enabled_;
  std::future<std::pair<std::span<const Move>, Score>> future_;
};

}; // namespace chess