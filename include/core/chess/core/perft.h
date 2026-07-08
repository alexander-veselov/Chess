#pragma once

#include "chess/core/game.h"

#include <map>
#include <string>

namespace chess {

uint64_t Perft(const State& state, int32_t depth);
std::map<std::string, uint64_t> Divide(const State& state, int32_t depth);

} // namespace chess