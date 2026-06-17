#pragma once

#include "chess/game/state.h"

namespace chess {

class Game {
public:
  Game();
  Game(const State& state);

  const State& GetState() const;

private:
  State state_;
};

}