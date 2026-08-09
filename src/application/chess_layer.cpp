#include "chess/application/chess_layer.h"

#include "chess/application/texture.h"
#include "chess/core/square.h"

#include <utility>
#include <unordered_map>

#include <imgui.h>

namespace chess {

void ChessLayer::OnUIRender() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

  ImGui::Begin("Chess", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse);

  chessBoardPanel_.OnUIRender(chess_);
  ImGui::SameLine();
  chessInformationPanel_.OnUIRender(chess_);

  ImGui::End();
}

} // namespace chess