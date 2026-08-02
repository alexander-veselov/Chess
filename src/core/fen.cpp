#include "chess/core/fen.h"

#include "chess/core/color.h"
#include "chess/core/piece.h"
#include "chess/core/square.h"

namespace chess {
namespace {

size_t CharacterToDigit(char character) {
  return static_cast<size_t>(character - '0');
}

bool ParseCastlingRights(State& state, char character) {
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

std::string CastlingRightsToString(const State& state) {
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

Square IndexToSquare(size_t index) {
  return CreateSquare(
      static_cast<File>(kBoardSize - (kBoardSize * kBoardSize - index - 1) % kBoardSize - 1),
      static_cast<Rank>((kBoardSize * kBoardSize - index - 1) / kBoardSize));
}

} // namespace

bool ParseFEN(std::string_view fen, State& state) {

  // TODO: add more checks

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
      if (!ParsePiece(fen.substr(characterIndex, 1), state.board[square])) {
        return false;
      }
    } else if (std::isdigit(character)) {
      squareIndex += CharacterToDigit(character);
    }
  }

  // Active color
  if (!ParseColor(fen.substr(characterIndex, 1), state.turn)) {
    return false;
  }
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
    state.enPassant = Square::kInvalid;
    characterIndex += 2;
  } else {
    if (!ParseSquare(fen.substr(characterIndex, 2), state.enPassant)) {
      return false;
    }
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

  FillBitboardsFromBoard(state);

  return true;
}

std::string StateToFEN(const State& state) {
  auto fen = std::string{};
  for (auto rank = I32{chess::Rank::_8}; rank >= I32{chess::Rank::_1}; --rank) {
    auto emptySquares = 0;
    for (auto file = I32{chess::File::_A}; file <= I32{chess::File::_H}; ++file) {
      const auto square = CreateSquare(static_cast<File>(file), static_cast<Rank>(rank));
      const auto piece = state.board[square];
      if (piece == Piece::kNone) {
        ++emptySquares;
      } else {
        if (emptySquares != 0) {
          fen += std::to_string(emptySquares);
          emptySquares = 0;
        }
        fen += PieceToString(piece);
      }
    }
    if (emptySquares != 0) {
      fen += std::to_string(emptySquares);
      emptySquares = 0;
    }
    if (rank != I32{chess::Rank::_1}) {
      fen += '/';
    }
  }

  fen += " ";
  fen += ColorToString(state.turn);

  fen += " ";
  fen += CastlingRightsToString(state);

  fen += " ";
  if (state.enPassant != Square::kInvalid) {
    fen += FileToString(GetFile(state.enPassant));
    fen += RankToString(GetRank(state.enPassant));
  } else {
    fen += "-";
  }

  fen += " ";
  fen += std::to_string(state.halfmoveClock);

  fen += " ";
  fen += std::to_string(state.fullmoveNumber);

  return fen;
}

State StateFromFEN(std::string_view fen) {
  auto state = State{};
  if (!ParseFEN(fen, state)) {
    throw std::exception("invalid FEN string");
  }
  return state;
}

} // namespace chess