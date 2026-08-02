#include "chess/core/square.h"

namespace chess {

std::string FileToString(File file) {
  return std::string(1, static_cast<char>('a' + static_cast<U8>(file)));
}

bool ParseFile(std::string_view string, File& file) {
  if (string.size() != 1) {
    return false;
  }

  const auto c = string[0];
  if (c < 'a' || c > 'h') {
    return false;
  }

  file = static_cast<File>(c - 'a');
  return true;
}

std::string RankToString(Rank rank) {
  return std::string(1, static_cast<char>('1' + static_cast<U8>(rank)));
}

bool ParseRank(std::string_view string, Rank& rank) {
  if (string.size() != 1) {
    return false;
  }

  const auto c = string[0];
  if (c < '1' || c > '8') {
    return false;
  }

  rank = static_cast<Rank>(c - '1');
  return true;
}

std::string SquareToString(Square square) {
  return FileToString(GetFile(square)) + RankToString(GetRank(square));
}

bool ParseSquare(std::string_view string, Square& square) {
  if (string.size() != 2) {
    return false;
  }
  auto file = File{};
  if (!ParseFile(string.substr(0, 1), file)) {
    return false;
  }
  auto rank = Rank{};
  if (!ParseRank(string.substr(1, 1), rank)) {
    return false;
  }
  square = CreateSquare(file, rank);
  return true;
}

} // namespace chess