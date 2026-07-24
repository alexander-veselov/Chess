#include "chess/core/move.h"

namespace chess {

static std::string PromotionToString(MoveType type) {
  switch (type) {
  case MoveType::kKnightPromotion:
    return "n";
  case MoveType::kBishopPromotion:
    return "b";
  case MoveType::kRookPromotion:
    return "r";
  case MoveType::kQueenPromotion:
    return "q";
  }
  return "";
}

static bool ParsePromotion(char character, MoveType& type) {
  switch (character) {
  case 'q':
    type = MoveType::kQueenPromotion;
    break;
  case 'r':
    type = MoveType::kRookPromotion;
    break;
  case 'b':
    type = MoveType::kBishopPromotion;
    break;
  case 'n':
    type = MoveType::kKnightPromotion;
    break;
  default:
    return false;
  }
  return true;
}

std::string MoveToString(Move move) {
  auto result = std::string{};

  result += SquareToString(GetFrom(move));
  result += SquareToString(GetTo(move));
  result += PromotionToString(GetType(move));

  return result;
}

bool ParseMove(const std::string& string, Move& move) {
  if (string.size() != 4 && string.size() != 5) {
    return false;
  }

  auto from = Square{};
  if (!ParseSquare(string.substr(0, 2), from)) {
    return false;
  }
  auto to = Square{};
  if (!ParseSquare(string.substr(2, 2), to)) {
    return false;
  }

  auto type = MoveType::kNormal;
  if (string.size() == 5 && !ParsePromotion(string[4], type)) {
    return false;
  }

  move = CreateMove(from, to, type);

  return true;
}

} // namespace chess