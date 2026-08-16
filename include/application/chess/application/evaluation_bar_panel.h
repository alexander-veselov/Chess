#pragma once

#include "chess/application/analysis.h"

namespace chess {

class EvaluationBarPanel {
public:
  void OnUIRender(const Analysis& analysis, bool flipBoard);
};

}; // namespace chess