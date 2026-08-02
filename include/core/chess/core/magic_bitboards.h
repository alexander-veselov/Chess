#pragma once

#include "chess/core/bitboard.h"
#include "chess/core/square.h"

namespace chess {

Bitboard MagicBishopAttacks(Square square, Bitboard occupancy);
Bitboard MagicRookAttacks(Square square, Bitboard occupancy);
Bitboard MagicQueenAttacks(Square square, Bitboard occupancy);

} // namespace chess
