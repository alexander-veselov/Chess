#include "chess/core/game.h"

#include "chess/core/attacks.h"
#include "chess/core/fen.h"

namespace chess {

namespace {

State CreateDefaultState() {
  return chess::StateFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

BasePiece PromotionTypeToBasePiece(MoveType type) {
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

bool MoveOrCapture(Board& board, const Move& move, Color color) {
  const auto from = GetFrom(move);
  const auto to = GetTo(move);
  const auto moveType = GetType(move);
  const auto isPromotion = IsPromotion(moveType);
  if (isPromotion) {
    board[to] = MakePiece(color, PromotionTypeToBasePiece(moveType));
  } else {
    board[to] = board[from];
  }
  board[from] = Piece::kNone;
  return isPromotion;
}

Square EvaluateEnPassant(const Board& board, Square from, Square to) {
  const auto piece = board[to];
  if (piece == Piece::kBlackPawn || piece == Piece::kWhitePawn) {
    const auto rankShift = GetRank(to) - GetRank(from);
    if (std::abs(rankShift) == 2) {
      return CreateSquare(GetFile(from), (Rank)(GetRank(from) + rankShift / 2));
    }
  }
  return Square::kInvalid;
}

Bitboard AllyPieces(const State& state) {
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

void GetKingMovesWithoutCastling(const State& state, Square square, Moves& moves) {
  const auto allyPieces = AllyPieces(state);
  auto attacks = KingAttacks(BBFromSquare(square)) & ~allyPieces;
  while (attacks) {
    const auto to = PopLSB(attacks);
    moves.push_back(CreateMove(square, to));
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

bool IsInCheck(const State& state, Color turn) {
  if (turn == Color::kWhite) {
    return IsAttacked(state, turn, state.bitboards[kWhiteKing]);
  } else if (turn == Color::kBlack) {
    return IsAttacked(state, turn, state.bitboards[kBlackKing]);
  }
  return false;
}

void GetKingMoves(const State& state, Bitboard allyPieces, Square square, Moves& moves) {
  GetKingMovesWithoutCastling(state, square, moves);
  if (!IsInCheck(state, state.turn)) {
    if (GetPieceColor(state.board[square]) == Color::kWhite) {
      if (state.whiteShortCastleAllowed) {
        if (state.board[F1] == Piece::kNone && state.board[G1] == Piece::kNone &&
            !IsAttacked(state, state.turn, BBFromSquare(F1))) {
          moves.push_back(CreateMove(square, G1, MoveType::kKingCastle));
        }
      }
      if (state.whiteLongCastleAllowed) {
        if (state.board[B1] == Piece::kNone && state.board[C1] == Piece::kNone &&
            state.board[D1] == Piece::kNone && !IsAttacked(state, state.turn, BBFromSquare(C1)) &&
            !IsAttacked(state, state.turn, BBFromSquare(D1))) {
          moves.push_back(CreateMove(square, C1, MoveType::kQueenCastle));
        }
      }
    } else {
      if (state.blackShortCastleAllowed) {
        if (state.board[F8] == Piece::kNone && state.board[G8] == Piece::kNone &&
            !IsAttacked(state, state.turn, BBFromSquare(F8))) {
          moves.push_back(CreateMove(square, G8, MoveType::kKingCastle));
        }
      }
      if (state.blackLongCastleAllowed) {
        if (state.board[B8] == Piece::kNone && state.board[C8] == Piece::kNone &&
            state.board[D8] == Piece::kNone && !IsAttacked(state, state.turn, BBFromSquare(C8)) &&
            !IsAttacked(state, state.turn, BBFromSquare(D8))) {
          moves.push_back(CreateMove(square, C8, MoveType::kQueenCastle));
        }
      }
    }
  }
}

void GetRookMoves(const State& state, Bitboard allyPieces, Square square, Moves& moves) {
  auto attacks = RookAttacks(BBFromSquare(square), ~state.bitboards[kNone]) & ~allyPieces;
  while (attacks) {
    const auto to = PopLSB(attacks);
    moves.push_back(CreateMove(square, to));
  }
}

void GetBishopMoves(const State& state, Bitboard allyPieces, Square square, Moves& moves) {
  auto attacks = BishopAttacks(BBFromSquare(square), ~state.bitboards[kNone]) & ~allyPieces;
  while (attacks) {
    const auto to = PopLSB(attacks);
    moves.push_back(CreateMove(square, to));
  }
}

void GetQueenMoves(const State& state, Bitboard allyPieces, Square square, Moves& moves) {
  auto attacks = QueenAttacks(BBFromSquare(square), ~state.bitboards[kNone]) & ~allyPieces;
  while (attacks) {
    const auto to = PopLSB(attacks);
    moves.push_back(CreateMove(square, to));
  }
}

void GetKnightMoves(const State& state, Bitboard allyPieces, Square square, Moves& moves) {
  auto attacks = KnightAttacks(BBFromSquare(square)) & ~allyPieces;
  while (attacks) {
    const auto to = PopLSB(attacks);
    moves.push_back(CreateMove(square, to));
  }
}

void GetPawnMoves(const State& state, Bitboard allyPieces, Square square, Moves& moves) {
  const auto bitboard = BBFromSquare(square);
  auto attacks = kEmptyBoard;
  if (state.turn == Color::kWhite) {
    attacks |= WhitePawnSinglePushes(bitboard, ~state.bitboards[kNone]);
    attacks |= WhitePawnDoublePushes(bitboard, ~state.bitboards[kNone]);
    attacks |= WhitePawnAttacks(bitboard) & ~(allyPieces | state.bitboards[kNone]);
  } else {
    attacks |= BlackPawnSinglePushes(bitboard, ~state.bitboards[kNone]);
    attacks |= BlackPawnDoublePushes(bitboard, ~state.bitboards[kNone]);
    attacks |= BlackPawnAttacks(bitboard) & ~(allyPieces | state.bitboards[kNone]);
  }
  while (attacks) {
    const auto toSquare = PopLSB(attacks);
    const auto toSquareRank = GetRank(toSquare);
    if (toSquareRank == Rank::_1 || toSquareRank == Rank::_8) {
      moves.push_back(CreateMove(square, toSquare, MoveType::kBishopPromotion));
      moves.push_back(CreateMove(square, toSquare, MoveType::kRookPromotion));
      moves.push_back(CreateMove(square, toSquare, MoveType::kKnightPromotion));
      moves.push_back(CreateMove(square, toSquare, MoveType::kQueenPromotion));
    } else {
      moves.push_back(CreateMove(square, toSquare));
    }
  }
  if (state.enPassant != Square::kInvalid) {
    const auto fileShift = GetFile(state.enPassant) - GetFile(square);
    const auto rankShift = GetRank(state.enPassant) - GetRank(square);
    const auto direction = state.turn == Color::kWhite ? 1 : -1;
    if (std::abs(fileShift) == 1 && rankShift == direction) {
      moves.push_back(CreateMove(square, state.enPassant, MoveType::kEnPassant));
    }
  }
}

void GetMoves(const State& state, Bitboard allyPieces, Square square, Moves& moves) {
  const auto piece = state.board[square];
  switch (piece) {
  case Piece::kWhiteKing:
  case Piece::kBlackKing:
    GetKingMoves(state, allyPieces, square, moves);
    break;
  case Piece::kWhiteQueen:
  case Piece::kBlackQueen:
    GetQueenMoves(state, allyPieces, square, moves);
    break;
  case Piece::kWhiteRook:
  case Piece::kBlackRook:
    GetRookMoves(state, allyPieces, square, moves);
    break;
  case Piece::kWhiteBishop:
  case Piece::kBlackBishop:
    GetBishopMoves(state, allyPieces, square, moves);
    break;
  case Piece::kWhiteKnight:
  case Piece::kBlackKnight:
    GetKnightMoves(state, allyPieces, square, moves);
    break;
  case Piece::kWhitePawn:
  case Piece::kBlackPawn:
    GetPawnMoves(state, allyPieces, square, moves);
    break;
  }
}

bool CanMoveInTurn(const State& state, Square square) {
  return GetPieceColor(state.board[square]) == state.turn;
}

void UpdateCastlingState(State& state, const Move& move) {
  const auto from = GetFrom(move);
  const auto to = GetTo(move);
  const auto fromPiece = state.board[from];
  const auto toPiece = state.board[to];
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
    if (from == A1) {
      state.whiteLongCastleAllowed = false;
    } else if (from == H1) {
      state.whiteShortCastleAllowed = false;
    }
    if (from == A8) {
      state.blackLongCastleAllowed = false;
    }
    if (from == H8) {
      state.blackShortCastleAllowed = false;
    }
  } else if (toBasePiece == BasePiece::kRook) {
    if (to == A1) {
      state.whiteLongCastleAllowed = false;
    } else if (to == H1) {
      state.whiteShortCastleAllowed = false;
    }
    if (to == A8) {
      state.blackLongCastleAllowed = false;
    }
    if (to == H8) {
      state.blackShortCastleAllowed = false;
    }
  }
}

void GetLegalMoves(const State& state, Square square, Moves& legalMoves) {
  auto& board = state.board;
  auto moves = Moves{};
  if (!CanMoveInTurn(state, square)) {
    return;
  }
  const auto allyPieces = AllyPieces(state);
  GetMoves(state, allyPieces, square, moves);
  for (const auto move : moves) {
    auto newState = State{state};
    MakeMove(newState, move);
    if (!IsInCheck(newState, state.turn)) {
      legalMoves.push_back(move);
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

} // namespace

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

void GetAllLegalMoves(const State& state, Moves& legalMoves) {
  auto bitboard = AllyPieces(state);
  while (bitboard) {
    const auto square = PopLSB(bitboard);
    GetLegalMoves(state, square, legalMoves);
  }
}

void MakeMove(State& state, const Move& move) {
  const auto from = GetFrom(move);
  const auto to = GetTo(move);
  const auto fromPiece = state.board[from];
  const auto toPiece = state.board[to];
  UpdateCastlingState(state, move);
  MoveOrCapture(state.board, move, state.turn);
  const auto moveType = GetType(move);
  if (moveType == MoveType::kEnPassant) {
    const auto captureSquare = CreateSquare(GetFile(to), GetRank(from));
    state.board[captureSquare] = Piece::kNone;
    if (state.turn == Color::kWhite) {
      state.bitboards[kBlackPawn] ^= BBFromSquare(captureSquare);
    } else if (state.turn == Color::kBlack) {
      state.bitboards[kWhitePawn] ^= BBFromSquare(captureSquare);
    }
    state.bitboards[kNone] ^= BBFromSquare(captureSquare);
  } else if (moveType == MoveType::kKingCastle || moveType == MoveType::kQueenCastle) {
    const auto to = GetTo(move);
    if (to == G1) {
      std::swap(state.board[H1], state.board[F1]);
      state.bitboards[kWhiteRook] ^= BBFromSquare(H1);
      state.bitboards[kWhiteRook] ^= BBFromSquare(F1);
      state.bitboards[kNone] ^= BBFromSquare(H1);
      state.bitboards[kNone] ^= BBFromSquare(F1);
    } else if (to == C1) {
      std::swap(state.board[A1], state.board[D1]);
      state.bitboards[kWhiteRook] ^= BBFromSquare(A1);
      state.bitboards[kWhiteRook] ^= BBFromSquare(D1);
      state.bitboards[kNone] ^= BBFromSquare(A1);
      state.bitboards[kNone] ^= BBFromSquare(D1);
    } else if (to == G8) {
      std::swap(state.board[H8], state.board[F8]);
      state.bitboards[kBlackRook] ^= BBFromSquare(H8);
      state.bitboards[kBlackRook] ^= BBFromSquare(F8);
      state.bitboards[kNone] ^= BBFromSquare(H8);
      state.bitboards[kNone] ^= BBFromSquare(F8);
    } else if (to == C8) {
      std::swap(state.board[A8], state.board[D8]);
      state.bitboards[kBlackRook] ^= BBFromSquare(A8);
      state.bitboards[kBlackRook] ^= BBFromSquare(D8);
      state.bitboards[kNone] ^= BBFromSquare(A8);
      state.bitboards[kNone] ^= BBFromSquare(D8);
    }
  }
  state.turn = FlipColor(state.turn);
  state.enPassant = EvaluateEnPassant(state.board, from, to);

  const auto toPiece2 = state.board[to];
  auto& emptySquares = state.bitboards[Piece::kNone];
  auto& movingPieces = state.bitboards[fromPiece];
  auto& capturedPieces = state.bitboards[toPiece];
  auto& destinationPiece = state.bitboards[toPiece2];

  movingPieces = FlipBit(movingPieces, from);
  emptySquares = FlipBit(emptySquares, from);
  capturedPieces = FlipBit(capturedPieces, to);
  destinationPiece = FlipBit(destinationPiece, to);
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