#include "chess/core/fen.h"
#include "chess/core/square.h"
#include "chess/core/piece.h"

namespace chess {

static Piece CharacterToPiece(char character) {
  switch (character) {
  case 'r':
    return Piece::kBlackRook;
  case 'n':
    return Piece::kBlackKnight;
  case 'b':
    return Piece::kBlackBishop;
  case 'q':
    return Piece::kBlackQueen;
  case 'k':
    return Piece::kBlackKing;
  case 'p':
    return Piece::kBlackPawn;
  case 'R':
    return Piece::kWhiteRook;
  case 'N':
    return Piece::kWhiteKnight;
  case 'B':
    return Piece::kWhiteBishop;
  case 'Q':
    return Piece::kWhiteQueen;
  case 'K':
    return Piece::kWhiteKing;
  case 'P':
    return Piece::kWhitePawn;
  }
  return Piece::kNone;
}

static Square IndexToSquare(size_t index) {
  return Square{
      static_cast<Rank>((kBoardSize * kBoardSize - index - 1) / kBoardSize),
      static_cast<File>(kBoardSize - (kBoardSize * kBoardSize - index - 1) % kBoardSize - 1),
  };
}

static size_t CharacterToDigit(char character) {
  return static_cast<size_t>(character - '0');
}

static Color CharacterToColor(char character) {
  switch (character) {
  case 'w':
    return Color::kWhite;
  case 'b':
    return Color::kBlack;
  }
  return Color::kNone;
}

static Rank CharacterToRank(char character) {
  return static_cast<Rank>(CharacterToDigit(character) - 1);
}

static File CharacterToFile(char character) {
  switch (character) {
  case 'a':
    return File::_A;
  case 'b':
    return File::_B;
  case 'c':
    return File::_C;
  case 'd':
    return File::_D;
  case 'e':
    return File::_E;
  case 'f':
    return File::_F;
  case 'g':
    return File::_G;
  case 'h':
    return File::_H;
  }

  // TODO: print error
  return File::_A;
}

static bool ParseCastlingRights(State& state, char character) {
  switch (character) {
  case 'q':
    state.blackLongCastleAllowed = true;
    break;
  case 'k':
    state.blackShortCastleAllowed = true;
    break;
  case 'Q':
    state.whiteLongCastleAllowed = true;
    break;
  case 'K':
    state.whiteShortCastleAllowed = true;
    break;
  default:
    return false;
  }
  return true;
}

State StateFromFEN(const std::string& fen) {
  auto state = State{};
  auto characterIndex = size_t{0};

  // Piece placement
  auto squareIndex = size_t{0};
  for (; characterIndex < fen.size(); ++characterIndex) {
    const auto character = fen[characterIndex];
    if (character == ' ') {
      characterIndex += 1;
      break;
    }
    if (character == '/') {
      continue;
    }
    if (std::isalpha(character)) {
      const auto square = IndexToSquare(squareIndex++);
      state.board[square.rank][square.file] = CharacterToPiece(character);
    } else if (std::isdigit(character)) {
      squareIndex += CharacterToDigit(character);
    }
  }

  // Active color
  state.turn = CharacterToColor(fen[characterIndex]);
  characterIndex += 2;

  // Castling rights
  state.whiteShortCastleAllowed = false;
  state.whiteLongCastleAllowed = false;
  state.blackShortCastleAllowed = false;
  state.blackLongCastleAllowed = false;
  if (fen[characterIndex] == '-') {
    characterIndex += 2;
  } else {
    for (; characterIndex < fen.size(); ++characterIndex) {
      if (!ParseCastlingRights(state, fen[characterIndex])) {
        break;
      }
    }
    characterIndex += 1;
  }
  
  // Possible en passant targets
  if (fen[characterIndex] == '-') {
    state.enPassant = std::nullopt;
    characterIndex += 2;
  } else {
    const auto file = CharacterToFile(fen[characterIndex + 0]);
    const auto rank = CharacterToRank(fen[characterIndex + 1]);
    state.enPassant = Square{rank, file};
    characterIndex += 3;
  }

  // Halfmove clock
  state.halfmoveClock = std::atoi(fen.data() + characterIndex);
  auto halfmoveClockStartIndex = characterIndex;
  for (; characterIndex < fen.size(); ++characterIndex) {
    if (!std::isdigit(fen[characterIndex])) {
      break;
    }
  }

  // Fullmove number
  state.fullmoveNumber = std::atoi(fen.data() + characterIndex);

  return state;
}

} // namespace chess