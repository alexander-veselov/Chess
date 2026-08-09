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

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

  ImGui::Begin("Chess", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse);

  const auto status = chess_.GetStatus();
  ImGui::SetWindowFontScale(3.0f);
  ImGui::Text("%s", StatusToString(status).data());
  ImGui::SetWindowFontScale(1.0f);

  chessBoardPanel_.OnUIRender(chess_);

  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace chess