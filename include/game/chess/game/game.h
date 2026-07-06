#pragma once

#include "chess/core/state.h"
#include "chess/core/move.h"

#include <vector>

namespace chess {

enum class Status { kWhiteToMove, kBlackToMove, kWhiteWon, kBlackWon, kDraw };

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

}