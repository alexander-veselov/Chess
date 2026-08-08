#pragma once

#include "chess/application/chess_controller.h"
#include "chess/application/chess_view.h"
#include "chess/application/layer.h"
#include "chess/core/game.h"

namespace chess {

class ChessLayer : public Layer {
public:
  void OnUIRender() override;

private:
  Game game_;
  ChessController controller_;
  ChessView view_;
};

}; // namespace chess