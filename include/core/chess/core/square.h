#pragma once

#include "chess/core/constants.h"

#include <cstdint>

namespace chess {

enum File : uint8_t { _A, _B, _C, _D, _E, _F, _G, _H };
enum Rank : uint8_t { _1, _2, _3, _4, _5, _6, _7, _8 };

struct Square {
  Rank rank;
  File file;
};

bool operator==(const Square& square1, const Square& square2);

} // namespace chess
