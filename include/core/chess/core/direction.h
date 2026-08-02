#pragma once

#include "chess/core/types.h"

namespace chess {

// NW   N   NE
//   \  |  /
// W ------- E
//   /  |  \
// SW   S   SE

enum Direction : I8 {
  SW = -9,
  S  = -8,
  SE = -7,
  W  = -1,
  E  = +1,
  NW = +7,
  N  = +8,
  NE = +9
};

} // namespace chess
