#pragma once

#include "chess/core/bitboard.h"
#include "chess/core/bits.h"
#include "chess/core/direction.h"
#include "chess/core/square.h"

namespace chess {

constexpr Bitboard KnightAttacks(Bitboard bitboard) {
  return ((bitboard & kNotABFile) << (+NW + W)) |
         ((bitboard & kNotAFile ) << (+NW + N)) |
         ((bitboard & kNotHFile ) << (+NE + N)) |
         ((bitboard & kNotGHFile) << (+NE + E)) |
         ((bitboard & kNotGHFile) >> (-SE - E)) |
         ((bitboard & kNotHFile ) >> (-SE - S)) |
         ((bitboard & kNotAFile ) >> (-SW - S)) |
         ((bitboard & kNotABFile) >> (-SW - W));
}

constexpr Bitboard KingAttacks(Bitboard bitboard) {
  return ((bitboard & kNotAFile) >> -SW) |
         ((bitboard            ) >> -S ) |
         ((bitboard & kNotHFile) >> -SE) |
         ((bitboard & kNotAFile) >> -W) |
         ((bitboard & kNotHFile) << +E ) |
         ((bitboard & kNotAFile) << +NW) |
         ((bitboard            ) << +N ) |
         ((bitboard & kNotHFile) << +NE);
}

constexpr Bitboard WhitePawnAttacks(Bitboard bitboard) {
  return ((bitboard & kNotAFile) << NW) |
         ((bitboard & kNotHFile) << NE);
}

constexpr Bitboard BlackPawnAttacks(Bitboard bitboard) {
  return ((bitboard & kNotAFile) >> -SW) |
         ((bitboard & kNotHFile) >> -SE);
}

constexpr Bitboard WhitePawnSinglePushes(Bitboard bitboard, Bitboard occupancy) {
  return (bitboard << N) & ~occupancy;
}

constexpr Bitboard WhitePawnDoublePushes(Bitboard bitboard, Bitboard occupancy) {
  return WhitePawnSinglePushes(WhitePawnSinglePushes(bitboard & k2Rank, occupancy), occupancy);
}

constexpr Bitboard BlackPawnSinglePushes(Bitboard bitboard, Bitboard occupancy) {
  return (bitboard >> -S) & ~occupancy;
}

constexpr Bitboard BlackPawnDoublePushes(Bitboard bitboard, Bitboard occupancy) {
  return BlackPawnSinglePushes(BlackPawnSinglePushes(bitboard & k7Rank, occupancy), occupancy);
}

constexpr Bitboard ShiftSquare(Square square, Direction direction) {
  const auto shifted = Square(square + direction);
  return ValidSquare(shifted) && std::abs(GetFile(square) - GetFile(shifted)) <= 1
             ? BBFromSquare(shifted)
             : kEmptyBoard;
}

constexpr Bitboard SlidingAttacks(Square square, Bitboard occupancy, Direction direction) {
  auto attacks = kEmptyBoard;

  auto shifted = square;
  auto attack = kEmptyBoard;
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
  auto attacks = kEmptyBoard;
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= SingleBishopAttacks(from, occupancy);
  }
  return attacks;
}

constexpr Bitboard RookAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = kEmptyBoard;
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= SingleRookAttacks(from, occupancy);
  }
  return attacks;
}

constexpr Bitboard QueenAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = kEmptyBoard;
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= SingleQueenAttacks(from, occupancy);
  }
  return attacks;
}

} // namespace chess
