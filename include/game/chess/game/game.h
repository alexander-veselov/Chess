#pragma once

#include "chess/game/state.h"

#include <vector>

namespace chess {

class Game {
public:
  Game();
  Game(const State& state, Color turn);

  const State& GetState() const;
  bool Move(Square from, Square to);
  std::vector<Square> GetLegalMoves(Square square) const;

private:
  State state_;
  Color turn_;
};

}