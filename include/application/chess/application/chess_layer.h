#pragma once

#include "chess/application/analysis.h"
#include "chess/application/auto_pilot.h"
#include "chess/application/chess.h"
#include "chess/application/chess_board_panel.h"
#include "chess/application/chess_information_panel.h"
#include "chess/application/controls_panel.h"
#include "chess/application/evaluation_bar_panel.h"
#include "chess/application/layer.h"
#include "chess/core/square.h"

namespace chess {

class ChessLayer : public Layer {
public:
  void OnUpdate() override;
  void OnUIRender() override;

private:
  Chess chess_;
  Analysis analysis_;
  AutoPilot autoPilot_;
  ChessBoardPanel chessBoardPanel_;
  ChessInformationPanel chessInformationPanel_;
  EvaluationBarPanel evaluationBarPanel_;
  ControlsPanel controlsPanel_;
};

}; // namespace chess