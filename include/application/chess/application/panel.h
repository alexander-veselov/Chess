#pragma once

#include <string>

#include <imgui.h>

namespace chess {

class Panel {
public:
  Panel(std::string_view name, const ImVec2& size);
  void OnUIRender();

protected:
  virtual void OnUIRenderImpl() = 0;

private:
  std::string name_;
  ImVec2 size_;
};

}; // namespace chess