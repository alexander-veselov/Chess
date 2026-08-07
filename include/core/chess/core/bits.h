#pragma once

#include "chess/core/square.h"
#include "chess/core/types.h"

#include <bit>
#include <cassert>

namespace chess {

constexpr Square LSB(U64 value) {
  assert(value != 0ULL);
#ifdef _MSC_VER
  unsigned long index;
  _BitScanForward64(&index, value);
  return static_cast<Square>(index);
#else
  return static_cast<Square>(std::countr_zero(value));
#endif
}

constexpr Square PopLSB(U64& value) {
  auto square = LSB(value);
  value &= value - 1;
  return square;
}

constexpr U32 PopCount(U64 value) {
  return std::popcount(value);
}

constexpr U64 SetBit(U64 value, U8 position) {
  return value | (1ULL << position);
}

constexpr U64 ResetBit(U64 value, U8 position) {
  return value & ~(1ULL << position);
}

constexpr U64 FlipBit(U64 value, U8 position) {
  return value ^ (1ULL << position);
}

constexpr U64 AssignBit(U64 value, U8 position, bool bit) {
  return bit ? SetBit(value, position) : ResetBit(value, position);
}

constexpr bool HasAllBits(U64 value, U64 bits) {
  return (value & bits) == bits;
}

} // namespace chess
