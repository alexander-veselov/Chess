#pragma once

#include "chess/core/square.h"
#include "chess/core/types.h"

#include <bit>

namespace chess {

constexpr Square LSB(U64 value) {
  return static_cast<Square>(std::countr_zero(value));
}

constexpr Square PopLSB(U64& value) {
  auto square = LSB(value);
  value &= value - 1;
  return square;
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

} // namespace chess
