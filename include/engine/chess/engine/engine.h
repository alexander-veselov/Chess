#pragma once

#include "chess/core/move.h"
#include "chess/core/state.h"
#include "chess/core/types.h"
#include "chess/engine/score.h"

#include <utility>
#include <vector>

namespace chess {

std::pair<std::vector<Move>, Score> BestMove(const State& state, U32 depth = 6);

}