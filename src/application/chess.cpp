#include "chess/application/chess.h"
#include "chess/application/texture.h"

#include "imgui.h"

namespace chess {

static constexpr int BoardSize = 8;
static constexpr float CellSize = 200.0f;

class ExampleLayer : public chess::Layer {
public:
  void OnUIRender() override {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("Chess", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    DrawBoard();

    ImGui::End();

    ImGui::PopStyleVar();
  }

private:
  void DrawBoard() {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();

    for (int y = 0; y < BoardSize; y++) {
      for (int x = 0; x < BoardSize; x++) {

        ImVec2 p0(origin.x + x * CellSize, origin.y + y * CellSize);
        ImVec2 p1(p0.x + CellSize, p0.y + CellSize);

        bool dark = (x + y) % 2 == 1;

        ImU32 color =
            dark ? IM_COL32(118, 150, 86, 255) : IM_COL32(238, 238, 210, 255);

        draw_list->AddRectFilled(p0, p1, color);
      }
    }

    DrawPieces(draw_list, origin);
  }

  void DrawPieces(ImDrawList* draw_list, ImVec2 origin) {
    static GLuint tex = 0;
    static bool init = false;

    if (!init) {
      LoadTextureFromSVG(tex, "assets/Chess_bdt45.svg", CellSize, CellSize);
      init = true;
    }

    ImGui::Image((ImTextureID)(intptr_t)tex, ImVec2(CellSize, CellSize));
  }
};

std::shared_ptr<Application> CreateApplication() {
  auto specification = chess::Application::Specification{};
  specification.name = "Chess";
  specification.width = BoardSize * CellSize;
  specification.height = BoardSize * CellSize;

  auto application = std::make_shared<chess::Application>(specification);
  application->PushLayer(std::make_shared<ExampleLayer>());
  return application;
}

} // namespace chess