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

Square EvaluateEnPassant(const Board& board, Square from, Square to, Piece fromPiece) {
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
    if ((BlackPawnAttacks(state.bitboards[kBlackPawn]) & target) != 0) {
      return true;
    }
    if ((KingAttacks(square) & state.bitboards[kBlackKing]) != 0) {
      return true;
    }
    if ((KnightAttacks(square) & state.bitboards[kBlackKnight]) != 0) {
      return true;
    }
    const auto bishopAttacks = BishopAttacks(square, occupancy);
    if ((bishopAttacks & state.bitboards[kBlackBishop]) != 0) {
      return true;
    }
    const auto rookAttacks = RookAttacks(square, occupancy);
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
    if ((KingAttacks(square) & state.bitboards[kWhiteKing]) != 0) {
      return true;
    }
    if ((KnightAttacks(square) & state.bitboards[kWhiteKnight]) != 0) {
      return true;
    }
    const auto bishopAtacks = BishopAttacks(square, occupancy);
    if ((bishopAtacks & state.bitboards[kWhiteBishop]) != 0) {
      return true;
    }
    const auto rookAttacks = RookAttacks(square, occupancy);
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
    return IsAttacked(state, turn, LSB(state.bitboards[kWhiteKing]));
  } else if (turn == Color::kBlack) {
    return IsAttacked(state, turn, LSB(state.bitboards[kBlackKing]));
  }
  return false;
}

void GetKingMoves(const State& state, Bitboard allyPieces, Bitboard kings, Moves& moves) {
  const auto isInCheck = IsInCheck(state, state.turn);
  while (kings) {
    const auto from = PopLSB(kings);
    auto attacks = KingAttacks(from) & ~allyPieces;
    while (attacks) {
      const auto to = PopLSB(attacks);
      moves.push_back(CreateMove(from, to));
    }
    if (!isInCheck) {
      if (GetPieceColor(state.board[from]) == Color::kWhite) {
        if (CanCastle(state.castlingRightsMask, CastlingRight::kWhiteKingSide)) {
          if (state.board[F1] == Piece::kNone && state.board[G1] == Piece::kNone &&
              !IsAttacked(state, state.turn, F1)) {
            moves.push_back(CreateMove(from, G1, MoveType::kKingCastle));
          }
        }
        if (CanCastle(state.castlingRightsMask, CastlingRight::kWhiteQueenSide)) {
          if (state.board[B1] == Piece::kNone && state.board[C1] == Piece::kNone &&
              state.board[D1] == Piece::kNone && !IsAttacked(state, state.turn, C1) &&
              !IsAttacked(state, state.turn, D1)) {
            moves.push_back(CreateMove(from, C1, MoveType::kQueenCastle));
          }
        }
      } else {
        if (CanCastle(state.castlingRightsMask, CastlingRight::kBlackKingSide)) {
          if (state.board[F8] == Piece::kNone && state.board[G8] == Piece::kNone &&
              !IsAttacked(state, state.turn, F8)) {
            moves.push_back(CreateMove(from, G8, MoveType::kKingCastle));
          }
        }
        if (CanCastle(state.castlingRightsMask, CastlingRight::kBlackQueenSide)) {
          if (state.board[B8] == Piece::kNone && state.board[C8] == Piece::kNone &&
              state.board[D8] == Piece::kNone && !IsAttacked(state, state.turn, C8) &&
              !IsAttacked(state, state.turn, D8)) {
            moves.push_back(CreateMove(from, C8, MoveType::kQueenCastle));
          }
        }
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
  while (pawns) {
    const auto from = PopLSB(pawns);
    const auto pawn = BBFromSquare(from);
    auto attacks = kEmptyBoard;
    if (state.turn == Color::kWhite) {
      attacks |= WhitePawnSinglePushes(pawn, ~state.bitboards[kNone]);
      attacks |= WhitePawnDoublePushes(pawn, ~state.bitboards[kNone]);
      attacks |= WhitePawnAttacks(pawn) & ~(allyPieces | state.bitboards[kNone]);
    } else {
      attacks |= BlackPawnSinglePushes(pawn, ~state.bitboards[kNone]);
      attacks |= BlackPawnDoublePushes(pawn, ~state.bitboards[kNone]);
      attacks |= BlackPawnAttacks(pawn) & ~(allyPieces | state.bitboards[kNone]);
    }
    while (attacks) {
      const auto to = PopLSB(attacks);
      const auto toRank = GetRank(to);
      if (toRank == Rank::_1 || toRank == Rank::_8) {
        moves.push_back(CreateMove(from, to, MoveType::kBishopPromotion));
        moves.push_back(CreateMove(from, to, MoveType::kRookPromotion));
        moves.push_back(CreateMove(from, to, MoveType::kKnightPromotion));
        moves.push_back(CreateMove(from, to, MoveType::kQueenPromotion));
      } else {
        moves.push_back(CreateMove(from, to));
      }
    }
    if (state.enPassant != Square::kInvalid) {
      const auto fileShift = GetFile(state.enPassant) - GetFile(from);
      const auto rankShift = GetRank(state.enPassant) - GetRank(from);
      const auto direction = state.turn == Color::kWhite ? 1 : -1;
      if (std::abs(fileShift) == 1 && rankShift == direction) {
        moves.push_back(CreateMove(from, state.enPassant, MoveType::kEnPassant));
      }
    }
  }
}

bool CanMoveInTurn(const State& state, Square square) {
  return GetPieceColor(state.board[square]) == state.turn;
}

void UpdateCastlingState(State& state, const Move& move) {
  const auto from = GetFrom(move);
  const auto to = GetTo(move);
  const auto fromBasePiece = GetBasePiece(state.board[from]);
  const auto toBasePiece = GetBasePiece(state.board[to]);
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

bool CanMove(const State& state, Square square) {
  // TODO: improve that function
  auto legalMoves = Moves{};
  GetLegalMoves(state, legalMoves);
  for (const auto move : legalMoves) {
    if (GetFrom(move) == square) {
      return true;
    }
  }
  return false;
}

bool HasAvailableMoves(const State& state) {
  auto legalMoves = Moves{};
  GetLegalMoves(state, legalMoves);
  return !legalMoves.empty();
}

bool LegalMove(State& state, const Move& move) {
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
  GetQueenMoves(  state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kQueen  )], moves);
  GetPawnMoves(   state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kPawn   )], moves);
  GetRookMoves(   state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kRook   )], moves);
  GetBishopMoves( state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kBishop )], moves);
  GetKnightMoves( state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kKnight )], moves);
  GetKingMoves(   state, allyPieces, state.bitboards[MakePiece(state.turn, BasePiece::kKing   )], moves);
  for (const auto move : moves) {
    auto newState = State{state};
    MakeMove(newState, move);
    if (!IsInCheck(newState, state.turn)) {
      legalMoves.push_back(move);
    }
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
  const auto resetClock = GetBasePiece(fromPiece) == BasePiece::kPawn || toPiece != Piece::kNone;
  state.halfmoveClock = resetClock ? 0 : state.halfmoveClock + 1;
  state.fullmoveNumber += state.turn == Color::kBlack ? 1 : 0;
  state.turn = FlipColor(state.turn);
  state.enPassant = EvaluateEnPassant(state.board, from, to, fromPiece);

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