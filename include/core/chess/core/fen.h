#pragma once

#include "chess/core/state.h"

#include <string>

namespace chess {

State StateFromFEN(const std::string& fen);
std::string FENFromState(const State& state);

}