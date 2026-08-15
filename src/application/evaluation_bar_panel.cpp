#include "chess/application/evaluation_bar_panel.h"

#include "chess/application/ui_constants.h"

#include <algorithm>
#include <cmath>
#include <format>

#include <imgui.h>

constexpr auto kWhiteColor = IM_COL32(255, 255, 255, 255);
constexpr auto kBlackColor = IM_COL32(63, 61, 57, 255);

namespace chess {
namespace {

float EvaluationToDominance(float evaluation) {
  const auto sign = evaluation >= 0.f ? 1. : -1.;
  const auto dominance = std::pow(8., std::log10(std::abs(evaluation)) / std::log10(15.)) / 4.7;
  return sign * std::clamp(dominance, 0., 1.);
}

float EvaluationToBarSize(Score evaluation, float panelHeight) {
  const auto panelHeightHalf = panelHeight / 2.f;
  const auto panelHeightLimit = IsMateInN(evaluation) ? panelHeightHalf : panelHeightHalf * 0.9f;
  return panelHeightHalf + EvaluationToDominance(ScoreToFloat(evaluation)) * panelHeightLimit;
}

} // namespace

void EvaluationBarPanel::OnUIRender(const Analysis& analysis) {
  const auto panelSize = ImVec2{kCellSize / 4.f, ImGui::GetContentRegionAvail().y};
  ImGui::BeginChild("EvaluationBarPanel", panelSize);

  auto& style = ImGui::GetStyle();
  auto drawList = ImGui::GetWindowDrawList();
  const auto origin = ImGui::GetCursorScreenPos();

  if (!analysis.GetEnabled()) {
    const auto topLeft = origin;
    const auto bottomRight = ImVec2{origin.x + panelSize.x, origin.y + panelSize.y};
    const auto color = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_::ImGuiCol_ChildBg]);
    drawList->AddRectFilled(topLeft, bottomRight, color);
    ImGui::EndChild();
    return;
  }

  const auto evaluation = analysis.GetEvaluation();

  const auto currentPlayerBarSize = EvaluationToBarSize(evaluation, panelSize.y);
  const auto oppositePlayerBarSize = panelSize.y - currentPlayerBarSize;

  const auto upperColor = kPerspectiveColor == Color::kWhite ? kBlackColor : kWhiteColor;
  const auto lowerColor = kPerspectiveColor == Color::kWhite ? kWhiteColor : kBlackColor;

  const auto splitPosition =
      kPerspectiveColor == Color::kWhite ? oppositePlayerBarSize : currentPlayerBarSize;

  const auto topLeftUpper = origin;
  const auto bottomRightUpper = ImVec2{origin.x + panelSize.x, origin.y + splitPosition};
  drawList->AddRectFilled(topLeftUpper, bottomRightUpper, upperColor);

  const auto topLeftLower = ImVec2{origin.x, origin.y + splitPosition};
  const auto bottomRightLower = ImVec2{origin.x + panelSize.x, origin.y + panelSize.y};
  drawList->AddRectFilled(topLeftLower, bottomRightLower, lowerColor);

  ImGui::SetWindowFontScale(0.8f);

  const auto evaluationString = ScoreToString(evaluation);
  const auto stringSize = ImGui::CalcTextSize(evaluationString.data());
  const auto availableSize = ImGui::GetContentRegionAvail();
  const auto dummyHeight = availableSize.y - 2.f * (stringSize.y + 2.f * style.ItemSpacing.y);
  const auto offsetX = (availableSize.x - stringSize.x) * .5f;

  ImGui::PushStyleColor(ImGuiCol_Text, lowerColor);
  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + style.ItemSpacing.y);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
  ImGui::Text(evaluationString.data());
  ImGui::PopStyleColor();
  ImGui::Dummy(ImVec2(0, dummyHeight));
  ImGui::PushStyleColor(ImGuiCol_Text, upperColor);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
  ImGui::Text(evaluationString.data());
  ImGui::PopStyleColor();

  ImGui::SetWindowFontScale(1.f);

  ImGui::EndChild();
}

} // namespace chess