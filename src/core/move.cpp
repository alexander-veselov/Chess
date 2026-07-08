#include "chess/core/move.h"

namespace chess {

bool operator==(const Move& move1, const Move& move2) {
  return move1.from == move2.from && move1.to == move2.to && move1.promotion == move2.promotion;
}

std::string MoveToString(const Move& move) {
  auto result = std::string{};

  result += static_cast<char>('a' + move.from.file);
  result += static_cast<char>('1' + move.from.rank);

  result += static_cast<char>('a' + move.to.file);
  result += static_cast<char>('1' + move.to.rank);

  if (move.promotion.has_value()) {
    switch (*move.promotion) {
    case Piece::kWhiteKnight:
    case Piece::kBlackKnight:
      result += 'n';
      break;
    case Piece::kWhiteBishop:
    case Piece::kBlackBishop:
      result += 'b';
      break;
    case Piece::kWhiteRook:
    case Piece::kBlackRook:
      result += 'r';
      break;
    case Piece::kWhiteQueen:
    case Piece::kBlackQueen:
      result += 'q';
      break;
    default:
      break;
    }
  }

  return result;
}

bool ParseMove(const std::string& string, Move& move) {
  if (string.size() != 4 && string.size() != 5) {
    return false;
  }

  move.from.file = static_cast<File>(string[0] - 'a');
  move.from.rank = static_cast<Rank>(string[1] - '1');
  move.to.file = static_cast<File>(string[2] - 'a');
  move.to.rank = static_cast<Rank>(string[3] - '1');

  if (string.size() == 5) {
    switch (string[4]) {
    case 'q':
      move.promotion = Piece::kWhiteQueen;
      break;
    case 'r':
      move.promotion = Piece::kWhiteRook;
      break;
    case 'b':
      move.promotion = Piece::kWhiteBishop;
      break;
    case 'n':
      move.promotion = Piece::kWhiteKnight;
      break;
    default:
      return false;
    }
  }

  return true;
}

} // namespace chess