#include "chess/application/application.h"
#include "chess/application/chess_layer.h"
#include "chess/core/constants.h"

#include <memory>

namespace chess {

std::shared_ptr<Application> CreateApplication() {
  static constexpr auto CellSize = 200.0f; // TODO: remove
  auto specification = chess::Application::Specification{};
  specification.name = "Chess";
  specification.width = kBoardSize * CellSize;
  specification.height = kBoardSize * CellSize + 44; // TODO: fix 44

  auto application = std::make_shared<chess::Application>(specification);
  application->PushLayer(std::make_shared<ChessLayer>());
  return application;
}

} // namespace chess

int main() {
  auto application = chess::CreateApplication();
  application->Run();
  return 0;
}