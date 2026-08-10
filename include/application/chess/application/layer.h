#pragma once

namespace chess {

class Layer {
 public:
  virtual ~Layer() = default;
  virtual void OnUpdate() = 0;
  virtual void OnUIRender() = 0;
};

}