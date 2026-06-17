#include "chess/application/chess.h"
#include "chess/application/texture.h"

#include <array>
#include <unordered_map>

#include "imgui.h"

namespace chess {

static constexpr int BoardSize = 8;
static constexpr float CellSize = 200.0f;

enum class Piece : uint8_t {
  kNone,
  kWhiteKing,
  kWhiteQueen,
  kWhiteRook,
  kWhiteBishop,
  kWhiteKnight,
  kWhitePawn,
  kBlackKing,
  kBlackQueen,
  kBlackRook,
  kBlackBishop,
  kBlackKnight,
  kBlackPawn
};

struct Board {
  std::array<std::array<Piece, BoardSize>, BoardSize> data_;
};

static Board CreateDefaultBoard() {
  Board b{};

  auto& d = b.data_;

  for (auto& row : d) {
    for (auto& c : row) {
      c = Piece::kNone;
    }
  }

  d[0][0] = Piece::kBlackRook;
  d[0][1] = Piece::kBlackKnight;
  d[0][2] = Piece::kBlackBishop;
  d[0][3] = Piece::kBlackQueen;
  d[0][4] = Piece::kBlackKing;
  d[0][5] = Piece::kBlackBishop;
  d[0][6] = Piece::kBlackKnight;
  d[0][7] = Piece::kBlackRook;

  for (int i = 0; i < BoardSize; i++) {
    d[1][i] = Piece::kBlackPawn;
  }

  for (int i = 0; i < BoardSize; i++) {
    d[6][i] = Piece::kWhitePawn;
  }

  d[7][0] = Piece::kWhiteRook;
  d[7][1] = Piece::kWhiteKnight;
  d[7][2] = Piece::kWhiteBishop;
  d[7][3] = Piece::kWhiteQueen;
  d[7][4] = Piece::kWhiteKing;
  d[7][5] = Piece::kWhiteBishop;
  d[7][6] = Piece::kWhiteKnight;
  d[7][7] = Piece::kWhiteRook;

  return b;
}

class ExampleLayer : public chess::Layer {
public:
  void OnUIRender() override {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("Chess", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    static auto board = CreateDefaultBoard();
    DrawBoard(board);

    ImGui::End();

    ImGui::PopStyleVar();
  }

private:
  void DrawBoard(const Board& board) {
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

    for (int y = 0; y < BoardSize; y++) {
      for (int x = 0; x < BoardSize; x++) {
        Piece p = board.data_[y][x];

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
  specification.width = BoardSize * CellSize;
  specification.height = BoardSize * CellSize;

  auto application = std::make_shared<chess::Application>(specification);
  application->PushLayer(std::make_shared<ExampleLayer>());
  return application;
}

} // namespace chess