#pragma once

#include "chess/application/chess.h"
#include "chess/application/chess_controller.h"
#include "chess/application/chess_view.h"
#include "chess/application/layer.h"

namespace chess {

class ChessLayer : public Layer {
public:
  void OnUIRender() override;

private:
  Chess game_;
  ChessController controller_;
  ChessView view_;
};

}; // namespace chess