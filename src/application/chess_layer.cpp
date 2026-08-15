#include "chess/application/chess_layer.h"

#include "chess/application/texture.h"
#include "chess/core/square.h"

#include <utility>
#include <unordered_map>

#include <imgui.h>

namespace chess {

void ChessLayer::OnUpdate() {
  const auto& state = chess_.GetState();
  analysis_.SetEnabled(controlsPanel_.GetEnableAnalysis());
  analysis_.Update(state);
  if (controlsPanel_.GetUseAutoPilot(state.turn)) {
    if (analysis_.IsReady()) {
      const auto& line = analysis_.GetLine();
      if (!line.empty()) {
        chess_.MakeMove(line[0]);
      }
    }
  } else {
    const auto move = chessBoardPanel_.PopPendingMove();
    if (move.has_value()) {
      chess_.MakeMove(move.value());
    }
  }
}

void ChessLayer::OnUIRender() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::Begin("Chess", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse);
    evaluationBarPanel_.OnUIRender(analysis_);
    ImGui::SameLine(); chessBoardPanel_.OnUIRender(chess_);
    ImGui::SameLine(); ImGui::BeginChild("ChessSidePanel");
      ImGui::SetNextWindowSize(
          ImVec2{ImGui::GetContentRegionAvail().x,
                 ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing()});
      chessInformationPanel_.OnUIRender(chess_, analysis_);
      controlsPanel_.OnUIRender();
    ImGui::EndChild();
  ImGui::End();
}

} // namespace chess