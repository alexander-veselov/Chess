#include "chess/application/controls_panel.h"

#include <imgui.h>

namespace chess {

ControlsPanel::ControlsPanel()
  : enableAnalysis_{true},
    flipBoard_{false},
    autoPilotOption_{0} {
}

bool ControlsPanel::GetEnableAnalysis() const {
  return enableAnalysis_;
}

bool ControlsPanel::GetFlipBoard() const {
  return flipBoard_;
}

bool ControlsPanel::GetUseAutoPilot(Color color) const {
  if (!enableAnalysis_) {
    return false;
  }
  switch (autoPilotOption_) {
  case AutoPilotOption::kDisabled:
    return false;
  case AutoPilotOption::kEnabled:
    return true;
  case AutoPilotOption::kWhite:
    return color == Color::kWhite;
  case AutoPilotOption::kBlack:
    return color == Color::kBlack;
  }
  return false;
}

void ControlsPanel::OnUIRender() {
  ImGui::BeginChild("ControlsPanel");
  ImGui::Checkbox("Enable analysis. Auto pilot", &enableAnalysis_);
  ImGui::SameLine();
  ImGui::BeginDisabled(!enableAnalysis_);
  auto autoPilotOption = static_cast<int>(autoPilotOption_);
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  ImGui::Combo("##autoPilot", &autoPilotOption, kAutoPilotOptions.data(), kAutoPilotOptions.size());
  autoPilotOption_ = static_cast<AutoPilotOption>(autoPilotOption);
  ImGui::EndDisabled();
  ImGui::Checkbox("Flip board", &flipBoard_);
  ImGui::EndChild();
}

} // namespace chess