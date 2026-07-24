#pragma once

#include "chess/core/board.h"
#include "chess/core/square.h"

#include <bit>
#include <cstdint>

namespace chess {
namespace bitboard {

using U64 = uint64_t;
using I64 = int64_t;
using U8 = uint8_t;
using I8 = int8_t;
using Bitboard = U64;

struct Board {
  U64 whitePawns    = 0ULL;
  U64 whiteKnights  = 0ULL;
  U64 whiteBishops  = 0ULL;
  U64 whiteRooks    = 0ULL;
  U64 whiteQueens   = 0ULL;
  U64 whiteKings    = 0ULL;
  U64 blackPawns    = 0ULL;
  U64 blackKnights  = 0ULL;
  U64 blackBishops  = 0ULL;
  U64 blackRooks    = 0ULL;
  U64 blackQueens   = 0ULL;
  U64 blackKings    = 0ULL;
};

chess::Board ToNaive(const Board& bitboard);
Board FromNaive(const chess::Board& board);

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

static Bitboard BishopAttacks(Square square, Bitboard occupancy) {
  return SlidingAttacks(square, occupancy, NW) |
         SlidingAttacks(square, occupancy, NE) |
         SlidingAttacks(square, occupancy, SE) |
         SlidingAttacks(square, occupancy, SW);
}

static Bitboard RookAttacks(Square square, Bitboard occupancy) {
  return SlidingAttacks(square, occupancy, N) |
         SlidingAttacks(square, occupancy, E) |
         SlidingAttacks(square, occupancy, S) |
         SlidingAttacks(square, occupancy, W);
}

static Bitboard QueenAttacks(Square square, Bitboard occupancy) {
  return BishopAttacks(square, occupancy) |
         RookAttacks(square, occupancy);
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

} // namespace bitboard
} // namespace chess
