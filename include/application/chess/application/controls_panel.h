#pragma once

#include "chess/core/color.h"

#include <array>

namespace chess {

class ControlsPanel {
public:
  ControlsPanel();
  void OnUIRender();
  bool GetEnableAnalysis() const;
  bool GetUseAutoPilot(Color color) const;

private:
  bool enableAnalysis_;
  enum class AutoPilotOption : int { kDisabled, kEnabled, kWhite, kBlack } autoPilotOption_;
  constexpr static auto kAutoPilotOptions = std::array{"Disabled", "Enabled", "White", "Black"};
};

}; // namespace chess