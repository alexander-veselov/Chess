#pragma once

#include "chess/core/types.h"

namespace chess {

struct State;

using Hash = U64;
Hash CalculateHash(const State& state);

}