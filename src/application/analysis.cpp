#include "chess/application/analysis.h"

namespace chess {
namespace {
constexpr auto kMaxDepth = 100;
}

Analysis::Analysis()
  : searchInfo_{},
    searchInfoMutex_{},
    lastState_{kNullState},
    enabled_{false},
    searchStart_{} {
}

void Analysis::SetEnabled(bool enabled) {
  enabled_ = enabled;
  if (enabled_ == false) {
    searchThread_.request_stop();
  }
}

bool Analysis::GetEnabled() const {
  return enabled_;
}

SearchInfo Analysis::GetSearchInfo() const {
  auto lock = std::lock_guard<std::mutex>(searchInfoMutex_);
  return searchInfo_;
}

std::chrono::steady_clock::duration Analysis::GetSearchDuration() const {
  auto lock = std::lock_guard<std::mutex>(searchInfoMutex_);
  return std::chrono::steady_clock::now() - searchStart_;
}

void Analysis::SetSearchInfo(const SearchInfo& info) {
  auto lock = std::lock_guard<std::mutex>(searchInfoMutex_);
  searchInfo_ = info;
}

void Analysis::Update(const State& state) {
  if (!enabled_ || state == lastState_) {
    return;
  }
  lastState_ = state;
  if (searchThread_.joinable()) {
    searchThread_.request_stop();
    searchThread_.join();
  }
  SetSearchInfo(SearchInfo{});
  searchStart_ = std::chrono::steady_clock::now();
  searchThread_ = std::jthread([state, this](std::stop_token stopToken) {
    BestMove(state, kMaxDepth, [this](const auto& info) { SetSearchInfo(info); }, stopToken);
  });
}

} // namespace chess