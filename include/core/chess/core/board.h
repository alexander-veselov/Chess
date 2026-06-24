#pragma once

#include "chess/core/constants.h"
#include "chess/core/piece.h"

#include <array>

namespace chess {

enum File : uint8_t { _A, _B, _C, _D, _E, _F, _G, _H };
enum Rank : uint8_t { _1, _2, _3, _4, _5, _6, _7, _8 };

struct Square {
  Rank rank;
  File file;
};

using Board = std::array<std::array<Piece, kBoardSize>, kBoardSize>;

}
