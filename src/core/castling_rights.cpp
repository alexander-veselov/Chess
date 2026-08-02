#include "chess/core/castling_rights.h"

namespace chess {

std::string CastlingRightToString(CastlingRight castlingRight) {
  switch (castlingRight) {
  case CastlingRight::kWhiteKingSide:
    return "K";
  case CastlingRight::kWhiteQueenSide:
    return "Q";
  case CastlingRight::kBlackKingSide:
    return "k";
  case CastlingRight::kBlackQueenSide:
    return "q";
  }
  return "";
}

bool ParseCastlingRight(std::string_view string, CastlingRight& castlingRight) {
  if (string.size() != 1) {
    return false;
  }

  switch (string[0]) {
  case 'K':
    castlingRight = CastlingRight::kWhiteKingSide;
    return true;
  case 'Q':
    castlingRight = CastlingRight::kWhiteQueenSide;
    return true;
  case 'k':
    castlingRight = CastlingRight::kBlackKingSide;
    return true;
  case 'q':
    castlingRight = CastlingRight::kBlackQueenSide;
    return true;
  }

  return false;
}

std::string CastlingRightsMaskToString(CastlingRightsMask mask) {
  auto string = std::string{};
  for (auto rightIndex = 1; rightIndex <= 8; rightIndex <<= 1) {
    const auto right = static_cast<CastlingRight>(rightIndex);
    if (CanCastle(mask, right)) {
      string += CastlingRightToString(right);
    }
  }
  return string.empty() ? "-" : string;
}

} // namespace chess