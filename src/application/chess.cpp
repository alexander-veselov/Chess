#include "chess/application/chess.h"
#include "chess/application/texture.h"

#include "chess/core/board.h"
#include "chess/game/game.h"

#include <unordered_map>

#include "imgui.h"

namespace chess {

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

    static auto game = Game{};
    DrawBoard(game.GetState().board_);

    ImGui::End();

    ImGui::PopStyleVar();
  }

private:
  void DrawBoard(const Board& board) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();

    for (int y = 0; y < kBoardSize; y++) {
      for (int x = 0; x < kBoardSize; x++) {

        ImVec2 p0(origin.x + x * CellSize, origin.y + y * CellSize);
        ImVec2 p1(p0.x + CellSize, p0.y + CellSize);

        bool dark = (x + y) % 2 == 1;

        ImU32 color =
            dark ? IM_COL32(118, 150, 86, 255) : IM_COL32(238, 238, 210, 255);

        draw_list->AddRectFilled(p0, p1, color);
      }
    }

    DrawPieces(draw_list, origin, board);
  }

  void DrawPieces(ImDrawList* draw_list, const ImVec2& origin,
                  const Board& board) {

    // TODO: create proper texture manager and load texture after proper opengl3 init
    static std::unordered_map<Piece, GLuint> kTextures{
      {Piece::kWhiteKing,   LoadTextureFromSVG("assets/Chess_klt45.svg", CellSize, CellSize)},
      {Piece::kWhiteQueen,  LoadTextureFromSVG("assets/Chess_qlt45.svg", CellSize, CellSize)},
      {Piece::kWhiteRook,   LoadTextureFromSVG("assets/Chess_rlt45.svg", CellSize, CellSize)},
      {Piece::kWhiteBishop, LoadTextureFromSVG("assets/Chess_blt45.svg", CellSize, CellSize)},
      {Piece::kWhiteKnight, LoadTextureFromSVG("assets/Chess_nlt45.svg", CellSize, CellSize)},
      {Piece::kWhitePawn,   LoadTextureFromSVG("assets/Chess_plt45.svg", CellSize, CellSize)},
      {Piece::kBlackKing,   LoadTextureFromSVG("assets/Chess_kdt45.svg", CellSize, CellSize)},
      {Piece::kBlackQueen,  LoadTextureFromSVG("assets/Chess_qdt45.svg", CellSize, CellSize)},
      {Piece::kBlackRook,   LoadTextureFromSVG("assets/Chess_rdt45.svg", CellSize, CellSize)},
      {Piece::kBlackBishop, LoadTextureFromSVG("assets/Chess_bdt45.svg", CellSize, CellSize)},
      {Piece::kBlackKnight, LoadTextureFromSVG("assets/Chess_ndt45.svg", CellSize, CellSize)},
      {Piece::kBlackPawn,   LoadTextureFromSVG("assets/Chess_pdt45.svg", CellSize, CellSize)}
    };

    for (int y = 0; y < kBoardSize; y++) {
      for (int x = 0; x < kBoardSize; x++) {
        Piece p = board[y][x];

        if (p == Piece::kNone) {
          continue;
        }

        auto it = kTextures.find(p);
        if (it == kTextures.end()) {
          continue;
        }

        ImVec2 pos = ImVec2(origin.x + x * CellSize, origin.y + y * CellSize);

        draw_list->AddImage((ImTextureID)(intptr_t)it->second, pos,
                            ImVec2(pos.x + CellSize, pos.y + CellSize));
      }
    }
  }
};

std::shared_ptr<Application> CreateApplication() {
  auto specification = chess::Application::Specification{};
  specification.name = "Chess";
  specification.width = kBoardSize * CellSize;
  specification.height = kBoardSize * CellSize;

  auto application = std::make_shared<chess::Application>(specification);
  application->PushLayer(std::make_shared<ExampleLayer>());
  return application;
}

} // namespace chess