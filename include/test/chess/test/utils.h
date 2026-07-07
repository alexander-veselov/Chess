#pragma once

#include "chess/core/game.h"

#include <string>

namespace chess {
namespace test {

uint64_t GetAllLegalMoves(const State& state, std::vector<Move>& legalMoves);
std::string MoveToString(const Move& move);
bool ParseMove(const std::string& string, Move& move);

} // namespace test
} // namespace chess