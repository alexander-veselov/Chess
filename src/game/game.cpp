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

static bool IsValidSquare(int rank, int file) {
  return File::_A <= file && file <= File::_H && Rank::_1 <= rank && rank <= Rank::_8;
}

static Square ShiftSquare(Square square, int rankShift, int fileShift) {
  if (!IsValidSquare(square.rank + rankShift, square.file + fileShift)) {
    return square;
  }
  const auto rank = static_cast<Rank>(static_cast<int>(square.rank) + rankShift);
  const auto file = static_cast<File>(static_cast<int>(square.file) + fileShift);
  return Square{rank, file};
}

static Piece Get(const Board& board, Square square) {
  return board[square.rank][square.file];
}

static void MoveOrCapture(Board& board, Square from, Square to) {
  board[to.rank][to.file] = board[from.rank][from.file];
  board[from.rank][from.file] = Piece::kNone;
}

static void EnPassantCapture(Board& board, Square from, Square to) {
  const auto captureSquare = Square{from.rank, to.file};
  board[captureSquare.rank][captureSquare.file] = Piece::kNone;
}

static std::optional<Square> EvaluateEnPassant(const Board& board, Square from, Square to) {
  const auto piece = Get(board, to);
  if (piece == Piece::kBlackPawn || piece == Piece::kWhitePawn) {
    const auto rankShift = to.rank - from.rank;
    if (std::abs(rankShift) == 2) {
      return ShiftSquare(from, rankShift / 2, 0);
    }
  }
  return std::nullopt;
}

static bool PushIfEmpty(std::vector<Square>& moves, const Board& board, Square square,
                        int rankShift, int fileShift) {
  const auto toSquare = ShiftSquare(square, rankShift, fileShift);
  const auto fromPiece = Get(board, square);
  const auto toPiece = Get(board, toSquare);

  if (toPiece == Piece::kInvalid) {
    return false;
  }

  if (toPiece == Piece::kNone) {
    moves.push_back(toSquare);
    return true;
  }

  return false;
}

static void PushIfOpposite(std::vector<Square>& moves, const Board& board, Square square,
                           int rankShift, int fileShift) {
  const auto toSquare = ShiftSquare(square, rankShift, fileShift);
  const auto fromPiece = Get(board, square);
  const auto toPiece = Get(board, toSquare);

  if (toPiece == Piece::kNone || toPiece == Piece::kInvalid) {
    return;
  }

  if (GetPieceColor(fromPiece) != GetPieceColor(toPiece)) {
    moves.push_back(toSquare);
  }
}

static bool PushIfEmptyOrOpposite(std::vector<Square>& moves, const Board& board, Square fromSquare,
                                  Square toSquare) {
  const auto fromPiece = Get(board, fromSquare);
  const auto toPiece = Get(board, toSquare);

  if (toPiece == Piece::kInvalid) {
    return false;
  }

  if (toPiece == Piece::kNone) {
    moves.push_back(toSquare);
    return true;
  }

  if (GetPieceColor(fromPiece) != GetPieceColor(toPiece)) {
    moves.push_back(toSquare);
    return false;
  }

  return false;
}

static bool PushIfEmptyOrOpposite(std::vector<Square>& moves, const Board& board, Square square,
                                  int rankShift, int fileShift) {
  return PushIfEmptyOrOpposite(moves, board, square, ShiftSquare(square, rankShift, fileShift));
}

static void GetKingMoves(const State& state, Square square, std::vector<Square>& moves) {
  PushIfEmptyOrOpposite(moves, state.board, square, -1, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, +0);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, +0, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, +0, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, +0);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, +1);
}

static void GetRookMoves(const State& state, Square square, std::vector<Square>& moves) {
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfEmptyOrOpposite(moves, state.board, square, +i, 0)) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfEmptyOrOpposite(moves, state.board, square, -i, 0)) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfEmptyOrOpposite(moves, state.board, square, 0, +i)) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfEmptyOrOpposite(moves, state.board, square, 0, -i)) {
      break;
    }
  }
}

static void GetBishopMoves(const State& state, Square square, std::vector<Square>& moves) {
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfEmptyOrOpposite(moves, state.board, square, +i, +i)) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfEmptyOrOpposite(moves, state.board, square, +i, -i)) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfEmptyOrOpposite(moves, state.board, square, -i, -i)) {
      break;
    }
  }
  for (auto i = 1; i < kBoardSize; ++i) {
    if (!PushIfEmptyOrOpposite(moves, state.board, square, -i, +i)) {
      break;
    }
  }
}

static void GetQueenMoves(const State& state, Square square, std::vector<Square>& moves) {
  GetBishopMoves(state, square, moves);
  GetRookMoves(state, square, moves);
}

static void GetKnightMoves(const State& state, Square square, std::vector<Square>& moves) {
  PushIfEmptyOrOpposite(moves, state.board, square, +1, +2);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, -2);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, +2);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, -2);
  PushIfEmptyOrOpposite(moves, state.board, square, +2, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, -2, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, +2, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, -2, -1);
}

static void GetPawnMoves(const State& state, Square square, std::vector<Square>& moves) {
  const auto color = GetPieceColor(state.board[square.rank][square.file]);
  const auto direction = color == Color::kWhite ? 1 : -1;
  const auto defaultRank = color == Color::kWhite ? Rank::_2 : Rank::_7;
  if (PushIfEmpty(moves, state.board, square, direction, 0)) {
    if (square.rank == defaultRank) {
      PushIfEmpty(moves, state.board, square, direction * 2, 0);
    }
  }
  PushIfOpposite(moves, state.board, square, direction, -1);
  PushIfOpposite(moves, state.board, square, direction, +1);
  if (state.enPassant.has_value()) {
    const auto fileShift = state.enPassant->file - square.file;
    const auto rankShift = state.enPassant->rank - square.rank;
    if (std::abs(fileShift) == 1 && std::abs(rankShift) == 1) {
      PushIfEmptyOrOpposite(moves, state.board, square, state.enPassant.value());
    }
  }
}

Game::Game()
  : state_{State{CreateDefaultBoard(), Color::kWhite}} {
}

Game::Game(const State& state)
  : state_{state} {
}

const State& Game::GetState() const {
  return state_;
}

bool Game::CanMove(Square square) const {
  return GetPieceColor(Get(state_.board, square)) == state_.turn;
}

bool Game::Move(Square from, Square to) {
  if (from == to || !CanMove(from)) {
    return false;
  }
  auto& board = state_.board;
  for (const auto move : GetLegalMoves(from)) {
    if (move == to) {
      MoveOrCapture(state_.board, from, to);
      if (state_.enPassant.has_value() && to == state_.enPassant) {
        EnPassantCapture(state_.board, from, to);
      }
      state_.turn = SwitchColor(state_.turn);
      state_.enPassant = EvaluateEnPassant(state_.board, from, to);
      return true;
    }
  }
  return false;
}

std::vector<Square> Game::GetLegalMoves(Square square) const {
  auto& board = state_.board;
  const auto piece = board[square.rank][square.file];
  auto moves = std::vector<Square>{};
  if (!CanMove(square)) {
    return moves;
  }
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