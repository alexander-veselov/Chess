#pragma once

#include "chess/application/layer.h"
#include "chess/application/window.h"

#include <memory>
#include <vector>

namespace chess {

class Application {
public:
  Application();
  ~Application();

  bool Initialize();
  void Run();
  void PushLayer(const std::shared_ptr<Layer>& layer);

private:
  std::unique_ptr<Window> window_;
  std::vector<std::shared_ptr<Layer>> layer_stack_;
};

} // namespace chess