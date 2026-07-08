#pragma once

#include "chess/core/move.h"
#include "chess/core/piece.h"
#include "chess/core/square.h"
#include "chess/core/state.h"

#include <optional>
#include <vector>

namespace chess {

enum class Status { kWhiteToMove, kBlackToMove, kWhiteWon, kBlackWon, kDraw };

State CreateDefaultState();
bool IsValidSquare(int rank, int file);
Square ShiftSquare(Square square, int rankShift, int fileShift);
Piece Get(const Board& board, Square square);
void MoveOrCapture(Board& board, const Move& move);
void EnPassantCapture(Board& board, Square from, Square to);
std::optional<Square> EvaluateEnPassant(const Board& board, Square from, Square to);
void PushPawn(std::vector<Move>& moves, Square fromSquare, Square toSquare, Color color);
bool PushPawnIfEmpty(std::vector<Move>& moves, const Board& board, Square fromSquare, int rankShift,
                     int fileShift);
void PushPawnIfOpposite(std::vector<Move>& moves, const Board& board, Square fromSquare,
                        int rankShift, int fileShift);
bool PushIfEmptyOrOpposite(std::vector<Move>& moves, const Board& board, Square fromSquare,
                           Square toSquare);
bool PushIfEmptyOrOpposite(std::vector<Move>& moves, const Board& board, Square square,
                           int rankShift, int fileShift);
void GetKingMovesWithoutCastling(const State& state, Square square, std::vector<Move>& moves);
bool IsAttacked(const State& state, Color turn, Square square);
void GetKingMoves(const State& state, Square square, std::vector<Move>& moves);
void GetRookMoves(const State& state, Square square, std::vector<Move>& moves);
void GetBishopMoves(const State& state, Square square, std::vector<Move>& moves);
void GetQueenMoves(const State& state, Square square, std::vector<Move>& moves);
void GetKnightMoves(const State& state, Square square, std::vector<Move>& moves);
void GetPawnMoves(const State& state, Square square, std::vector<Move>& moves);
void GetMoves(const State& state, Square square, std::vector<Move>& moves);
bool IsInCheck(const State& state, Color turn);
bool CanMoveInTurn(const State& state, Square square);
void UpdateCastlingState(State& state, const Move& move);
void ProcessCastle(State& state, const Move& move);
void MakeMove(State& state, const Move& move);
void GetLegalMoves(const State& state, Square square, std::vector<Move>& legalMoves);
void GetAllLegalMoves(const State& state, std::vector<Move>& legalMoves);
bool CanMove(const State& state, Square square);
bool HasAvailableMoves(const State& state);
Status GetStatus(const State& state);
bool LegalMove(State& state, const Move& move);

class Game {
public:
  Game();
  Game(const State& state);

  const State& GetState() const;
  const Status GetStatus() const;
  bool CanMove(Square square) const;
  bool IsInCheck() const;
  bool MakeMove(const Move& move);
  std::vector<Move> GetLegalMoves(Square square) const;

private:
  State state_;
};

} // namespace chess