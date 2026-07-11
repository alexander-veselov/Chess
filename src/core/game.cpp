#include "chess/core/game.h"
#include "chess/core/fen.h"

namespace chess {

State CreateDefaultState() {
  return chess::StateFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

bool IsValidSquare(int rank, int file) {
  return File::_A <= file && file <= File::_H && Rank::_1 <= rank && rank <= Rank::_8;
}

Square ShiftSquare(Square square, int rankShift, int fileShift) {
  const auto rank = GetRank(square) + rankShift;
  const auto file = GetFile(square) + fileShift;
  if (!IsValidSquare(rank, file)) {
    return square;
  }
  return MakeSquare(static_cast<File>(file), static_cast<Rank>(rank));
}

void MoveOrCapture(Board& board, const Move& move) {
  if (move.promotion.has_value()) {
    board[move.to] = move.promotion.value();
  } else {
    board[move.to] = board[move.from];
  }
  board[move.from] = Piece::kNone;
}

void EnPassantCapture(Board& board, Square from, Square to) {
  if (GetBasePiece(board[to]) == BasePiece::kPawn) {
    const auto captureSquare = MakeSquare(GetFile(to), GetRank(from));
    board[captureSquare] = Piece::kNone;
  }
}

std::optional<Square> EvaluateEnPassant(const Board& board, Square from, Square to) {
  const auto piece = board[to];
  if (piece == Piece::kBlackPawn || piece == Piece::kWhitePawn) {
    const auto rankShift = GetRank(to) - GetRank(from);
    if (std::abs(rankShift) == 2) {
      return ShiftSquare(from, rankShift / 2, 0);
    }
  }
  return std::nullopt;
}

void PushPawn(std::vector<Move>& moves, Square fromSquare, Square toSquare, Color color) {
  const auto toSquareRank = GetRank(toSquare);
  if (toSquareRank == Rank::_1 || toSquareRank == Rank::_8) {
    moves.push_back(Move{fromSquare, toSquare, MakePiece(color, BasePiece::kBishop)});
    moves.push_back(Move{fromSquare, toSquare, MakePiece(color, BasePiece::kRook)});
    moves.push_back(Move{fromSquare, toSquare, MakePiece(color, BasePiece::kKnight)});
    moves.push_back(Move{fromSquare, toSquare, MakePiece(color, BasePiece::kQueen)});
  } else {
    moves.push_back(Move{fromSquare, toSquare, std::nullopt});
  }
}

bool PushPawnIfEmpty(std::vector<Move>& moves, const Board& board, Square fromSquare, int rankShift,
                     int fileShift) {
  const auto toSquare = ShiftSquare(fromSquare, rankShift, fileShift);
  const auto fromPiece = board[fromSquare];
  const auto toPiece = board[toSquare];

  if (toPiece == Piece::kNone) {
    PushPawn(moves, fromSquare, toSquare, GetPieceColor(fromPiece));
    return true;
  }

  return false;
}

void PushPawnIfOpposite(std::vector<Move>& moves, const Board& board, Square fromSquare,
                        int rankShift, int fileShift) {
  const auto toSquare = ShiftSquare(fromSquare, rankShift, fileShift);
  const auto fromPiece = board[fromSquare];
  const auto toPiece = board[toSquare];
  const auto fromPieceColor = GetPieceColor(fromPiece);

  if (toPiece == Piece::kNone) {
    return;
  }

  if (GetPieceColor(fromPiece) != GetPieceColor(toPiece)) {
    PushPawn(moves, fromSquare, toSquare, GetPieceColor(fromPiece));
  }
}

bool PushIfEmptyOrOpposite(std::vector<Move>& moves, const Board& board, Square fromSquare,
                           Square toSquare) {
  const auto fromPiece = board[fromSquare];
  const auto toPiece = board[toSquare];

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

bool PushIfEmptyOrOpposite(std::vector<Move>& moves, const Board& board, Square square,
                           int rankShift, int fileShift) {
  const auto shifted = ShiftSquare(square, rankShift, fileShift);
  if (shifted == square) {
    return false;
  }
  return PushIfEmptyOrOpposite(moves, board, square, shifted);
}

void GetKingMovesWithoutCastling(const State& state, Square square, std::vector<Move>& moves) {
  PushIfEmptyOrOpposite(moves, state.board, square, -1, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, +0);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, +0, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, +0, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, +0);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, +1);
}

bool IsAttacked(const State& state, Color turn, Square square) {
  const auto opponentColor = SwitchColor(turn);
  for (auto fromSquareIndex = 0; fromSquareIndex < kBoardSize * kBoardSize; ++fromSquareIndex) {
    const auto fromSquare = static_cast<Square>(fromSquareIndex);
    const auto piece = state.board[fromSquare];
    if (GetPieceColor(piece) == opponentColor) {
      auto moves = std::vector<Move>{};
      if (fromSquare == square) {
        continue;
      }
      const auto basePiece = GetBasePiece(piece);
      if (basePiece == BasePiece::kKing) {
        GetKingMovesWithoutCastling(state, fromSquare, moves);
      } else {
        GetMoves(state, fromSquare, moves);
        // Special case for pawns. A pawn cannot capture an empty square, but still attacks it
        if (basePiece == BasePiece::kPawn) {
          const auto direction = opponentColor == Color::kWhite ? 1 : -1;
          PushPawnIfEmpty(moves, state.board, fromSquare, direction, -1);
          PushPawnIfEmpty(moves, state.board, fromSquare, direction, +1);
        }
      }
      for (const auto& move : moves) {
        if (move.to == square) {
          return true;
        }
      }
    }
  }
  return false;
}

void GetKingMoves(const State& state, Square square, std::vector<Move>& moves) {
  GetKingMovesWithoutCastling(state, square, moves);
  if (!IsInCheck(state, state.turn)) {
    if (GetPieceColor(state.board[square]) == Color::kWhite) {
      if (state.whiteShortCastleAllowed) {
        if (state.board[F1] == Piece::kNone && state.board[G1] == Piece::kNone &&
            !IsAttacked(state, state.turn, F1)) {
          moves.push_back(Move{square, G1, std::nullopt});
        }
      }
      if (state.whiteLongCastleAllowed) {
        if (state.board[B1] == Piece::kNone && state.board[C1] == Piece::kNone &&
            state.board[D1] == Piece::kNone && !IsAttacked(state, state.turn, C1) &&
            !IsAttacked(state, state.turn, D1)) {
          moves.push_back(Move{square, C1, std::nullopt});
        }
      }
    } else {
      if (state.blackShortCastleAllowed) {
        if (state.board[F8] == Piece::kNone && state.board[G8] == Piece::kNone &&
            !IsAttacked(state, state.turn, F8)) {
          moves.push_back(Move{square, G8, std::nullopt});
        }
      }
      if (state.blackLongCastleAllowed) {
        if (state.board[B8] == Piece::kNone && state.board[C8] == Piece::kNone &&
            state.board[D8] == Piece::kNone && !IsAttacked(state, state.turn, C8) &&
            !IsAttacked(state, state.turn, D8)) {
          moves.push_back(Move{square, C8, std::nullopt});
        }
      }
    }
  }
}

void GetRookMoves(const State& state, Square square, std::vector<Move>& moves) {
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

void GetBishopMoves(const State& state, Square square, std::vector<Move>& moves) {
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

void GetQueenMoves(const State& state, Square square, std::vector<Move>& moves) {
  GetBishopMoves(state, square, moves);
  GetRookMoves(state, square, moves);
}

void GetKnightMoves(const State& state, Square square, std::vector<Move>& moves) {
  PushIfEmptyOrOpposite(moves, state.board, square, +1, +2);
  PushIfEmptyOrOpposite(moves, state.board, square, +1, -2);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, +2);
  PushIfEmptyOrOpposite(moves, state.board, square, -1, -2);
  PushIfEmptyOrOpposite(moves, state.board, square, +2, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, -2, +1);
  PushIfEmptyOrOpposite(moves, state.board, square, +2, -1);
  PushIfEmptyOrOpposite(moves, state.board, square, -2, -1);
}

void GetPawnMoves(const State& state, Square square, std::vector<Move>& moves) {
  const auto color = GetPieceColor(state.board[square]);
  const auto direction = color == Color::kWhite ? 1 : -1;
  const auto defaultRank = color == Color::kWhite ? Rank::_2 : Rank::_7;
  if (PushPawnIfEmpty(moves, state.board, square, direction, 0)) {
    if (GetRank(square) == defaultRank) {
      PushPawnIfEmpty(moves, state.board, square, direction * 2, 0);
    }
  }
  PushPawnIfOpposite(moves, state.board, square, direction, -1);
  PushPawnIfOpposite(moves, state.board, square, direction, +1);
  if (state.enPassant.has_value()) {
    const auto fileShift = GetFile(*state.enPassant) - GetFile(square);
    const auto rankShift = GetRank(*state.enPassant) - GetRank(square);
    if (std::abs(fileShift) == 1 && rankShift == direction) {
      PushIfEmptyOrOpposite(moves, state.board, square, state.enPassant.value());
    }
  }
}

void GetMoves(const State& state, Square square, std::vector<Move>& moves) {
  const auto piece = state.board[square];
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

bool IsInCheck(const State& state, Color turn) {
  const auto king = MakePiece(turn, BasePiece::kKing);
  for (auto squareIndex = 0; squareIndex < kBoardSize * kBoardSize; ++squareIndex) {
    const auto square = static_cast<Square>(squareIndex);
    if (state.board[square] == king) {
      const auto opponentColor = SwitchColor(turn);

      const static auto bishopSigns =
          std::vector<std::pair<int8_t, int8_t>>{{+1, +1}, {+1, -1}, {-1, -1}, {-1, +1}};
      for (const auto sign : bishopSigns) {
        for (auto i = 1; i < kBoardSize; ++i) {
          const auto toSquare = ShiftSquare(square, sign.first * i, sign.second * i);
          if (toSquare == square) {
            break;
          }
          const auto piece = state.board[toSquare];
          const auto pieceColor = GetPieceColor(piece);
          const auto basePiece = GetBasePiece(piece);
          if (pieceColor == opponentColor) {
            if (basePiece == BasePiece::kBishop || basePiece == BasePiece::kQueen) {
              return true;
            }
            if (i == 1 && basePiece == BasePiece::kKing) {
              return true;
            }
          }
          if (pieceColor != Color::kNone) {
            break;
          }
        }
      }

      const static auto rookSigns =
          std::vector<std::pair<int8_t, int8_t>>{{+1, 0}, {-1, 0}, {0, +1}, {0, -1}};
      for (const auto sign : rookSigns) {
        for (auto i = 1; i < kBoardSize; ++i) {
          const auto toSquare = ShiftSquare(square, sign.first * i, sign.second * i);
          if (toSquare == square) {
            break;
          }
          const auto piece = state.board[toSquare];
          const auto pieceColor = GetPieceColor(piece);
          const auto basePiece = GetBasePiece(piece);
          if (pieceColor == opponentColor) {
            if (basePiece == BasePiece::kRook || basePiece == BasePiece::kQueen) {
              return true;
            }
            if (i == 1 && basePiece == BasePiece::kKing) {
              return true;
            }
          }
          if (pieceColor != Color::kNone) {
            break;
          }
        }
      }

      const static auto knightShifts = std::vector<std::pair<int8_t, int8_t>>{
          {+1, +2}, {+1, -2}, {-1, +2}, {-1, -2}, {+2, +1}, {-2, +1}, {+2, -1}, {-2, -1}};
      for (const auto shift : knightShifts) {
        const auto toSquare = ShiftSquare(square, shift.first, shift.second);
        const auto piece = state.board[toSquare];
        const auto pieceColor = GetPieceColor(piece);
        const auto basePiece = GetBasePiece(piece);
        if (pieceColor == opponentColor) {
          if (basePiece == BasePiece::kKnight) {
            return true;
          }
        }
      }

      const auto direction = turn == Color::kWhite ? 1 : -1;
      for (auto sign : {-1, +1}) {
        const auto toSquare = ShiftSquare(square, direction, sign);
        const auto piece = state.board[toSquare];
        const auto pieceColor = GetPieceColor(piece);
        const auto basePiece = GetBasePiece(piece);
        if (pieceColor == opponentColor) {
          if (basePiece == BasePiece::kPawn) {
            return true;
          }
        }
      }

      break;
    }
  }
  return false;
}

bool CanMoveInTurn(const State& state, Square square) {
  return GetPieceColor(state.board[square]) == state.turn;
}

void UpdateCastlingState(State& state, const Move& move) {
  const auto fromPiece = state.board[move.from];
  const auto toPiece = state.board[move.to];
  const auto fromBasePiece = GetBasePiece(fromPiece);
  const auto toBasePiece = GetBasePiece(toPiece);
  if (fromBasePiece == BasePiece::kKing) {
    if (state.turn == Color::kWhite) {
      state.whiteShortCastleAllowed = false;
      state.whiteLongCastleAllowed = false;
    } else {
      state.blackShortCastleAllowed = false;
      state.blackLongCastleAllowed = false;
    }
  } else if (fromBasePiece == BasePiece::kRook) {
    if (move.from == A1) {
      state.whiteLongCastleAllowed = false;
    } else if (move.from == H1) {
      state.whiteShortCastleAllowed = false;
    }
    if (move.from == A8) {
      state.blackLongCastleAllowed = false;
    }
    if (move.from == H8) {
      state.blackShortCastleAllowed = false;
    }
  } else if (toBasePiece == BasePiece::kRook) {
    if (move.to == A1) {
      state.whiteLongCastleAllowed = false;
    } else if (move.to == H1) {
      state.whiteShortCastleAllowed = false;
    }
    if (move.to == A8) {
      state.blackLongCastleAllowed = false;
    }
    if (move.to == H8) {
      state.blackShortCastleAllowed = false;
    }
  }
}

void ProcessCastle(State& state, const Move& move) {
  const auto piece = state.board[move.to];
  const auto basePiece = GetBasePiece(piece);
  if (basePiece != BasePiece::kKing) {
    return;
  }
  const auto fileShift = GetFile(move.from) - GetFile(move.to);
  if (std::abs(fileShift) != 2) {
    return;
  }
  if (move.to == G1) {
    std::swap(state.board[H1], state.board[F1]);
  } else if (move.to == C1) {
    std::swap(state.board[A1], state.board[D1]);
  } else if (move.to == G8) {
    std::swap(state.board[H8], state.board[F8]);
  } else if (move.to == C8) {
    std::swap(state.board[A8], state.board[D8]);
  }
}

void MakeMove(State& state, const Move& move) {
  UpdateCastlingState(state, move);
  MoveOrCapture(state.board, move);
  if (state.enPassant.has_value() && move.to == state.enPassant) {
    EnPassantCapture(state.board, move.from, move.to);
  }
  ProcessCastle(state, move);
  state.turn = SwitchColor(state.turn);
  state.enPassant = EvaluateEnPassant(state.board, move.from, move.to);
}

void GetLegalMoves(const State& state, Square square, std::vector<Move>& legalMoves) {
  auto& board = state.board;
  auto moves = std::vector<Move>{};
  if (!CanMoveInTurn(state, square)) {
    return;
  }
  GetMoves(state, square, moves);
  for (const auto move : moves) {
    auto newState = State{state};
    MakeMove(newState, move);
    if (!IsInCheck(newState, state.turn)) {
      legalMoves.push_back(move);
    }
  }
}

void GetAllLegalMoves(const State& state, std::vector<Move>& legalMoves) {
  for (auto squareIndex = 0; squareIndex < kBoardSize * kBoardSize; ++squareIndex) {
    const auto square = static_cast<Square>(squareIndex);
    if (GetPieceColor(state.board[square]) == state.turn) {
      GetLegalMoves(state, square, legalMoves);
    }
  }
}

bool CanMove(const State& state, Square square) {
  auto legalMoves = std::vector<Move>{};
  GetLegalMoves(state, square, legalMoves);
  return !legalMoves.empty();
}

bool HasAvailableMoves(const State& state) {
  for (auto squareIndex = 0; squareIndex < kBoardSize * kBoardSize; ++squareIndex) {
    const auto square = static_cast<Square>(squareIndex);
    if (GetPieceColor(state.board[square]) == state.turn) {
      if (CanMove(state, square)) {
        return true;
      }
    }
  }
  return false;
}

Status GetStatus(const State& state) {
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

bool LegalMove(State& state, const Move& move) {
  if (move.from == move.to || !CanMoveInTurn(state, move.from)) {
    return false;
  }
  auto& board = state.board;
  auto legalMoves = std::vector<Move>{};
  GetLegalMoves(state, move.from, legalMoves);
  for (const auto legalMove : legalMoves) {
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
  auto legalMoves = std::vector<Move>{};
  chess::GetLegalMoves(state_, square, legalMoves);
  return legalMoves;
}

} // namespace chess