#pragma once

#include "chess/core/board.h"
#include "chess/core/color.h"
#include "chess/core/move.h"
#include "chess/core/square.h"
#include "chess/core/state.h"

#include <optional>
#include <vector>

class ImVec2;

namespace chess {

class Chess;

class ChessBoardPanel {
public:
  ChessBoardPanel();
  void OnUIRender(const Chess& chess);
  std::optional<Move> PopPendingMove();

private:
  struct DrawData {
    Board board;
    Color turn;
    bool isInCheck;
    std::vector<Move> legalMoves;
  };

  void OnSquareClickedEvent(const Chess& chess, Square clickedSquare);
  void UpdateDrawData(const Chess& chess);

private:
  Square highlightedSquare_;
  DrawData drawData_;
  State lastState_;
  std::optional<Move> pendingMove_;
};

}; // namespace chess