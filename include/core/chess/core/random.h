#pragma once

#include "chess/core/types.h"

#include <limits>
#include <random>

namespace chess {

std::mt19937_64& GetRNG();
U32 RandomU32(U32 min = 0, U32 max = std::numeric_limits<U32>::max());
U64 RandomU64(U64 min = 0, U64 max = std::numeric_limits<U64>::max());

}