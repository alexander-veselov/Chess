#include "chess/core/square.h"

namespace chess {

std::string SquareToString(Square square) {
  return {
    static_cast<char>('a' + GetFile(square)),
    static_cast<char>('1' + GetRank(square))
  };
}

bool ParseSquare(const std::string& string, Square& square) {
  if (string.size() != 2) {
    return false;
  }
  const auto fileChar = string[0];
  const auto rankChar = string[1];
  if (fileChar < 'a' || fileChar > 'h') {
    return false;
  }
  if (rankChar < '1' || rankChar > '8') {
    return false;
  }
  const auto file = static_cast<File>(fileChar - 'a');
  const auto rank = static_cast<Rank>(rankChar - '1');
  square = CreateSquare(file, rank);
  return true;
}


} // namespace chess