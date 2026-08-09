#pragma once

#include "chess/core/square.h"

namespace chess {

class Chess;

class ChessController {
public:
  ChessController();
  void SquareClickedEvent(Chess& game, Square square);
  Square GetHighlightedSquare() const;

private:
  Square highlightedSquare_;
};

}; // namespace chess