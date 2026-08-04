#pragma once

#include "chess/core/status.h"
#include "chess/core/state.h"
#include "chess/core/square.h"
#include "chess/core/moves.h"
#include "chess/core/move.h"

namespace chess {

Status GetStatus(const State& state);
void GetLegalMoves(const State& state, Moves& legalMoves);
void MakeMove(State& state, Move move);

class Game {
public:
  Game();
  Game(const State& state);

  const State& GetState() const;
  const Status GetStatus() const;
  bool CanMove(Square square) const;
  bool IsInCheck() const;
  bool MakeMove(Move move);
  Moves GetLegalMoves(Square square) const;

private:
  State state_;
};

} // namespace chess