#pragma once

#include "chess/core/bitboard.h"
#include "chess/core/bits.h"
#include "chess/core/direction.h"
#include "chess/core/square.h"

namespace chess {

constexpr Bitboard KnightAttacks(Bitboard bitboard) {
  return ((bitboard & NotABFile) << (+NW + W)) |
         ((bitboard & NotAFile ) << (+NW + N)) |
         ((bitboard & NotHFile ) << (+NE + N)) |
         ((bitboard & NotGHFile) << (+NE + E)) |
         ((bitboard & NotGHFile) >> (-SE - E)) |
         ((bitboard & NotHFile ) >> (-SE - S)) |
         ((bitboard & NotAFile ) >> (-SW - S)) |
         ((bitboard & NotABFile) >> (-SW - W));
}

constexpr Bitboard KingAttacks(Bitboard bitboard) {
  return ((bitboard & NotAFile) >> -SW) |
         ((bitboard           ) >> -S ) |
         ((bitboard & NotHFile) >> -SE) |
         ((bitboard & NotAFile) >> -W) |
         ((bitboard & NotHFile) << +E ) |
         ((bitboard & NotAFile) << +NW) |
         ((bitboard           ) << +N ) |
         ((bitboard & NotHFile) << +NE);
}

constexpr Bitboard WhitePawnAttacks(Bitboard bitboard) {
  return ((bitboard & NotAFile) << NW) |
         ((bitboard & NotHFile) << NE);
}

constexpr Bitboard BlackPawnAttacks(Bitboard bitboard) {
  return ((bitboard & NotAFile) >> -SW) |
         ((bitboard & NotHFile) >> -SE);
}

constexpr Bitboard WhitePawnSinglePushes(Bitboard bitboard, Bitboard occupancy) {
  return (bitboard << N) & ~occupancy;
}

constexpr Bitboard WhitePawnDoublePushes(Bitboard bitboard, Bitboard occupancy) {
  return WhitePawnSinglePushes(WhitePawnSinglePushes(bitboard & _2Rank, occupancy), occupancy);
}

constexpr Bitboard BlackPawnSinglePushes(Bitboard bitboard, Bitboard occupancy) {
  return (bitboard >> -S) & ~occupancy;
}

constexpr Bitboard BlackPawnDoublePushes(Bitboard bitboard, Bitboard occupancy) {
  return BlackPawnSinglePushes(BlackPawnSinglePushes(bitboard & _7Rank, occupancy), occupancy);
}

constexpr Bitboard ShiftSquare(Square square, Direction direction) {
  const auto shifted = Square(square + direction);
  return ValidSquare(shifted) && std::abs(GetFile(square) - GetFile(shifted)) <= 1 ? 1ULL << shifted
                                                                                   : 0ULL;
}

constexpr Bitboard SlidingAttacks(Square square, Bitboard occupancy, Direction direction) {
  auto attacks = Bitboard(0ULL);

  auto shifted = square;
  auto attack = Bitboard(0ULL);
  while (attack = ShiftSquare(shifted, direction)) {
    shifted = Square(shifted + direction);
    attacks |= attack;
    if (attack & occupancy) {
      break;
    }
  }

  return attacks;
}

constexpr Bitboard SingleBishopAttacks(Square square, Bitboard occupancy) {
  return SlidingAttacks(square, occupancy, NW) |
         SlidingAttacks(square, occupancy, NE) |
         SlidingAttacks(square, occupancy, SE) |
         SlidingAttacks(square, occupancy, SW);
}

constexpr Bitboard SingleRookAttacks(Square square, Bitboard occupancy) {
  return SlidingAttacks(square, occupancy, N) |
         SlidingAttacks(square, occupancy, E) |
         SlidingAttacks(square, occupancy, S) |
         SlidingAttacks(square, occupancy, W);
}

constexpr Bitboard SingleQueenAttacks(Square square, Bitboard occupancy) {
  return SingleBishopAttacks(square, occupancy) |
         SingleRookAttacks(square, occupancy);
}

constexpr Bitboard BishopAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = Bitboard{0ULL};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= SingleBishopAttacks(from, occupancy);
  }
  return attacks;
}

constexpr Bitboard RookAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = Bitboard{0ULL};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= SingleRookAttacks(from, occupancy);
  }
  return attacks;
}

constexpr Bitboard QueenAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = Bitboard{0ULL};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= SingleQueenAttacks(from, occupancy);
  }
  return attacks;
}

} // namespace chess
