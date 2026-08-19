#pragma once

#include "chess/core/color.h"
#include "chess/core/move.h"
#include "chess/core/state.h"
#include "chess/engine/engine.h"
#include "chess/engine/score.h"

#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

namespace chess {

class Chess;

class Analysis {
public:
  Analysis();

  bool GetEnabled() const;
  SearchInfo GetSearchInfo() const;
  std::chrono::steady_clock::duration GetSearchDuration() const;

  void Update(const State& state);
  void SetEnabled(bool enabled);

private:
  void SetSearchInfo(const SearchInfo& info);

  State lastState_;
  bool enabled_;
  SearchInfo searchInfo_;
  mutable std::mutex searchInfoMutex_;
  std::jthread searchThread_;
  std::chrono::steady_clock::time_point searchStart_;
};

}; // namespace chess