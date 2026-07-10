#pragma once

#include "chess/core/state.h"
#include "chess/core/move.h"

namespace chess {

Move BestMove(const State& state, uint32_t depth=4);

}