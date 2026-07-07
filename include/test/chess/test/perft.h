#pragma once

#include "chess/core/game.h"

#include <map>
#include <string>

namespace chess {
namespace test {

uint64_t Perft(const State& state, int32_t depth);
std::map<std::string, uint64_t> PerftDebug(const State& state, int32_t depth);

} // namespace test
} // namespace chess