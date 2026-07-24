#include "chess/core/game.h"
#include "chess/core/fen.h"

namespace chess {

State CreateDefaultState() {
  auto state = chess::StateFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  FillBitboardsFromBoard(state);
  return state;
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
  return CreateSquare(static_cast<File>(file), static_cast<Rank>(rank));
}

static BasePiece PromotionTypeToBasePiece(MoveType type) {
  switch (type) {
  case MoveType::kKnightPromotion:
    return BasePiece::kKnight;
  case MoveType::kBishopPromotion:
    return BasePiece::kBishop;
  case MoveType::kRookPromotion:
    return BasePiece::kRook;
  case MoveType::kQueenPromotion:
    return BasePiece::kQueen;
  }
  return BasePiece::kNone;
}

static bool MoveOrCapture(Board& board, const Move& move, Color color) {
  const auto moveType = GetType(move);
  const auto isPromotion = IsPromotion(moveType);
  if (isPromotion) {
    board[GetTo(move)] = MakePiece(color, PromotionTypeToBasePiece(moveType));
  } else {
    board[GetTo(move)] = board[GetFrom(move)];
  }
  board[GetFrom(move)] = Piece::kNone;
  return isPromotion;
}

void EnPassantCapture(Board& board, Square from, Square to) {
  if (GetBasePiece(board[to]) == BasePiece::kPawn) {
    const auto captureSquare = CreateSquare(GetFile(to), GetRank(from));
    board[captureSquare] = Piece::kNone;
  }
}

Square EvaluateEnPassant(const Board& board, Square from, Square to) {
  const auto piece = board[to];
  if (piece == Piece::kBlackPawn || piece == Piece::kWhitePawn) {
    const auto rankShift = GetRank(to) - GetRank(from);
    if (std::abs(rankShift) == 2) {
      return ShiftSquare(from, rankShift / 2, 0);
    }
  }
  return Square::kInvalid;
}

static Bitboard AllyPieces(const State& state) {
  auto allyPieces = Bitboard{};
  if (state.turn == Color::kWhite) {
    allyPieces |= state.bitboards[kWhiteKing];
    allyPieces |= state.bitboards[kWhiteQueen];
    allyPieces |= state.bitboards[kWhiteRook];
    allyPieces |= state.bitboards[kWhiteBishop];
    allyPieces |= state.bitboards[kWhiteKnight];
    allyPieces |= state.bitboards[kWhitePawn];
  } else if (state.turn == Color::kBlack) {
    allyPieces |= state.bitboards[kBlackKing];
    allyPieces |= state.bitboards[kBlackQueen];
    allyPieces |= state.bitboards[kBlackRook];
    allyPieces |= state.bitboards[kBlackBishop];
    allyPieces |= state.bitboards[kBlackKnight];
    allyPieces |= state.bitboards[kBlackPawn];
  }
  return allyPieces;
}

void PushPawn(Moves& moves, Square fromSquare, Square toSquare, Color color) {
  const auto toSquareRank = GetRank(toSquare);
  if (toSquareRank == Rank::_1 || toSquareRank == Rank::_8) {
    moves.push_back(CreateMove(fromSquare, toSquare, MoveType::kBishopPromotion));
    moves.push_back(CreateMove(fromSquare, toSquare, MoveType::kRookPromotion));
    moves.push_back(CreateMove(fromSquare, toSquare, MoveType::kKnightPromotion));
    moves.push_back(CreateMove(fromSquare, toSquare, MoveType::kQueenPromotion));
  } else {
    moves.push_back(CreateMove(fromSquare, toSquare));
  }
}

bool PushPawnIfEmpty(Moves& moves, const Board& board, Square fromSquare, int rankShift,
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

void PushPawnIfOpposite(Moves& moves, const Board& board, Square fromSquare,
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

bool PushIfEmptyOrOpposite(Moves& moves, const Board& board, Square fromSquare,
                           Square toSquare) {
  const auto fromPiece = board[fromSquare];
  const auto toPiece = board[toSquare];

  if (toPiece == Piece::kNone) {
    moves.push_back(CreateMove(fromSquare, toSquare));
    return true;
  }

  if (GetPieceColor(fromPiece) != GetPieceColor(toPiece)) {
    moves.push_back(CreateMove(fromSquare, toSquare));
    return false;
  }

  return false;
}

bool PushIfEmptyOrOpposite(Moves& moves, const Board& board, Square square,
                           int rankShift, int fileShift) {
  const auto shifted = ShiftSquare(square, rankShift, fileShift);
  if (shifted == square) {
    return false;
  }
  return PushIfEmptyOrOpposite(moves, board, square, shifted);
}

void GetKingMovesWithoutCastling(const State& state, Square square, Moves& moves) {
  const auto allyPieces = AllyPieces(state);
  auto bitboard = Bitboard{1ULL << square};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    auto attacks = KingAttacks(1ULL << from) & ~allyPieces;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

bool IsAttacked(const State& state, Color turn, Bitboard target) {
  auto occupancy = ~state.bitboards[Piece::kNone];
  if (turn == Color::kWhite) {
    if ((BlackPawnAttacks(state.bitboards[kBlackPawn]) & target) != 0) {
      return true;
    }
    if ((KingAttacks(target) & state.bitboards[kBlackKing]) != 0) {
      return true;
    }
    if ((KnightAttacks(target) & state.bitboards[kBlackKnight]) != 0) {
      return true;
    }
    const auto bishopAttacks = BishopAttacks(target, occupancy);
    if ((bishopAttacks & state.bitboards[kBlackBishop]) != 0) {
      return true;
    }
    const auto rookAttacks = RookAttacks(target, occupancy);
    if ((rookAttacks & state.bitboards[kBlackRook]) != 0) {
      return true;
    }
    if (((bishopAttacks | rookAttacks) & state.bitboards[kBlackQueen]) != 0) {
      return true;
    }
    return false;
  } else if (turn == Color::kBlack) {
    if ((WhitePawnAttacks(state.bitboards[kWhitePawn]) & target) != 0) {
      return true;
    }
    if ((KingAttacks(target) & state.bitboards[kWhiteKing]) != 0) {
      return true;
    }
    if ((KnightAttacks(target) & state.bitboards[kWhiteKnight]) != 0) {
      return true;
    }
    const auto bishopAtacks = BishopAttacks(target, occupancy);
    if ((bishopAtacks & state.bitboards[kWhiteBishop]) != 0) {
      return true;
    }
    const auto rookAttacks = RookAttacks(target, occupancy);
    if ((rookAttacks & state.bitboards[kWhiteRook]) != 0) {
      return true;
    }
    if (((bishopAtacks | rookAttacks) & state.bitboards[kWhiteQueen]) != 0) {
      return true;
    }
    return false;
  }

  return false;
}

void GetKingMoves(const State& state, Square square, Moves& moves) {
  GetKingMovesWithoutCastling(state, square, moves);
  if (!IsInCheck(state, state.turn)) {
    if (GetPieceColor(state.board[square]) == Color::kWhite) {
      if (state.whiteShortCastleAllowed) {
        if (state.board[F1] == Piece::kNone && state.board[G1] == Piece::kNone &&
            !IsAttacked(state, state.turn, 1ULL << F1)) {
          moves.push_back(CreateMove(square, G1));
        }
      }
      if (state.whiteLongCastleAllowed) {
        if (state.board[B1] == Piece::kNone && state.board[C1] == Piece::kNone &&
            state.board[D1] == Piece::kNone && !IsAttacked(state, state.turn, 1ULL << C1) &&
            !IsAttacked(state, state.turn, 1ULL << D1)) {
          moves.push_back(CreateMove(square, C1));
        }
      }
    } else {
      if (state.blackShortCastleAllowed) {
        if (state.board[F8] == Piece::kNone && state.board[G8] == Piece::kNone &&
            !IsAttacked(state, state.turn, 1ULL << F8)) {
          moves.push_back(CreateMove(square, G8));
        }
      }
      if (state.blackLongCastleAllowed) {
        if (state.board[B8] == Piece::kNone && state.board[C8] == Piece::kNone &&
            state.board[D8] == Piece::kNone && !IsAttacked(state, state.turn, 1ULL << C8) &&
            !IsAttacked(state, state.turn, 1ULL << D8)) {
          moves.push_back(CreateMove(square, C8));
        }
      }
    }
  }
}

void GetRookMoves(const State& state, Square square, Moves& moves) {
  const auto allyPieces = AllyPieces(state);
  auto bitboard = Bitboard{1ULL << square};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    auto attacks = RookAttacks(1ULL << from, ~state.bitboards[kNone]) & ~allyPieces;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetBishopMoves(const State& state, Square square, Moves& moves) {
  const auto allyPieces = AllyPieces(state);
  auto bitboard = Bitboard{1ULL << square};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    auto attacks = BishopAttacks(1ULL << from, ~state.bitboards[kNone]) & ~allyPieces;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetQueenMoves(const State& state, Square square, Moves& moves) {
  GetBishopMoves(state, square, moves);
  GetRookMoves(state, square, moves);
}

void GetKnightMoves(const State& state, Square square, Moves& moves) {
  const auto allyPieces = AllyPieces(state);
  auto bitboard = Bitboard{1ULL << square};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    auto attacks = KnightAttacks(1ULL << from) & ~allyPieces;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetPawnMoves(const State& state, Square square, Moves& moves) {
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
  if (state.enPassant != Square::kInvalid) {
    const auto fileShift = GetFile(state.enPassant) - GetFile(square);
    const auto rankShift = GetRank(state.enPassant) - GetRank(square);
    if (std::abs(fileShift) == 1 && rankShift == direction) {
      PushIfEmptyOrOpposite(moves, state.board, square, state.enPassant);
    }
  }
}

void GetMoves(const State& state, Square square, Moves& moves) {
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
  auto occupancy = ~state.bitboards[Piece::kNone];
  if (turn == Color::kWhite) {
    return IsAttacked(state, turn, state.bitboards[kWhiteKing]);
  } else if (turn == Color::kBlack) {
    return IsAttacked(state, turn, state.bitboards[kBlackKing]);
  }

  return false;
}

bool CanMoveInTurn(const State& state, Square square) {
  return GetPieceColor(state.board[square]) == state.turn;
}

void UpdateCastlingState(State& state, const Move& move) {
  const auto fromPiece = state.board[GetFrom(move)];
  const auto toPiece = state.board[GetTo(move)];
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
    if (GetFrom(move) == A1) {
      state.whiteLongCastleAllowed = false;
    } else if (GetFrom(move) == H1) {
      state.whiteShortCastleAllowed = false;
    }
    if (GetFrom(move) == A8) {
      state.blackLongCastleAllowed = false;
    }
    if (GetFrom(move) == H8) {
      state.blackShortCastleAllowed = false;
    }
  } else if (toBasePiece == BasePiece::kRook) {
    if (GetTo(move) == A1) {
      state.whiteLongCastleAllowed = false;
    } else if (GetTo(move) == H1) {
      state.whiteShortCastleAllowed = false;
    }
    if (GetTo(move) == A8) {
      state.blackLongCastleAllowed = false;
    }
    if (GetTo(move) == H8) {
      state.blackShortCastleAllowed = false;
    }
  }
}

static bool ProcessCastle(State& state, const Move& move) {
  const auto piece = state.board[GetTo(move)];
  const auto basePiece = GetBasePiece(piece);
  if (basePiece != BasePiece::kKing) {
    return false;
  }
  const auto fileShift = GetFile(GetFrom(move)) - GetFile(GetTo(move));
  if (std::abs(fileShift) != 2) {
    return false;
  }
  if (GetTo(move) == G1) {
    std::swap(state.board[H1], state.board[F1]);
  } else if (GetTo(move) == C1) {
    std::swap(state.board[A1], state.board[D1]);
  } else if (GetTo(move) == G8) {
    std::swap(state.board[H8], state.board[F8]);
  } else if (GetTo(move) == C8) {
    std::swap(state.board[A8], state.board[D8]);
  }
  return true;
}

void MakeMove(State& state, const Move& move) {
  const auto from = GetFrom(move);
  const auto to = GetTo(move);
  const auto fromPiece = state.board[from];
  const auto toPiece = state.board[to];
  UpdateCastlingState(state, move);
  auto needsFullBitboardRebuild = false;
  MoveOrCapture(state.board, move, state.turn);
  if (state.enPassant != Square::kInvalid && to == state.enPassant) {
    EnPassantCapture(state.board, from, to);
    needsFullBitboardRebuild |= true;
  }
  needsFullBitboardRebuild |= ProcessCastle(state, move);
  state.turn = SwitchColor(state.turn);
  state.enPassant = EvaluateEnPassant(state.board, from, to);
  if (needsFullBitboardRebuild) {
    FillBitboardsFromBoard(state);
  } else {
    const auto toPiece2 = state.board[to];
    auto& e = state.bitboards[Piece::kNone];
    auto& f = state.bitboards[fromPiece];
    auto& t = state.bitboards[toPiece];
    auto& t2 = state.bitboards[toPiece2];
    f = InvertBit(f, from);
    e = InvertBit(e, from);
    t = InvertBit(t, to);
    t2 = InvertBit(t2, to);
  }
}

void GetLegalMoves(const State& state, Square square, Moves& legalMoves) {
  auto& board = state.board;
  auto moves = Moves{};
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

void GetAllLegalMoves(const State& state, Moves& legalMoves) {
  for (auto squareIndex = 0; squareIndex < kBoardSize * kBoardSize; ++squareIndex) {
    const auto square = static_cast<Square>(squareIndex);
    if (GetPieceColor(state.board[square]) == state.turn) {
      GetLegalMoves(state, square, legalMoves);
    }
  }
}

bool CanMove(const State& state, Square square) {
  auto legalMoves = Moves{};
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
  if (GetFrom(move) == GetTo(move) || !CanMoveInTurn(state, GetFrom(move))) {
    return false;
  }
  auto& board = state.board;
  auto legalMoves = Moves{};
  GetLegalMoves(state, GetFrom(move), legalMoves);
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

Moves Game::GetLegalMoves(Square square) const {
  auto legalMoves = Moves{};
  chess::GetLegalMoves(state_, square, legalMoves);
  return legalMoves;
}

} // namespace chess