#pragma once

#include "chess/application/chess.h"
#include "chess/application/chess_board_panel.h"
#include "chess/application/layer.h"
#include "chess/core/square.h"

namespace chess {

class ChessLayer : public Layer {
public:
  void OnUIRender() override;

private:
  Chess chess_;
  ChessBoardPanel chessBoardPanel_;
};

}; // namespace chess