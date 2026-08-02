#pragma once

#include "chess/core/state.h"

#include <string>

namespace chess {

std::string StateToFEN(const State& state);
bool ParseFEN(std::string_view fen, State& state);

State StateFromFEN(std::string_view fen);

}