#pragma once

#include "chess/core/game.h"

#include <map>
#include <string>

namespace chess {

U64 Perft(const State& state, I32 depth);
std::map<std::string, U64> Divide(const State& state, I32 depth);

} // namespace chess