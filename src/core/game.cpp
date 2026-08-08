#include "chess/core/game.h"

#include "chess/core/attacks.h"
#include "chess/core/castling_rights.h"
#include "chess/core/color.h"
#include "chess/core/fen.h"

namespace chess {

namespace {

State CreateDefaultState() {
  return chess::StateFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Square EvaluateEnPassant(Square from, Square to, Piece fromPiece) {
  const auto diff = std::abs(to - from);
  if (diff != 16) {
    return Square::kInvalid;
  }
  switch (fromPiece) {
  case Piece::kWhitePawn:
    return static_cast<Square>(from + 8);
  case Piece::kBlackPawn:
    return static_cast<Square>(from - 8);
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

bool IsAttacked(const State& state, Color turn, Square square) {
  const auto target = BBFromSquare(square);
  const auto occupancy = ~state.bitboards[Piece::kNone];
  if (turn == Color::kWhite) {
    if (BlackPawnAttacks(state.bitboards[kBlackPawn]) & target) {
      return true;
    }
    if (KingAttacks(square) & state.bitboards[kBlackKing]) {
      return true;
    }
    if (KnightAttacks(square) & state.bitboards[kBlackKnight]) {
      return true;
    }
    if (BishopAttacks(square, occupancy) &
        (state.bitboards[kBlackBishop] | state.bitboards[kBlackQueen])) {
      return true;
    }
    if (RookAttacks(square, occupancy) &
        (state.bitboards[kBlackRook] | state.bitboards[kBlackQueen])) {
      return true;
    }
    return false;
  } else if (turn == Color::kBlack) {
    if (WhitePawnAttacks(state.bitboards[kWhitePawn]) & target) {
      return true;
    }
    if (KingAttacks(square) & state.bitboards[kWhiteKing]) {
      return true;
    }
    if (KnightAttacks(square) & state.bitboards[kWhiteKnight]) {
      return true;
    }
    if (BishopAttacks(square, occupancy) &
        (state.bitboards[kWhiteBishop] | state.bitboards[kWhiteQueen])) {
      return true;
    }
    if (RookAttacks(square, occupancy) &
        (state.bitboards[kWhiteRook] | state.bitboards[kWhiteQueen])) {
      return true;
    }
    return false;
  }

  return false;
}

bool IsInCheck(const State& state, Color turn) {
  if (turn == Color::kWhite) {
    return IsAttacked(state, turn, LSB(state.bitboards[kWhiteKing]));
  } else if (turn == Color::kBlack) {
    return IsAttacked(state, turn, LSB(state.bitboards[kBlackKing]));
  }
  return false;
}

void GetKingMoves(const State& state, Bitboard allyPieces, Bitboard king, Moves& moves) {
  const auto from = PopLSB(king);
  auto attacks = KingAttacks(from) & ~allyPieces;
  while (attacks) {
    const auto to = PopLSB(attacks);
    moves.push_back(CreateMove(from, to));
  }
  if (!IsInCheck(state, state.turn)) {
    const auto empty = state.bitboards[kNone];
    if (state.turn == Color::kWhite) {
      if (CanCastle(state.castlingRightsMask, CastlingRight::kWhiteKingSide) &&
          HasAllBits(empty, kF1G1) && !IsAttacked(state, state.turn, F1)) {
        moves.push_back(CreateMove(from, G1, MoveType::kKingCastle));
      }
      if (CanCastle(state.castlingRightsMask, CastlingRight::kWhiteQueenSide) &&
          HasAllBits(empty, kB1C1D1) && !IsAttacked(state, state.turn, C1) &&
          !IsAttacked(state, state.turn, D1)) {
        moves.push_back(CreateMove(from, C1, MoveType::kQueenCastle));
      }
    } else {
      if (CanCastle(state.castlingRightsMask, CastlingRight::kBlackKingSide) &&
          HasAllBits(empty, kF8G8) && !IsAttacked(state, state.turn, F8)) {
        moves.push_back(CreateMove(from, G8, MoveType::kKingCastle));
      }
      if (CanCastle(state.castlingRightsMask, CastlingRight::kBlackQueenSide) &&
          HasAllBits(empty, kB8C8D8) && !IsAttacked(state, state.turn, C8) &&
          !IsAttacked(state, state.turn, D8)) {
        moves.push_back(CreateMove(from, C8, MoveType::kQueenCastle));
      }
    }
  }
}

void GetRookMoves(const State& state, Bitboard allyPieces, Bitboard rooks, Moves& moves) {
  while (rooks) {
    const auto from = PopLSB(rooks);
    auto attacks = RookAttacks(from, ~state.bitboards[kNone]) & ~allyPieces;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetBishopMoves(const State& state, Bitboard allyPieces, Bitboard bishops, Moves& moves) {
  while (bishops) {
    const auto from = PopLSB(bishops);
    auto attacks = BishopAttacks(from, ~state.bitboards[kNone]) & ~allyPieces;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetQueenMoves(const State& state, Bitboard allyPieces, Bitboard queens, Moves& moves) {
  while (queens) {
    const auto from = PopLSB(queens);
    auto attacks = QueenAttacks(from, ~state.bitboards[kNone]) & ~allyPieces;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetKnightMoves(const State& state, Bitboard allyPieces, Bitboard knights, Moves& moves) {
  while (knights) {
    const auto from = PopLSB(knights);
    auto attacks = KnightAttacks(from) & ~allyPieces;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetPawnMoves(const State& state, Bitboard allyPieces, Bitboard pawns, Moves& moves) {
  const auto enPassantBB =
      state.enPassant == Square::kInvalid ? kEmptyBoard : BBFromSquare(state.enPassant);
  const auto occupancy = ~state.bitboards[kNone];
  const auto captures = ~allyPieces & occupancy | enPassantBB;
  while (pawns) {
    const auto from = PopLSB(pawns);
    const auto pawn = BBFromSquare(from);
    auto attacks = kEmptyBoard;
    if (state.turn == Color::kWhite) {
      attacks |= WhitePawnSinglePushes(pawn, occupancy);
      attacks |= WhitePawnDoublePushes(pawn, occupancy);
      attacks |= WhitePawnAttacks(pawn) & captures;
    } else {
      attacks |= BlackPawnSinglePushes(pawn, occupancy);
      attacks |= BlackPawnDoublePushes(pawn, occupancy);
      attacks |= BlackPawnAttacks(pawn) & captures;
    }
    while (attacks) {
      const auto to = PopLSB(attacks);
      if (to == state.enPassant) {
        moves.push_back(CreateMove(from, state.enPassant, MoveType::kEnPassant));
      } else if (BBFromSquare(to) & k18Rank) {
        moves.push_back(CreateMove(from, to, MoveType::kBishopPromotion));
        moves.push_back(CreateMove(from, to, MoveType::kRookPromotion));
        moves.push_back(CreateMove(from, to, MoveType::kKnightPromotion));
        moves.push_back(CreateMove(from, to, MoveType::kQueenPromotion));
      } else {
        moves.push_back(CreateMove(from, to));
      }
    }
  }
}

bool CanMoveInTurn(const State& state, Square square) {
  return GetPieceColor(state.board[square]) == state.turn;
}

void UpdateCastlingState(State& state,
                         Square from, Square to,
                         BasePiece fromBasePiece, BasePiece toBasePiece) {
  if (fromBasePiece == BasePiece::kKing) {
    if (state.turn == Color::kWhite) {
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kWhiteKingSide);
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kWhiteQueenSide);
    } else {
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kBlackKingSide);
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kBlackQueenSide);
    }
  }
  if (fromBasePiece == BasePiece::kRook) {
    if (from == A1) {
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kWhiteQueenSide);
    } else if (from == H1) {
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kWhiteKingSide);
    } else if (from == A8) {
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kBlackQueenSide);
    } else if (from == H8) {
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kBlackKingSide);
    }
  }
  if (toBasePiece == BasePiece::kRook) {
    if (to == A1) {
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kWhiteQueenSide);
    } else if (to == H1) {
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kWhiteKingSide);
    } else if (to == A8) {
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kBlackQueenSide);
    } else if (to == H8) {
      RemoveCastlingRight(state.castlingRightsMask, CastlingRight::kBlackKingSide);
    }
  }
}

bool HasAvailableMoves(const State& state) {
  auto legalMoves = Moves{};
  GetLegalMoves(state, legalMoves);
  return !legalMoves.empty();
}

bool LegalMove(State& state, Move move) {
  if (GetFrom(move) == GetTo(move) || !CanMoveInTurn(state, GetFrom(move))) {
    return false;
  }
  auto legalMoves = Moves{};
  GetLegalMoves(state, legalMoves);
  for (const auto legalMove : legalMoves) {
    if (legalMove == move) {
      MakeMove(state, legalMove);
      return true;
    }
  }
  return false;
}

bool IsPotentiallyPinned(Bitboard bishopAttacks, Bitboard rookAttacks, bool hasDiagonalPin,
                         bool hasOrthogonalPin, Square square) {
  const auto bitboard = BBFromSquare(square);
  return hasDiagonalPin && (bishopAttacks & bitboard) ||
         hasOrthogonalPin && (rookAttacks & bitboard);
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

void GetLegalMoves(const State& state, Moves& legalMoves) {
  auto moves = Moves{};
  const auto allyPieces = AllyPieces(state);
  GetQueenMoves (state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kQueen )], moves);
  GetPawnMoves  (state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kPawn  )], moves);
  GetRookMoves  (state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kRook  )], moves);
  GetBishopMoves(state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kBishop)], moves);
  GetKnightMoves(state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kKnight)], moves);
  GetKingMoves  (state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kKing  )], moves);

  const auto occupancy = ~state.bitboards[kNone];
  const auto kingSquare = LSB(state.bitboards[MakePiece(state.turn, BasePiece::kKing)]);
  const auto bishopAttacks = XRayBishopAttacks(kingSquare, occupancy);
  const auto rookAttacks = XRayRookAttacks(kingSquare, occupancy);
  const auto enemyColor = FlipColor(state.turn);
  const auto bishopAttackers = state.bitboards[MakePiece(enemyColor, BasePiece::kBishop)] |
                               state.bitboards[MakePiece(enemyColor, BasePiece::kQueen)];
  const auto rookAttackers = state.bitboards[MakePiece(enemyColor, BasePiece::kRook)] |
                             state.bitboards[MakePiece(enemyColor, BasePiece::kQueen)];
  const auto hasDiagonalPin = bishopAttacks & bishopAttackers;
  const auto hasOrthogonalPin = rookAttacks & rookAttackers;

  const auto isInCheck = IsInCheck(state, state.turn);
  for (const auto move : moves) {
    const auto from = GetFrom(move);
    if (isInCheck || GetBasePiece(state.board[from]) == BasePiece::kKing ||
        GetType(move) == MoveType::kEnPassant ||
        IsPotentiallyPinned(bishopAttacks, rookAttacks, hasDiagonalPin, hasOrthogonalPin, from)) {
      auto newState = State{state};
      MakeMove(newState, move);
      if (!IsInCheck(newState, state.turn)) {
        legalMoves.push_back(move);
      }
    } else {
      legalMoves.push_back(move);
    }
  }
}

void MakeMove(State& state, Move move) {
  const auto from = GetFrom(move);
  const auto to = GetTo(move);
  const auto fromPiece = state.board[from];
  const auto toPiece = state.board[to];
  
  state.board[to] = state.board[from];
  state.board[from] = Piece::kNone;

  switch (GetType(move)) {
  case MoveType::kEnPassant:
    if (state.turn == Color::kWhite) {
      const auto captureSquare = static_cast<Square>(state.enPassant - 8);
      state.board[captureSquare] = Piece::kNone;
      state.bitboards[kBlackPawn] ^= BBFromSquare(captureSquare);
      state.bitboards[kNone] ^= BBFromSquare(captureSquare);
    } else if (state.turn == Color::kBlack) {
      const auto captureSquare = static_cast<Square>(state.enPassant + 8);
      state.board[captureSquare] = Piece::kNone;
      state.bitboards[kWhitePawn] ^= BBFromSquare(captureSquare);
      state.bitboards[kNone] ^= BBFromSquare(captureSquare);
    }
    break;
  case MoveType::kKingCastle:
    switch (to) {
    case G1:
      std::swap(state.board[H1], state.board[F1]);
      state.bitboards[kWhiteRook] ^= BBFromSquare(H1) | BBFromSquare(F1);
      state.bitboards[kNone] ^= BBFromSquare(H1) | BBFromSquare(F1);
      break;
    case G8:
      std::swap(state.board[H8], state.board[F8]);
      state.bitboards[kBlackRook] ^= BBFromSquare(H8) | BBFromSquare(F8);
      state.bitboards[kNone] ^= BBFromSquare(H8) | BBFromSquare(F8);
      break;
    }
    break;
  case MoveType::kQueenCastle:
    switch (to) {
    case C1:
      std::swap(state.board[A1], state.board[D1]);
      state.bitboards[kWhiteRook] ^= BBFromSquare(A1) | BBFromSquare(D1);
      state.bitboards[kNone] ^= BBFromSquare(A1) | BBFromSquare(D1);
      break;
    case C8:
      std::swap(state.board[A8], state.board[D8]);
      state.bitboards[kBlackRook] ^= BBFromSquare(A8) | BBFromSquare(D8);
      state.bitboards[kNone] ^= BBFromSquare(A8) | BBFromSquare(D8);
      break;
    }
    break;
  case MoveType::kQueenPromotion:
    state.board[to] = MakePiece(state.turn, BasePiece::kQueen);
    break;
  case MoveType::kRookPromotion:
    state.board[to] = MakePiece(state.turn, BasePiece::kRook);
    break;
  case MoveType::kBishopPromotion:
    state.board[to] = MakePiece(state.turn, BasePiece::kBishop);
    break;
  case MoveType::kKnightPromotion:
    state.board[to] = MakePiece(state.turn, BasePiece::kKnight);
    break;
  }

  state.bitboards[Piece::kNone] ^= BBFromSquare(from);
  state.bitboards[fromPiece] ^= BBFromSquare(from);
  state.bitboards[toPiece] ^= BBFromSquare(to);
  state.bitboards[state.board[to]] ^= BBFromSquare(to);

  state.enPassant = EvaluateEnPassant(from, to, fromPiece);
  UpdateCastlingState(state, from, to, GetBasePiece(fromPiece), GetBasePiece(toPiece));

  const auto resetClock = GetBasePiece(fromPiece) == BasePiece::kPawn || toPiece != Piece::kNone;
  state.halfmoveClock = resetClock ? 0 : state.halfmoveClock + 1;
  state.fullmoveNumber += state.turn == Color::kBlack ? 1 : 0;
  state.turn = FlipColor(state.turn);
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

bool Game::IsInCheck() const {
  return chess::IsInCheck(state_, state_.turn);
}

bool Game::MakeMove(Move move) {
  return chess::LegalMove(state_, move);
}

Moves Game::GetLegalMoves(Square square) const {
  // TODO: improve that function
  auto movesForSquare = Moves{};
  auto legalMoves = Moves{};
  chess::GetLegalMoves(state_, legalMoves);
  for (const auto move : legalMoves) {
    if (GetFrom(move) == square) {
      movesForSquare.push_back(move);
    }
  }
  return movesForSquare;
}

} // namespace chess