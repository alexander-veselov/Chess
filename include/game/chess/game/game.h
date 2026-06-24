#pragma once

#include "chess/game/state.h"

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
  bool Move(Square from, Square to);
  std::vector<Square> GetLegalMoves(Square square) const;

private:
  State state_;
};

}