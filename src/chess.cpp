#include "chess.h"
#include "imgui.h"

namespace chess {

class ExampleLayer : public chess::Layer {
 public:
  virtual void OnUIRender() override {
    ImGui::Begin("Hello");
    ImGui::Button("Button");
    ImGui::End();

    ImGui::ShowDemoWindow();
  }
};

std::shared_ptr<Application> CreateApplication() {
  auto specification = chess::Application::Specification{};
  specification.name = "Chess";
  specification.width = 2560;
  specification.height = 1440;

  auto application = std::make_shared<chess::Application>(specification);
  application->PushLayer(std::make_shared<ExampleLayer>());
  return application;
}

}  // namespace chess