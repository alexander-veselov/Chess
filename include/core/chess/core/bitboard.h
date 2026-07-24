#pragma once

#include "chess/core/board.h"
#include "chess/core/square.h"
#include "chess/core/types.h"

#include <bit>

namespace chess {

using Bitboard = U64;

constexpr Square LSB(const Bitboard& bitboard) {
  return static_cast<Square>(std::countr_zero(bitboard));
}

constexpr Square PopLSB(Bitboard& bitboard) {
  auto square = LSB(bitboard);
  bitboard &= bitboard - 1;
  return square;
}

constexpr U64 ClearBit(U64 value, U8 position) {
  return value & ~(1ULL << position);
}

constexpr U64 SetBit(U64 value, U8 position) {
  return value | (1ULL << position);
}

static U64 SetBit(U64 value, U8 position, U8 bit) {
  if (bit == 0) {
    return ClearBit(value, position);
  } else {
    return SetBit(value, position);
  }
}

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

constexpr Bitboard AFile =
    (1ULL << A1) |
    (1ULL << A2) |
    (1ULL << A3) |
    (1ULL << A4) |
    (1ULL << A5) |
    (1ULL << A6) |
    (1ULL << A7) |
    (1ULL << A8);

constexpr Bitboard _1Rank = 0b11111111;

constexpr Bitboard FillFile(File file) {
  return AFile << file;
}

constexpr Bitboard FillRank(Rank rank) {
  return _1Rank << (8 * rank);
}

constexpr Bitboard _2Rank = FillRank(_2);
constexpr Bitboard _7Rank = FillRank(_7);
constexpr Bitboard NotAFile = ~FillFile(_A);
constexpr Bitboard NotHFile = ~FillFile(_H);
constexpr Bitboard NotABFile = ~(FillFile(_A) | FillFile(_B));
constexpr Bitboard NotGHFile = ~(FillFile(_G) | FillFile(_H));

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

constexpr bool ValidSquare(Square square) {
  return A1 <= square && square <= H8;
}

constexpr Bitboard ShiftSquare(Square square, Direction direction) {
  const auto shifted = Square(square + direction);
  return ValidSquare(shifted) && std::abs(GetFile(square) - GetFile(shifted)) <= 1 ? 1ULL << shifted
                                                                                   : 0ULL;
}

static Bitboard SlidingAttacks(Square square, Bitboard occupancy, Direction direction) {
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

static Bitboard SingleBishopAttacks(Square square, Bitboard occupancy) {
  return SlidingAttacks(square, occupancy, NW) |
         SlidingAttacks(square, occupancy, NE) |
         SlidingAttacks(square, occupancy, SE) |
         SlidingAttacks(square, occupancy, SW);
}

static Bitboard SingleRookAttacks(Square square, Bitboard occupancy) {
  return SlidingAttacks(square, occupancy, N) |
         SlidingAttacks(square, occupancy, E) |
         SlidingAttacks(square, occupancy, S) |
         SlidingAttacks(square, occupancy, W);
}

static Bitboard SingleQueenAttacks(Square square, Bitboard occupancy) {
  return SingleBishopAttacks(square, occupancy) |
         SingleRookAttacks(square, occupancy);
}

static Bitboard BishopAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = Bitboard{0ULL};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= SingleBishopAttacks(from, occupancy);
  }
  return attacks;
}

static Bitboard RookAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = Bitboard{0ULL};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= SingleRookAttacks(from, occupancy);
  }
  return attacks;
}

static Bitboard QueenAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = Bitboard{0ULL};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= SingleQueenAttacks(from, occupancy);
  }
  return attacks;
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

} // namespace chess
