#include "chess/core/move.h"

namespace chess {

bool operator==(const Move& move1, const Move& move2) {
  return move1.from == move2.from && move1.to == move2.to && move1.promotion == move2.promotion;
}

} // namespace chess