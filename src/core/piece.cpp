#include "chess/core/piece.h"

namespace chess {

Color GetPieceColor(Piece piece) {
  switch (piece) {
  case Piece::kWhiteKing:
  case Piece::kWhiteQueen:
  case Piece::kWhiteRook:
  case Piece::kWhiteBishop:
  case Piece::kWhiteKnight:
  case Piece::kWhitePawn:
    return Color::kWhite;
  case Piece::kBlackKing:
  case Piece::kBlackQueen:
  case Piece::kBlackRook:
  case Piece::kBlackBishop:
  case Piece::kBlackKnight:
  case Piece::kBlackPawn:
    return Color::kBlack;
  }
  return Color::kNone;
}

} // namespace chess