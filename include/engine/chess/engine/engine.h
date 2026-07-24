#pragma once

#include "chess/core/state.h"
#include "chess/core/move.h"

namespace chess {

Move BestMove(const State& state, U32 depth=5);

}