#pragma once

#include "chess/core/move.h"
#include "chess/core/state.h"
#include "chess/core/types.h"
#include "chess/core/zobrist_hash.h"
#include "chess/engine/score.h"

#include <functional>
#include <stop_token>
#include <utility>
#include <vector>

namespace chess {

struct SearchInfo {
  Hash hash = 0;
  U64 nodes = 0;
  U64 nodesPerSecond = 0;
  U32 depth = 0;
  Score score = 0;
  std::vector<Move> line;
};

using SearchInfoCallback = std::function<void(const SearchInfo&)>;

SearchInfo BestMove(const State& state, U32 maxDepth = 6, SearchInfoCallback callback = {},
                    std::stop_token stopToken = {});

}