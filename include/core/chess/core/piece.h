#pragma once

#include <cstdint>

namespace chess {

enum class Color : uint8_t {
  kNone,
  kWhite,
  kBlack
};

enum class BasePiece : uint8_t {
  kNone,
  kKing,
  kQueen,
  kRook,
  kBishop,
  kKnight,
  kPawn,
};

constexpr uint8_t MAKE_PIECE(Color color, BasePiece basePiece) {
  return (static_cast<uint8_t>(basePiece) << 2) + static_cast<uint8_t>(color);
}

enum class Piece : uint8_t {
  kNone        = MAKE_PIECE(Color::kNone,  BasePiece::kNone),
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
};

constexpr Piece MakePiece(Color color, BasePiece basePiece) {
  return static_cast<Piece>(MAKE_PIECE(color, basePiece));
}

constexpr Color GetPieceColor(Piece piece) {
  return static_cast<Color>(static_cast<uint8_t>(piece) & 0b11);
}

constexpr BasePiece GetBasePiece(Piece piece) {
  return static_cast<BasePiece>(static_cast<uint8_t>(piece) >> 2);
}

constexpr Color SwitchColor(Color color) {
  switch (color) {
  case Color::kWhite:
    return Color::kBlack;
  case Color::kBlack:
    return Color::kWhite;
  }
  return Color::kNone;
}

}