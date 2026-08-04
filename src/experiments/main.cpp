#include "chess/core/bits.h"
#include "chess/core/bitboard.h"
#include "chess/core/attacks.h"

#include <string>
#include <iostream>
#include <ranges>

using namespace chess;

namespace {

std::string ToString(Bitboard bitboard) {
  auto result = std::string(kBoardTotalSize, '0');
  for (auto position = 0; position < kBoardTotalSize; ++position) {
    const auto bit = bitboard & BBFromSquare(static_cast<Square>(position));
    if (bit != 0) {
      result[position] = '1';
    }
  }
  return result;
}

std::string To2DString(Bitboard bitboard) {
  auto result = std::string{};
  for (auto chunk : ToString(bitboard) | std::views::chunk(kBoardSize)) {
    result = std::string{chunk.begin(), chunk.end()} + '\n' + result;
  }
  result.pop_back();
  return result;
}

} // namespace

int main() {
  std::cout << "{";
  for (auto square = 0; square < 64; ++square) {
    std::cout << MultipleKingAttacks(1ULL << square) << ", ";
  }
  std::cout << "}";
  
  return 0;
}