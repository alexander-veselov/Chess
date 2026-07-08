#pragma once

#include "chess/core/piece.h"
#include "chess/core/square.h"

#include <string>
#include <optional>

namespace chess {

struct Move {
  Square from;
  Square to;
  std::optional<Piece> promotion;
};

bool operator==(const Move& move1, const Move& move2);

std::string MoveToString(const Move& move);
bool ParseMove(const std::string& string, Move& move);

} // namespace chess