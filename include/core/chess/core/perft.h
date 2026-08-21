#pragma once

#include "chess/core/game.h"

#include <map>
#include <string>
#include <functional>

namespace chess {

U64 Perft(const State& state, I32 depth);
U64 PerftF(const State& state, I32 depth, std::function<void(const State&, Move)> f);
std::map<std::string, U64> Divide(const State& state, I32 depth);

} // namespace chess