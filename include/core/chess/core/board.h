#pragma once

#include "chess/core/constants.h"
#include "chess/core/piece.h"
#include "chess/core/square.h"

#include <array>

namespace chess {

bool operator==(const Square& square1, const Square& square2);

using Board = std::array<std::array<Piece, kBoardSize>, kBoardSize>;

}
