#include "chess/application/chess_layer.h"

#include "chess/core/square.h"

#include <imgui.h>

namespace chess {

void ChessLayer::OnUIRender() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

  ImGui::Begin("Chess", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse);

  const auto status = game_.GetStatus();
  ImGui::SetWindowFontScale(3.0f);
  ImGui::Text("%s", StatusToString(status).data());
  ImGui::SetWindowFontScale(1.0f);

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    const auto mousePosition = ImGui::GetMousePos();

    const auto square = view_.ScreenToSquare(mousePosition);
    if (square != Square::kInvalid) {
      controller_.SquareClickedEvent(game_, square);
    }
  }

  view_.Draw(game_, controller_.GetHighlightedSquare());

  ImGui::End();
  ImGui::PopStyleVar();
}

} // namespace chess