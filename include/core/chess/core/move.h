#pragma once

#include "chess/core/piece.h"
#include "chess/core/square.h"

#include <string>

namespace chess {

enum class MoveType : U8 {
  kNormal,
  kEnPassant,
  kKingCastle,
  kQueenCastle,
  kQueenPromotion,
  kRookPromotion,
  kBishopPromotion,
  kKnightPromotion,
};

using Move = U16; // [type, to, from]

constexpr Move CreateMove(Square from, Square to, MoveType type = MoveType::kNormal) {
  return from | (to << 6) | (static_cast<Move>(type) << 12);
}

constexpr Square GetFrom(Move move) {
  return static_cast<Square>(move & 0b111111);
}

constexpr Square GetTo(Move move) {
  return static_cast<Square>((move >> 6) & 0b111111);
}

constexpr MoveType GetType(Move move) {
  return static_cast<MoveType>((move >> 12) & 0b1111);
}

constexpr bool IsPromotion(MoveType type) {
  switch (type) {
  case MoveType::kKnightPromotion:
  case MoveType::kBishopPromotion:
  case MoveType::kRookPromotion:
  case MoveType::kQueenPromotion:
    return true;
  }
  return false;
}

std::string MoveToString(Move move);
bool ParseMove(const std::string& string, Move& move);

} // namespace chess