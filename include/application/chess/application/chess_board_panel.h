#pragma once

#include "chess/core/board.h"
#include "chess/core/color.h"
#include "chess/core/moves.h"
#include "chess/core/square.h"

class ImVec2;

namespace chess {

class Chess;

class ChessBoardPanel {
public:
  ChessBoardPanel();
  void OnUIRender(Chess& chess);

private:
  struct DrawData {
    Board board;
    Color turn;
    bool isInCheck;
    Moves legalMoves;
  };

  void OnSquareClickedEvent(Chess& chess, Square clickedSquare);
  void UpdateDrawData(const Chess& chess);

private:
  Square highlightedSquare_;
  DrawData drawData_;
  bool dirty_;
};

}; // namespace chess