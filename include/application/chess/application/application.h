#pragma once

#include "layer.h"

#include <vector>
#include <string>
#include <memory>

struct GLFWwindow;

namespace chess {

class Application {
 public:
  struct Specification {
    std::string name;
    uint32_t width;
    uint32_t height;
  };

  Application(const Specification& specification);
  ~Application();
  void Run();

  void PushLayer(const std::shared_ptr<Layer>& layer);

 private:
  Specification specification_;
  GLFWwindow* window_handle_;
  std::vector<std::shared_ptr<Layer>> layer_stack_;
};

}  // namespace chess