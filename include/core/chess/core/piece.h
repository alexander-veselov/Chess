#pragma once

#include "chess/core/types.h"
#include "chess/core/color.h"

#include <string>

namespace chess {

enum class BasePiece : U8 {
  kNone,
  kKing,
  kQueen,
  kRook,
  kBishop,
  kKnight,
  kPawn,
};

constexpr U8 MAKE_PIECE(Color color, BasePiece basePiece) {
  return (static_cast<U8>(color) << 3) + static_cast<U8>(basePiece);
}

enum Piece : U8 {
  kNone        = BasePiece::kNone,
  kWhiteKing   = MAKE_PIECE(Color::kWhite, BasePiece::kKing),
  kWhiteQueen  = MAKE_PIECE(Color::kWhite, BasePiece::kQueen),
  kWhiteRook   = MAKE_PIECE(Color::kWhite, BasePiece::kRook),
  kWhiteBishop = MAKE_PIECE(Color::kWhite, BasePiece::kBishop),
  kWhiteKnight = MAKE_PIECE(Color::kWhite, BasePiece::kKnight),
  kWhitePawn   = MAKE_PIECE(Color::kWhite, BasePiece::kPawn),
  kBlackKing   = MAKE_PIECE(Color::kBlack, BasePiece::kKing),
  kBlackQueen  = MAKE_PIECE(Color::kBlack, BasePiece::kQueen),
  kBlackRook   = MAKE_PIECE(Color::kBlack, BasePiece::kRook),
  kBlackBishop = MAKE_PIECE(Color::kBlack, BasePiece::kBishop),
  kBlackKnight = MAKE_PIECE(Color::kBlack, BasePiece::kKnight),
  kBlackPawn   = MAKE_PIECE(Color::kBlack, BasePiece::kPawn),
  kPieceCount
};

constexpr Piece MakePiece(Color color, BasePiece basePiece) {
  return static_cast<Piece>(MAKE_PIECE(color, basePiece));
}

constexpr Color GetPieceColor(Piece piece) {
  return static_cast<Color>(static_cast<U8>(piece) >> 3);
}

constexpr BasePiece GetBasePiece(Piece piece) {
  return static_cast<BasePiece>(static_cast<U8>(piece) & 0b111);
}

std::string PieceToString(Piece piece);
bool ParsePiece(std::string_view string, Piece& piece);

}