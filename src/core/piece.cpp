#include "chess/core/piece.h"

namespace chess {

std::string PieceToString(Piece piece) {
  switch (piece) {
  case Piece::kBlackRook:
    return "r";
  case Piece::kBlackKnight:
    return "n";
  case Piece::kBlackBishop:
    return "b";
  case Piece::kBlackQueen:
    return "q";
  case Piece::kBlackKing:
    return "k";
  case Piece::kBlackPawn:
    return "p";
  case Piece::kWhiteRook:
    return "R";
  case Piece::kWhiteKnight:
    return "N";
  case Piece::kWhiteBishop:
    return "B";
  case Piece::kWhiteQueen:
    return "Q";
  case Piece::kWhiteKing:
    return "K";
  case Piece::kWhitePawn:
    return "P";
  }
  return "";
}

bool ParsePiece(std::string_view string, Piece& piece) {
  if (string.size() != 1) {
    return false;
  }
  switch (string[0]) {
  case 'r':
    piece = Piece::kBlackRook;
    return true;
  case 'n':
    piece = Piece::kBlackKnight;
    return true;
  case 'b':
    piece = Piece::kBlackBishop;
    return true;
  case 'q':
    piece = Piece::kBlackQueen;
    return true;
  case 'k':
    piece = Piece::kBlackKing;
    return true;
  case 'p':
    piece = Piece::kBlackPawn;
    return true;
  case 'R':
    piece = Piece::kWhiteRook;
    return true;
  case 'N':
    piece = Piece::kWhiteKnight;
    return true;
  case 'B':
    piece = Piece::kWhiteBishop;
    return true;
  case 'Q':
    piece = Piece::kWhiteQueen;
    return true;
  case 'K':
    piece = Piece::kWhiteKing;
    return true;
  case 'P':
    piece = Piece::kWhitePawn;
    return true;
  }
  return false;
}

} // namespace chess