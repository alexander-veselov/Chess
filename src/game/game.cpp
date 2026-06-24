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
  // TODO: implement castling
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

static void GetMoves(const State& state, Square square, std::vector<Square>& moves) {
  const auto piece = state.board[square.rank][square.file];
  switch (piece) {
  case Piece::kWhiteKing:
  case Piece::kBlackKing:
    GetKingMoves(state, square, moves);
    break;
  case Piece::kWhiteQueen:
  case Piece::kBlackQueen:
    GetQueenMoves(state, square, moves);
    break;
  case Piece::kWhiteRook:
  case Piece::kBlackRook:
    GetRookMoves(state, square, moves);
    break;
  case Piece::kWhiteBishop:
  case Piece::kBlackBishop:
    GetBishopMoves(state, square, moves);
    break;
  case Piece::kWhiteKnight:
  case Piece::kBlackKnight:
    GetKnightMoves(state, square, moves);
    break;
  case Piece::kWhitePawn:
  case Piece::kBlackPawn:
    GetPawnMoves(state, square, moves);
    break;
  }
}

static bool IsInCheck(const State& state, Color turn) {
  const auto opponentColor = SwitchColor(turn);
  const auto king = turn == Color::kWhite ? Piece::kWhiteKing : Piece::kBlackKing;
  for (auto rank = 0; rank < kBoardSize; ++rank) {
    for (auto file = 0; file < kBoardSize; ++file) {
      if (GetPieceColor(state.board[rank][file]) == opponentColor) {
        auto moves = std::vector<Square>{};
        GetMoves(state, Square{static_cast<Rank>(rank), static_cast<File>(file)}, moves);
        for (const auto& move : moves) {
          if (state.board[move.rank][move.file] == king) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

static bool CanMoveInTurn(const State& state, Square square) {
  return GetPieceColor(Get(state.board, square)) == state.turn;
}

static void Move(State& state, Square from, Square to) {
  MoveOrCapture(state.board, from, to);
  if (state.enPassant.has_value() && to == state.enPassant) {
    EnPassantCapture(state.board, from, to);
  }
  state.turn = SwitchColor(state.turn);
  state.enPassant = EvaluateEnPassant(state.board, from, to);
}

static std::vector<Square> GetLegalMoves(const State& state, Square square) {
  auto& board = state.board;
  auto moves = std::vector<Square>{};
  auto legalMoves = std::vector<Square>{};
  if (!CanMoveInTurn(state, square)) {
    return moves;
  }
  GetMoves(state, square, moves);
  for (const auto move : moves) {
    auto newState = State{state};
    Move(newState, square, move);
    if (!IsInCheck(newState, state.turn)) {
      legalMoves.push_back(move);
    }
  }
  return legalMoves;
}

static bool CanMove(const State& state, Square square) {
  return !GetLegalMoves(state, square).empty();
}

static bool HasAvailableMoves(const State& state) {
  for (auto rank = 0; rank < kBoardSize; ++rank) {
    for (auto file = 0; file < kBoardSize; ++file) {
      if (GetPieceColor(state.board[rank][file]) == state.turn) {
        if (CanMove(state, Square{static_cast<Rank>(rank), static_cast<File>(file)})) {
          return true;
        }
      }
    }
  }
  return false;
}

static Status GetStatus(const State& state) {
  const auto isInCheck = IsInCheck(state, state.turn);
  const auto hasAvailableMoves = HasAvailableMoves(state);
  if (!hasAvailableMoves) {
    if (isInCheck) {
      return state.turn == Color::kWhite ? Status::kBlackWon : Status::kWhiteWon;
    } else {
      return Status::kDraw;
    }
  }

  // TODO: implement draw by repetition/insufficient material/etc

  return state.turn == Color::kWhite ? Status::kWhiteToMove : Status::kBlackToMove;
}

static bool LegalMove(State& state, Square from, Square to) {
  if (from == to || !CanMoveInTurn(state, from)) {
    return false;
  }
  auto& board = state.board;
  for (const auto move : GetLegalMoves(state, from)) {
    if (move == to) {
      Move(state, from, to);
      return true;
    }
  }
  return false;
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

const Status Game::GetStatus() const {
  return chess::GetStatus(state_);
}

bool Game::CanMove(Square square) const {
  return chess::CanMove(state_, square);
}

bool Game::IsInCheck() const {
  return chess::IsInCheck(state_, state_.turn);
}

bool Game::Move(Square from, Square to) {
  return chess::LegalMove(state_, from, to);
}

std::vector<Square> Game::GetLegalMoves(Square square) const {
  return chess::GetLegalMoves(state_, square);
}

} // namespace chess