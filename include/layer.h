#pragma once

namespace chess {

class Layer {
 public:
  virtual ~Layer() = default;
  virtual void OnUIRender() = 0;
};

}