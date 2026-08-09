#include "chess/application/chess_information_panel.h"

#include "chess/application/chess.h"
#include "chess/core/fen.h"

#include <imgui.h>

namespace chess {

void ChessInformationPanel::OnUIRender(Chess& chess) {
  ImGui::BeginChild("ChessInformationPanel");

  const auto status = chess.GetStatus();
  ImGui::Text("%s", StatusToString(status).data());

  const auto fen = StateToFEN(chess.GetState());
  std::ranges::copy(fen, fenTextInputData_.begin());
  fenTextInputData_[fen.size()] = '\0';
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  ImGui::InputText("##fen", fenTextInputData_.data(), fenTextInputData_.size(),
                   ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

  ImGui::EndChild();
}

} // namespace chess