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

static char PieceToCharacter(Piece piece) {
  switch (piece) {
  case Piece::kBlackRook:
    return 'r';
  case Piece::kBlackKnight:
    return 'n';
  case Piece::kBlackBishop:
    return 'b';
  case Piece::kBlackQueen:
    return 'q';
  case Piece::kBlackKing:
    return 'k';
  case Piece::kBlackPawn:
    return 'p';
  case Piece::kWhiteRook:
    return 'R';
  case Piece::kWhiteKnight:
    return 'N';
  case Piece::kWhiteBishop:
    return 'B';
  case Piece::kWhiteQueen:
    return 'Q';
  case Piece::kWhiteKing:
    return 'K';
  case Piece::kWhitePawn:
    return 'P';
  }
  return '\0';
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

static char ColorToCharacter(Color color) {
  switch (color) {
  case Color::kWhite:
    return 'w';
  case Color::kBlack:
    return 'b';
  }
  return '\0';
}

static Rank CharacterToRank(char character) {
  return static_cast<Rank>(CharacterToDigit(character) - 1);
}

static File CharacterToFile(char character) {
  return static_cast<File>(character - 'a' + File::_A);
}

static char RankToCharacter(Rank rank) {
  return '1' + rank;
}

static char FileToCharacter(File file) {
  return 'a' + file;
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

static std::string CastlingRightsToString(const State& state) {
  auto result = std::string{};
  if (state.whiteShortCastleAllowed) {
    result += 'K';
  }
  if (state.whiteLongCastleAllowed) {
    result += 'Q';
  }
  if (state.blackShortCastleAllowed) {
    result += 'k';
  }
  if (state.blackLongCastleAllowed) {
    result += 'q';
  }
  if (result.empty()) {
    return "-";
  }
  return result;
}

static Square IndexToSquare(size_t index) {
  return MakeSquare(
      static_cast<File>(kBoardSize - (kBoardSize * kBoardSize - index - 1) % kBoardSize - 1),
      static_cast<Rank>((kBoardSize * kBoardSize - index - 1) / kBoardSize));
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
      state.board[square] = CharacterToPiece(character);
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
    state.enPassant = MakeSquare(file, rank);
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

std::string FENFromState(const State& state) {
  auto fen = std::string{};
  for (auto rank = int32_t{chess::Rank::_8}; rank >= int32_t{chess::Rank::_1}; --rank) {
    auto emptySquares = 0;
    for (auto file = int32_t{chess::File::_A}; file <= int32_t{chess::File::_H}; ++file) {
      const auto square = MakeSquare(static_cast<File>(file), static_cast<Rank>(rank));
      const auto piece = state.board[square];
      if (piece == Piece::kNone) {
        ++emptySquares;
      } else {
        if (emptySquares != 0) {
          fen += std::to_string(emptySquares);
          emptySquares = 0;
        }
        fen += PieceToCharacter(piece);
      }
    }
    if (emptySquares != 0) {
      fen += std::to_string(emptySquares);
      emptySquares = 0;
    }
    if (rank != int32_t{chess::Rank::_1}) {
      fen += '/';
    }
  }

  fen += " ";
  fen += ColorToCharacter(state.turn);

  fen += " ";
  fen += CastlingRightsToString(state);

  fen += " ";
  if (state.enPassant.has_value()) {
    fen += FileToCharacter(GetFile(state.enPassant.value()));
    fen += RankToCharacter(GetRank(state.enPassant.value()));
  } else {
    fen += "-";
  }

  fen += " ";
  fen += std::to_string(state.halfmoveClock);

  fen += " ";
  fen += std::to_string(state.fullmoveNumber);
  
  return fen;
}

} // namespace chess