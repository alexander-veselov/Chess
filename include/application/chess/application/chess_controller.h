#pragma once

#include "chess/core/square.h"

namespace chess {

class Game;

class ChessController {
public:
  ChessController();
  void SquareClickedEvent(Game& game, Square square);
  Square GetHighlightedSquare() const;

private:
  Square highlightedSquare_;
};

}; // namespace chess