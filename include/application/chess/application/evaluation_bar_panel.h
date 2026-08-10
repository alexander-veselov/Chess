#pragma once

#include "chess/application/analysis.h"

namespace chess {

class EvaluationBarPanel {
public:
  void OnUIRender(const Analysis& analysis);
};

}; // namespace chess