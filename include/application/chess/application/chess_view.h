#pragma once

#include "chess/core/color.h"
#include "chess/core/square.h"

class ImVec2;

namespace chess {

class Chess;

class ChessView {
public:
  void Draw(const Chess& game, Square highlightedSquare);
  Square ScreenToSquare(const ImVec2& mouse);
};

}; // namespace chess