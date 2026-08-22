#include "chess/core/game.h"

#include "chess/core/attacks.h"
#include "chess/core/castling_rights.h"
#include "chess/core/color.h"
#include "chess/core/zobrist_hash.h"

#include <unordered_map>

namespace chess {

namespace {

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

Bitboard GetPiecesOfColor(const State& state, Color color) {
  auto pieces = Bitboard{};
  if (color == Color::kWhite) {
    pieces |= state.bitboards[kWhiteKing];
    pieces |= state.bitboards[kWhiteQueen];
    pieces |= state.bitboards[kWhiteRook];
    pieces |= state.bitboards[kWhiteBishop];
    pieces |= state.bitboards[kWhiteKnight];
    pieces |= state.bitboards[kWhitePawn];
  } else if (color == Color::kBlack) {
    pieces |= state.bitboards[kBlackKing];
    pieces |= state.bitboards[kBlackQueen];
    pieces |= state.bitboards[kBlackRook];
    pieces |= state.bitboards[kBlackBishop];
    pieces |= state.bitboards[kBlackKnight];
    pieces |= state.bitboards[kBlackPawn];
  }
  return pieces;
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

void GetKingMoves(const State& state, Bitboard mask, Bitboard king, Moves& moves) {
  const auto from = PopLSB(king);
  auto attacks = KingAttacks(from) & mask;
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

void GetRookMoves(const State& state, Bitboard mask, Bitboard rooks, Moves& moves) {
  while (rooks) {
    const auto from = PopLSB(rooks);
    auto attacks = RookAttacks(from, ~state.bitboards[kNone]) & mask;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetBishopMoves(const State& state, Bitboard mask, Bitboard bishops, Moves& moves) {
  while (bishops) {
    const auto from = PopLSB(bishops);
    auto attacks = BishopAttacks(from, ~state.bitboards[kNone]) & mask;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetQueenMoves(const State& state, Bitboard mask, Bitboard queens, Moves& moves) {
  while (queens) {
    const auto from = PopLSB(queens);
    auto attacks = QueenAttacks(from, ~state.bitboards[kNone]) & mask;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetKnightMoves(const State& state, Bitboard mask, Bitboard knights, Moves& moves) {
  while (knights) {
    const auto from = PopLSB(knights);
    auto attacks = KnightAttacks(from) & mask;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
  }
}

void GetPawnMoves(const State& state, Bitboard mask, Bitboard pawns, Moves& moves) {
  const auto enPassantBB =
      state.enPassant == Square::kInvalid ? kEmptyBoard : BBFromSquare(state.enPassant);
  const auto occupancy = ~state.bitboards[kNone];
  const auto captures = mask & occupancy | enPassantBB;
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

bool IsPotentiallyPinned(Bitboard bishopAttacks, Bitboard rookAttacks, bool hasDiagonalPin,
                         bool hasOrthogonalPin, Square square) {
  const auto bitboard = BBFromSquare(square);
  return hasDiagonalPin && (bishopAttacks & bitboard) ||
         hasOrthogonalPin && (rookAttacks & bitboard);
}

void GetMoves(State& state, Bitboard mask, Moves& legalMoves) {
  auto moves = Moves{};
  GetQueenMoves (state, mask, state.bitboards[MakePiece(state.turn, BasePiece::kQueen )], moves);
  GetPawnMoves  (state, mask, state.bitboards[MakePiece(state.turn, BasePiece::kPawn  )], moves);
  GetRookMoves  (state, mask, state.bitboards[MakePiece(state.turn, BasePiece::kRook  )], moves);
  GetBishopMoves(state, mask, state.bitboards[MakePiece(state.turn, BasePiece::kBishop)], moves);
  GetKnightMoves(state, mask, state.bitboards[MakePiece(state.turn, BasePiece::kKnight)], moves);
  GetKingMoves  (state, mask, state.bitboards[MakePiece(state.turn, BasePiece::kKing  )], moves);

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
      auto undo = Undo{};
      auto currentTurn = state.turn;
      MakeMove(state, move, undo);
      if (!IsInCheck(state, currentTurn)) {
        legalMoves.push_back(move);
      }
      UndoMove(state, move, undo);
    } else {
      legalMoves.push_back(move);
    }
  }
}

} // namespace

Status GetStatus(const State& state) {
  const auto hasAvailableMoves = HasAvailableMoves(state);
  if (!hasAvailableMoves) {
    if (IsInCheck(state, state.turn)) {
      return state.turn == Color::kWhite ? Status::kBlackWon : Status::kWhiteWon;
    } else {
      return Status::kDraw;
    }
  }

  if (Is50MoveRuleDraw(state)) {
    return Status::kDraw;
  }

  if (IsThreefoldRepetition(state)) {
    return Status::kDraw;
  }

  // TODO: implement draw by repetition/insufficient material/etc

  return state.turn == Color::kWhite ? Status::kWhiteToMove : Status::kBlackToMove;
}

bool Is50MoveRuleDraw(const State& state) {
  return state.halfmoveClock >= 100;
}

bool IsThreefoldRepetition(const State& state) {
  auto occurrences = 0;
  for (auto it = state.history.rbegin() + 1; it != state.history.rend(); ++it) {
    if (*it == state.hash) {
      if (++occurrences >= 2) {
        return true;
      }
    }
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

void GetLegalMoves(const State& state, Moves& legalMoves) {
  const auto mask = ~GetPiecesOfColor(state, state.turn);
  return GetMoves(const_cast<State&>(state), mask, legalMoves);
}

void GetCaptures(const State& state, Moves& legalMoves) {
  // TODO: fix pawns, check other types
  const auto mask = GetPiecesOfColor(state, FlipColor(state.turn));
  return GetMoves(const_cast<State&>(state), mask, legalMoves);
}

void MakeMove(State& state, Move move, Undo& undo) {
  const auto from = GetFrom(move);
  const auto to = GetTo(move);
  const auto fromPiece = state.board[from];
  const auto toPiece = state.board[to];

  undo.fromPiece = fromPiece;
  undo.toPiece = toPiece;
  undo.halfmoveClock = state.halfmoveClock;
  undo.fullmoveNumber = state.fullmoveNumber;
  undo.turn = state.turn;
  undo.enPassant = state.enPassant;
  undo.castlingRightsMask = state.castlingRightsMask;
  undo.hash = state.hash;
  undo.history = state.history;

  state.board[to] = fromPiece;
  state.board[from] = Piece::kNone;
  UpdatePiece(state.hash, from, fromPiece);
  UpdatePiece(state.hash, to, fromPiece);
  if (toPiece != Piece::kNone) {
    UpdatePiece(state.hash, to, toPiece);
  }

  if (state.enPassant != Square::kInvalid) {
    UpdateEnPassant(state.hash, GetFile(state.enPassant));
  }

  const auto moveType = GetType(move);
  switch (moveType) {
  case MoveType::kEnPassant:
    if (state.turn == Color::kWhite) {
      const auto captureSquare = static_cast<Square>(state.enPassant - 8);
      state.board[captureSquare] = Piece::kNone;
      UpdatePiece(state.hash, captureSquare, kBlackPawn);
      state.bitboards[kBlackPawn] ^= BBFromSquare(captureSquare);
      state.bitboards[kNone] ^= BBFromSquare(captureSquare);
    } else if (state.turn == Color::kBlack) {
      const auto captureSquare = static_cast<Square>(state.enPassant + 8);
      state.board[captureSquare] = Piece::kNone;
      UpdatePiece(state.hash, captureSquare, kWhitePawn);
      state.bitboards[kWhitePawn] ^= BBFromSquare(captureSquare);
      state.bitboards[kNone] ^= BBFromSquare(captureSquare);
    }
    break;
  case MoveType::kKingCastle:
    switch (to) {
    case G1:
      std::swap(state.board[H1], state.board[F1]);
      UpdatePiece(state.hash, F1, kWhiteRook);
      UpdatePiece(state.hash, H1, kWhiteRook);
      state.bitboards[kWhiteRook] ^= BBFromSquare(H1) | BBFromSquare(F1);
      state.bitboards[kNone] ^= BBFromSquare(H1) | BBFromSquare(F1);
      break;
    case G8:
      std::swap(state.board[H8], state.board[F8]);
      UpdatePiece(state.hash, F8, kBlackRook);
      UpdatePiece(state.hash, H8, kBlackRook);
      state.bitboards[kBlackRook] ^= BBFromSquare(H8) | BBFromSquare(F8);
      state.bitboards[kNone] ^= BBFromSquare(H8) | BBFromSquare(F8);
      break;
    }
    break;
  case MoveType::kQueenCastle:
    switch (to) {
    case C1:
      std::swap(state.board[A1], state.board[D1]);
      UpdatePiece(state.hash, D1, kWhiteRook);
      UpdatePiece(state.hash, A1, kWhiteRook);
      state.bitboards[kWhiteRook] ^= BBFromSquare(A1) | BBFromSquare(D1);
      state.bitboards[kNone] ^= BBFromSquare(A1) | BBFromSquare(D1);
      break;
    case C8:
      std::swap(state.board[A8], state.board[D8]);
      UpdatePiece(state.hash, D8, kBlackRook);
      UpdatePiece(state.hash, A8, kBlackRook);
      state.bitboards[kBlackRook] ^= BBFromSquare(A8) | BBFromSquare(D8);
      state.bitboards[kNone] ^= BBFromSquare(A8) | BBFromSquare(D8);
      break;
    }
    break;
  case MoveType::kQueenPromotion:
    UpdatePiece(state.hash, to, state.board[to]);
    state.board[to] = MakePiece(state.turn, BasePiece::kQueen);
    UpdatePiece(state.hash, to, state.board[to]);
    break;
  case MoveType::kRookPromotion:
    UpdatePiece(state.hash, to, state.board[to]);
    state.board[to] = MakePiece(state.turn, BasePiece::kRook);
    UpdatePiece(state.hash, to, state.board[to]);
    break;
  case MoveType::kBishopPromotion:
    UpdatePiece(state.hash, to, state.board[to]);
    state.board[to] = MakePiece(state.turn, BasePiece::kBishop);
    UpdatePiece(state.hash, to, state.board[to]);
    break;
  case MoveType::kKnightPromotion:
    UpdatePiece(state.hash, to, state.board[to]);
    state.board[to] = MakePiece(state.turn, BasePiece::kKnight);
    UpdatePiece(state.hash, to, state.board[to]);
    break;
  }

  state.bitboards[Piece::kNone] ^= BBFromSquare(from);
  state.bitboards[fromPiece] ^= BBFromSquare(from);
  state.bitboards[toPiece] ^= BBFromSquare(to);
  state.bitboards[state.board[to]] ^= BBFromSquare(to);

  state.enPassant = EvaluateEnPassant(from, to, fromPiece);
  if (state.enPassant != Square::kInvalid) {
    UpdateEnPassant(state.hash, GetFile(state.enPassant));
  }

  const auto oldCastlingRights = state.castlingRightsMask;
  UpdateCastlingRights(state.hash, state.castlingRightsMask);
  UpdateCastlingState(state, from, to, GetBasePiece(fromPiece), GetBasePiece(toPiece));
  UpdateCastlingRights(state.hash, state.castlingRightsMask);

  const auto resetClock = GetBasePiece(fromPiece) == BasePiece::kPawn || toPiece != Piece::kNone ||
                          moveType == MoveType::kEnPassant;
  state.halfmoveClock = resetClock ? 0 : state.halfmoveClock + 1;
  state.fullmoveNumber += state.turn == Color::kBlack ? 1 : 0;

  state.turn = FlipColor(state.turn);
  UpdateTurn(state.hash);

  const auto irreversible = GetBasePiece(fromPiece) == BasePiece::kPawn ||
                            toPiece != Piece::kNone ||
                            state.castlingRightsMask != oldCastlingRights;
  if (irreversible) {
    state.history.clear();
  }
  state.history.push_back(state.hash);
}

void MakeMove(State& state, Move move) {
  auto undo = Undo{};
  MakeMove(state, move, undo);
}

void UndoMove(State& state, Move move, const Undo& undo) {
  state.halfmoveClock = undo.halfmoveClock;
  state.fullmoveNumber = undo.fullmoveNumber;
  state.turn = undo.turn;
  state.enPassant = undo.enPassant;
  state.castlingRightsMask = undo.castlingRightsMask;
  state.hash = undo.hash;
  const auto from = GetFrom(move);
  const auto to = GetTo(move);
  const auto toPiece = state.board[to];
  state.board[from] = undo.fromPiece;
  state.board[to] = undo.toPiece;
  state.history = undo.history;

  const auto moveType = GetType(move);
  switch (moveType) {
  case MoveType::kEnPassant:
    if (state.turn == Color::kWhite) {
      const auto captureSquare = static_cast<Square>(state.enPassant - 8);
      state.board[captureSquare] = Piece::kBlackPawn;
      state.bitboards[kBlackPawn] ^= BBFromSquare(captureSquare);
      state.bitboards[kNone] ^= BBFromSquare(captureSquare);
    } else if (state.turn == Color::kBlack) {
      const auto captureSquare = static_cast<Square>(state.enPassant + 8);
      state.board[captureSquare] = Piece::kWhitePawn;
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
  }

  state.bitboards[Piece::kNone] ^= BBFromSquare(from);
  state.bitboards[undo.fromPiece] ^= BBFromSquare(from);
  state.bitboards[undo.toPiece] ^= BBFromSquare(to);
  state.bitboards[toPiece] ^= BBFromSquare(to);
}

} // namespace chess