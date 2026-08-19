#include "chess/application/chess_information_panel.h"

#include "chess/application/analysis.h"
#include "chess/application/chess.h"
#include "chess/core/fen.h"

#include <imgui.h>

namespace chess {
namespace {

std::string MovesToString(const std::vector<Move>& moves) {
  if (moves.empty()) {
    return {};
  }
  auto result = std::string{};
  for (const auto move : moves) {
    result += MoveToString(move) + " ";
  }
  result.pop_back();
  return result;
}

} // namespace

void ChessInformationPanel::OnUIRender(Chess& chess, Analysis& analysis) {
  ImGui::BeginChild("ChessInformationPanel");

  const auto status = chess.GetStatus();
  ImGui::Text("%s", StatusToString(status).data());

  fen_ = StateToFEN(chess.GetState());
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  ImGui::InputText("##fen", fen_.data(), fen_.capacity(),
                   ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

  history_ = MovesToString(chess.GetHistory());
  const auto historySize = ImVec2{ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.f};
  ImGui::InputTextMultiline("##history", history_.data(), history_.capacity(), historySize,
                            ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly |
                            ImGuiInputTextFlags_::ImGuiInputTextFlags_WordWrap);

  // TODO: draw properly
  const auto searchInfo = analysis.GetSearchInfo();
  analysisLine_ = std::string{};
  analysisLine_ += "Depth: " + std::to_string(searchInfo.depth) + "\n";
  analysisLine_ += "Nodes: " + std::to_string(searchInfo.nodes) + "\n";
  analysisLine_ += "Nodes/s: " + std::to_string(searchInfo.nodesPerSecond) + "\n";
  analysisLine_ += "Moves: " + MovesToString(searchInfo.line);
  const auto analysisLineSize = ImGui::GetContentRegionAvail();
  ImGui::InputTextMultiline("##analysisLine", analysisLine_.data(), analysisLine_.capacity(), analysisLineSize,
                            ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly |
                            ImGuiInputTextFlags_::ImGuiInputTextFlags_WordWrap);
  
  ImGui::EndChild();
}

} // namespace chess