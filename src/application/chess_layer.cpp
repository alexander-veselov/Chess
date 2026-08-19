#include "chess/application/chess_layer.h"

#include "chess/application/texture.h"
#include "chess/core/square.h"

#include <iostream>
#include <unordered_map>
#include <utility>

#include <imgui.h>

namespace chess {

void ChessLayer::OnUpdate() {
  const auto& state = chess_.GetState();
  analysis_.SetEnabled(controlsPanel_.GetEnableAnalysis());
  analysis_.Update(state);
  autoPilot_.Update(state, analysis_);
  if (IsGameOver(chess_.GetStatus())) {
    return;
  }
  if (controlsPanel_.GetUseAutoPilot(state.turn)) {
    const auto move = autoPilot_.PopPendingMove();
    if (move.has_value()) {
      if (!chess_.MakeMove(move.value())) {
        std::cout << "Auto pilot produced illegal move: " << MoveToString(move.value()) << std::endl;
      }
    }
  } else {
    const auto move = chessBoardPanel_.PopPendingMove();
    if (move.has_value()) {
      if (!chess_.MakeMove(move.value())) {
        std::cout << "Player produced illegal move" << MoveToString(move.value()) << std::endl;
      }
    }
  }
}

void ChessLayer::OnUIRender() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::Begin("Chess", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse);
    const auto flipBoard = controlsPanel_.GetFlipBoard();
    evaluationBarPanel_.OnUIRender(analysis_, flipBoard);
    ImGui::SameLine(); chessBoardPanel_.OnUIRender(chess_, flipBoard);
    ImGui::SameLine(); ImGui::BeginChild("ChessSidePanel");
      ImGui::SetNextWindowSize(
          ImVec2{ImGui::GetContentRegionAvail().x,
                 ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing() * 2.f});
      chessInformationPanel_.OnUIRender(chess_, analysis_);
      controlsPanel_.OnUIRender();
    ImGui::EndChild();
  ImGui::End();
}

} // namespace chess