#pragma once

#include "chess/core/constants.h"
#include "chess/core/piece.h"

#include <array>

namespace chess {

enum File : uint8_t { _A, _B, _C, _D, _E, _F, _G, _H };
enum Rank : uint8_t { _8, _7, _6, _5, _4, _3, _2, _1 };

struct Square {
  Rank rank;
  File file;
};

using Board = std::array<std::array<Piece, kBoardSize>, kBoardSize>;

}
