#include "chess/game/game.h"

namespace chess {

static Board CreateDefaultBoard() {
  auto board = Board{};

  for (auto& row : board) {
    for (auto& c : row) {
      c = Piece::kNone;
    }
  }

  board[_8][_A] = Piece::kBlackRook;
  board[_8][_B] = Piece::kBlackKnight;
  board[_8][_C] = Piece::kBlackBishop;
  board[_8][_D] = Piece::kBlackQueen;
  board[_8][_E] = Piece::kBlackKing;
  board[_8][_F] = Piece::kBlackBishop;
  board[_8][_G] = Piece::kBlackKnight;
  board[_8][_H] = Piece::kBlackRook;

  for (auto i = 0; i < kBoardSize; ++i) {
    board[_7][i] = Piece::kBlackPawn;
  }

  for (auto i = 0; i < kBoardSize; ++i) {
    board[_2][i] = Piece::kWhitePawn;
  }

  board[_1][_A] = Piece::kWhiteRook;
  board[_1][_B] = Piece::kWhiteKnight;
  board[_1][_C] = Piece::kWhiteBishop;
  board[_1][_D] = Piece::kWhiteQueen;
  board[_1][_E] = Piece::kWhiteKing;
  board[_1][_F] = Piece::kWhiteBishop;
  board[_1][_G] = Piece::kWhiteKnight;
  board[_1][_H] = Piece::kWhiteRook;

  return board;
}

static bool IsValidSquare(Square square) {
  return File::_A <= square.file && square.file <= File::_H &&
         Rank::_8 <= square.rank && square.rank <= Rank::_1;
}

static bool PushIfValid(std::vector<Square>& moves, Square square) {
  if (IsValidSquare(square)) {
    moves.push_back(square);
    return true;
  }
  return false;
}

static void GetKingMoves(const State& state, Square square,
                         std::vector<Square>& moves) {
  // finish capture, don't capture own pieces
  PushIfValid(moves, Square{(Rank)(square.rank - 1), (File)(square.file - 1)});
  PushIfValid(moves, Square{(Rank)(square.rank - 1), (File)(square.file + 0)});
  PushIfValid(moves, Square{(Rank)(square.rank - 1), (File)(square.file + 1)});
  PushIfValid(moves, Square{(Rank)(square.rank + 0), (File)(square.file - 1)});
  PushIfValid(moves, Square{(Rank)(square.rank + 0), (File)(square.file + 1)});
  PushIfValid(moves, Square{(Rank)(square.rank + 1), (File)(square.file - 1)});
  PushIfValid(moves, Square{(Rank)(square.rank + 1), (File)(square.file + 0)});
  PushIfValid(moves, Square{(Rank)(square.rank + 1), (File)(square.file + 1)});
}

static void GetRookMoves(const State& state, Square square,
                         std::vector<Square>& moves) {
  // finish capture, don't capture own pieces
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfValid(
            moves, Square{(Rank)(square.rank + i), (File)(square.file + 0)})) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfValid(
            moves, Square{(Rank)(square.rank - i), (File)(square.file + 0)})) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfValid(
            moves, Square{(Rank)(square.rank + 0), (File)(square.file + i)})) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfValid(
            moves, Square{(Rank)(square.rank + 0), (File)(square.file - i)})) {
      break;
    }
  }
}

static void GetBishopMoves(const State& state, Square square,
                           std::vector<Square>& moves) {
  // TODO: finish capture, don't capture own pieces
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfValid(
            moves, Square{(Rank)(square.rank + i), (File)(square.file + i)})) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfValid(
            moves, Square{(Rank)(square.rank + i), (File)(square.file - i)})) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfValid(
            moves, Square{(Rank)(square.rank - i), (File)(square.file - i)})) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfValid(
            moves, Square{(Rank)(square.rank - i), (File)(square.file + i)})) {
      break;
    }
  }
}

static void GetQueenMoves(const State& state, Square square,
                          std::vector<Square>& moves) {
  GetBishopMoves(state, square, moves);
  GetRookMoves(state, square, moves);
}

static void GetKnightMoves(const State& state, Square square,
                           std::vector<Square>& moves) {
  // TODO: finish capture, don't capture own pieces
  PushIfValid(moves, Square{(Rank)(square.rank + 1), (File)(square.file + 2)});
  PushIfValid(moves, Square{(Rank)(square.rank + 1), (File)(square.file - 2)});
  PushIfValid(moves, Square{(Rank)(square.rank - 1), (File)(square.file + 2)});
  PushIfValid(moves, Square{(Rank)(square.rank - 1), (File)(square.file - 2)});
  PushIfValid(moves, Square{(Rank)(square.rank + 2), (File)(square.file + 1)});
  PushIfValid(moves, Square{(Rank)(square.rank - 2), (File)(square.file + 1)});
  PushIfValid(moves, Square{(Rank)(square.rank + 2), (File)(square.file - 1)});
  PushIfValid(moves, Square{(Rank)(square.rank - 2), (File)(square.file - 1)});
}

static void GetPawnMoves(const State& state, Square square,
                         std::vector<Square>& moves) {
  // TODO: finish capture and en-passant
  const auto color = GetPieceColor(state.board[square.rank][square.file]);
  const auto direction = color == Color::kWhite ? -1 : 1;
  const auto defaultRank = color == Color::kWhite ? Rank::_2 : Rank::_7;
  PushIfValid(moves, Square{(Rank)(square.rank + direction), square.file});
  if (square.rank == defaultRank) {
    PushIfValid(moves,
                Square{(Rank)(square.rank + direction * 2), square.file});
  }
}

Game::Game()
  : state_{State{CreateDefaultBoard()}},
    turn_{Color::kWhite} {
}

Game::Game(const State& state, Color turn)
  : state_{state},
    turn_{turn} {
}

const State& Game::GetState() const {
  return state_;
}

bool Game::Move(Square from, Square to) {
  if (from.file == to.file && from.rank == to.rank) {
    return false;
  }
  auto& board = state_.board;
  for (const auto move : GetLegalMoves(from)) {
    if (move.file == to.file && move.rank == to.rank) {
      board[to.rank][to.file] = board[from.rank][from.file];
      board[from.rank][from.file] = Piece::kNone;
      return true;
    }
  }
  return false;
}

std::vector<Square> Game::GetLegalMoves(Square square) const {
  auto& board = state_.board;
  const auto piece = board[square.rank][square.file];
  auto moves = std::vector<Square>{};
  switch (piece) {
  case Piece::kWhiteKing:
  case Piece::kBlackKing:
    GetKingMoves(state_, square, moves);
    break;
  case Piece::kWhiteQueen:
  case Piece::kBlackQueen:
    GetQueenMoves(state_, square, moves);
    break;
  case Piece::kWhiteRook:
  case Piece::kBlackRook:
    GetRookMoves(state_, square, moves);
    break;
  case Piece::kWhiteBishop:
  case Piece::kBlackBishop:
    GetBishopMoves(state_, square, moves);
    break;
  case Piece::kWhiteKnight:
  case Piece::kBlackKnight:
    GetKnightMoves(state_, square, moves);
    break;
  case Piece::kWhitePawn:
  case Piece::kBlackPawn:
    GetPawnMoves(state_, square, moves);
    break;
  }
  return moves;
}

} // namespace chess