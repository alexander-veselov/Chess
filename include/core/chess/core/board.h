#pragma once

#include "chess/core/constants.h"
#include "chess/core/piece.h"

#include <array>

namespace chess {

using Board = std::array<std::array<Piece, kBoardSize>, kBoardSize>;

}
