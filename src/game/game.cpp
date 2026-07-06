#include "chess/game/game.h"
#include "chess/core/fen.h"

namespace chess {

static State CreateDefaultState() {
  return StateFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
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

static void MoveOrCapture(Board& board, const Move& move) {
  if (move.promotion.has_value()) {
    board[move.to.rank][move.to.file] = move.promotion.value();
  } else {
    board[move.to.rank][move.to.file] = board[move.from.rank][move.from.file];
  }
  board[move.from.rank][move.from.file] = Piece::kNone;
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

static void PushPawn(std::vector<Move>& moves, Square fromSquare, Square toSquare, Color color) {
  if (toSquare.rank == Rank::_1 || toSquare.rank == Rank::_8) {
    moves.push_back(Move{fromSquare, toSquare, MakePiece(color, BasePiece::kBishop)});
    moves.push_back(Move{fromSquare, toSquare, MakePiece(color, BasePiece::kRook)});
    moves.push_back(Move{fromSquare, toSquare, MakePiece(color, BasePiece::kKnight)});
    moves.push_back(Move{fromSquare, toSquare, MakePiece(color, BasePiece::kQueen)});
  } else {
    moves.push_back(Move{fromSquare, toSquare, std::nullopt});
  }
}

static bool PushPawnIfEmpty(std::vector<Move>& moves, const Board& board, Square fromSquare,
                            int rankShift, int fileShift) {
  const auto toSquare = ShiftSquare(fromSquare, rankShift, fileShift);
  const auto fromPiece = Get(board, fromSquare);
  const auto toPiece = Get(board, toSquare);

  if (toPiece == Piece::kNone) {
    PushPawn(moves, fromSquare, toSquare, GetPieceColor(fromPiece));
    return true;
  }

  return false;
}

static void PushPawnIfOpposite(std::vector<Move>& moves, const Board& board, Square fromSquare,
                               int rankShift, int fileShift) {
  const auto toSquare = ShiftSquare(fromSquare, rankShift, fileShift);
  const auto fromPiece = Get(board, fromSquare);
  const auto toPiece = Get(board, toSquare);
  const auto fromPieceColor = GetPieceColor(fromPiece);

  if (toPiece == Piece::kNone) {
    return;
  }

  if (GetPieceColor(fromPiece) != GetPieceColor(toPiece)) {
    PushPawn(moves, fromSquare, toSquare, GetPieceColor(fromPiece));
  }
}

static bool PushIfEmptyOrOpposite(std::vector<Move>& moves, const Board& board, Square fromSquare,
                                  Square toSquare) {
  const auto fromPiece = Get(board, fromSquare);
  const auto toPiece = Get(board, toSquare);

  if (toPiece == Piece::kNone) {
    moves.push_back(Move{fromSquare, toSquare, std::nullopt});
    return true;
  }

  if (GetPieceColor(fromPiece) != GetPieceColor(toPiece)) {
    moves.push_back(Move{fromSquare, toSquare, std::nullopt});
    return false;
  }

  return false;
}

static bool PushIfEmptyOrOpposite(std::vector<Move>& moves, const Board& board, Square square,
                                  int rankShift, int fileShift) {
  return PushIfEmptyOrOpposite(moves, board, square, ShiftSquare(square, rankShift, fileShift));
}

static void GetMoves(const State& state, Square square, std::vector<Move>& moves);
static bool IsInCheck(const State& state, Color turn);

static void GetKingMovesWithoutCastling(const State& state, Square square, std::vector<Move>& moves) {
  PushIfEmptyOrOpposite(moves, state.board, square, -1, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, +0);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, +0, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, +0, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, +0);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, +1);
}

static bool IsAttacked(const State& state, Color turn, Square square) {
  const auto opponentColor = SwitchColor(turn);
  for (auto rank = 0; rank < kBoardSize; ++rank) {
    for (auto file = 0; file < kBoardSize; ++file) {
      const auto piece = state.board[rank][file];
      if (GetPieceColor(piece) == opponentColor) {
        auto moves = std::vector<Move>{};
        const auto toSquare = Square{static_cast<Rank>(rank), static_cast<File>(file)};
        if (toSquare == square) {
          continue;
        }
        if (GetBasePiece(piece) == BasePiece::kKing) {
          GetKingMovesWithoutCastling(state, toSquare, moves);
        } else {
          GetMoves(state, toSquare, moves);
        }
        for (const auto& move : moves) {
          if (move.to == square) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

static void GetKingMoves(const State& state, Square square, std::vector<Move>& moves) {
  GetKingMovesWithoutCastling(state, square, moves);
  if (!IsInCheck(state, state.turn)) {
    if (GetPieceColor(state.board[square.rank][square.file]) == Color::kWhite) {
      if (state.whiteShortCastleAllowed) {
        if (state.board[Rank::_1][File::_F] == Piece::kNone &&
            state.board[Rank::_1][File::_G] == Piece::kNone &&
            !IsAttacked(state, state.turn, Square{Rank::_1, File::_F})) {
          moves.push_back(Move{square, Square{Rank::_1, File::_G}, std::nullopt});
        }
      }
      if (state.whiteLongCastleAllowed) {
        if (state.board[Rank::_1][File::_B] == Piece::kNone &&
            state.board[Rank::_1][File::_C] == Piece::kNone &&
            state.board[Rank::_1][File::_D] == Piece::kNone &&
            !IsAttacked(state, state.turn, Square{Rank::_1, File::_C}) &&
            !IsAttacked(state, state.turn, Square{Rank::_1, File::_D})) {
          moves.push_back(Move{square, Square{Rank::_1, File::_C}, std::nullopt});
        }
      }
    } else {
      if (state.blackShortCastleAllowed) {
        if (state.board[Rank::_8][File::_F] == Piece::kNone &&
            state.board[Rank::_8][File::_G] == Piece::kNone &&
            !IsAttacked(state, state.turn, Square{Rank::_8, File::_F})) {
          moves.push_back(Move{square, Square{Rank::_8, File::_G}, std::nullopt});
        }
      }
      if (state.blackLongCastleAllowed) {
        if (state.board[Rank::_8][File::_B] == Piece::kNone &&
            state.board[Rank::_8][File::_C] == Piece::kNone &&
            state.board[Rank::_8][File::_D] == Piece::kNone &&
            !IsAttacked(state, state.turn, Square{Rank::_8, File::_C}) &&
            !IsAttacked(state, state.turn, Square{Rank::_8, File::_D})) {
          moves.push_back(Move{square, Square{Rank::_8, File::_C}, std::nullopt});
        }
      }
    }
  }
}

static void GetRookMoves(const State& state, Square square, std::vector<Move>& moves) {
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

static void GetBishopMoves(const State& state, Square square, std::vector<Move>& moves) {
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

static void GetQueenMoves(const State& state, Square square, std::vector<Move>& moves) {
  GetBishopMoves(state, square, moves);
  GetRookMoves(state, square, moves);
}

static void GetKnightMoves(const State& state, Square square, std::vector<Move>& moves) {
  PushIfEmptyOrOpposite(moves, state.board, square, +1, +2);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, -2);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, +2);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, -2);
  PushIfEmptyOrOpposite(moves, state.board, square, +2, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, -2, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, +2, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, -2, -1);
}

static void GetPawnMoves(const State& state, Square square, std::vector<Move>& moves) {
  const auto color = GetPieceColor(state.board[square.rank][square.file]);
  const auto direction = color == Color::kWhite ? 1 : -1;
  const auto defaultRank = color == Color::kWhite ? Rank::_2 : Rank::_7;
  if (PushPawnIfEmpty(moves, state.board, square, direction, 0)) {
    if (square.rank == defaultRank) {
      PushPawnIfEmpty(moves, state.board, square, direction * 2, 0);
    }
  }
  PushPawnIfOpposite(moves, state.board, square, direction, -1);
  PushPawnIfOpposite(moves, state.board, square, direction, +1);
  if (state.enPassant.has_value()) {
    const auto fileShift = state.enPassant->file - square.file;
    const auto rankShift = state.enPassant->rank - square.rank;
    if (std::abs(fileShift) == 1 && std::abs(rankShift) == 1) {
      PushIfEmptyOrOpposite(moves, state.board, square, state.enPassant.value());
    }
  }
}

static void GetMoves(const State& state, Square square, std::vector<Move>& moves) {
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
  for (auto rank = 0; rank < kBoardSize; ++rank) {
    for (auto file = 0; file < kBoardSize; ++file) {
      if (GetPieceColor(state.board[rank][file]) == opponentColor &&
          GetBasePiece(state.board[rank][file]) != BasePiece::kKing) {
        auto moves = std::vector<Move>{};
        GetMoves(state, Square{static_cast<Rank>(rank), static_cast<File>(file)}, moves);
        for (const auto& move : moves) {
          if (GetBasePiece(state.board[move.to.rank][move.to.file]) == BasePiece::kKing) {
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

static void UpdateCastlingState(State& state, const Move& move) {
  const auto piece = state.board[move.from.rank][move.from.file];
  const auto basePiece = GetBasePiece(piece);
  if (basePiece == BasePiece::kKing) {
    if (state.turn == Color::kWhite) {
      state.whiteShortCastleAllowed = false;
      state.whiteLongCastleAllowed = false;
    } else {
      state.blackShortCastleAllowed = false;
      state.blackLongCastleAllowed = false;
    }
  } else if (basePiece == BasePiece::kRook) {
    if (move.from == Square{Rank::_1, File::_A}) {
      state.whiteLongCastleAllowed = false;
    } else if (move.from == Square{Rank::_1, File::_H}) {
      state.whiteShortCastleAllowed = false;
    }
    if (move.from == Square{Rank::_8, File::_A}) {
      state.blackLongCastleAllowed = false;
    }
    if (move.from == Square{Rank::_8, File::_H}) {
      state.blackShortCastleAllowed = false;
    }
  }
}

static void ProcessCastle(State& state, const Move& move) {
  const auto piece = state.board[move.to.rank][move.to.file];
  const auto basePiece = GetBasePiece(piece);
  if (basePiece != BasePiece::kKing) {
    return;
  }
  const auto fileShift = move.from.file - move.to.file;
  if (std::abs(fileShift) != 2) {
    return;
  }
  if (move.to == Square{Rank::_1, File::_G}) {
    std::swap(state.board[Rank::_1][File::_H], state.board[Rank::_1][File::_F]);
  } else if (move.to == Square{Rank::_1, File::_C}) {
    std::swap(state.board[Rank::_1][File::_A], state.board[Rank::_1][File::_D]);
  } else if (move.to == Square{Rank::_8, File::_G}) {
    std::swap(state.board[Rank::_8][File::_H], state.board[Rank::_8][File::_F]);
  } else if (move.to == Square{Rank::_8, File::_C}) {
    std::swap(state.board[Rank::_8][File::_A], state.board[Rank::_8][File::_D]);
  }
}

static void MakeMove(State& state, const Move& move) {
  UpdateCastlingState(state, move);
  MoveOrCapture(state.board, move);
  if (state.enPassant.has_value() && move.to == state.enPassant) {
    EnPassantCapture(state.board, move.from, move.to);
  }
  ProcessCastle(state, move);
  state.turn = SwitchColor(state.turn);
  state.enPassant = EvaluateEnPassant(state.board, move.from, move.to);
}

static std::vector<Move> GetLegalMoves(const State& state, Square square) {
  auto& board = state.board;
  auto moves = std::vector<Move>{};
  auto legalMoves = std::vector<Move>{};
  if (!CanMoveInTurn(state, square)) {
    return moves;
  }
  GetMoves(state, square, moves);
  for (const auto move : moves) {
    auto newState = State{state};
    MakeMove(newState, move);
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

static bool LegalMove(State& state, const Move& move) {
  if (move.from == move.to || !CanMoveInTurn(state, move.from)) {
    return false;
  }
  auto& board = state.board;
  for (const auto legalMove : GetLegalMoves(state, move.from)) {
    if (legalMove == move) {
      MakeMove(state, legalMove);
      return true;
    }
  }
  return false;
}

Game::Game()
  : state_{CreateDefaultState()} {
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

bool Game::MakeMove(const Move& move) {
  return chess::LegalMove(state_, move);
}

std::vector<Move> Game::GetLegalMoves(Square square) const {
  return chess::GetLegalMoves(state_, square);
}

} // namespace chess