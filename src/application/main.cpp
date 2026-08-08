#include "chess/application/application.h"
#include "chess/application/chess_layer.h"

#include <memory>

int main() {
  auto application = chess::Application{};
  if (!application.Initialize()) {
    return -1;
  }
  application.PushLayer(std::make_shared<chess::ChessLayer>());
  application.Run();
  return 0;
}