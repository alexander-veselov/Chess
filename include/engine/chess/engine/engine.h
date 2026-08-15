#pragma once

#include "chess/core/move.h"
#include "chess/core/state.h"
#include "chess/core/types.h"
#include "chess/engine/score.h"

#include <span>
#include <utility>
#include <vector>

namespace chess {

std::pair<std::span<const Move>, Score> BestMove(const State& state, U32 depth = 6);

}