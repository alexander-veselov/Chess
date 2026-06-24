#include "chess/application/chess.h"
#include "chess/application/texture.h"

#include "chess/core/board.h"
#include "chess/game/game.h"

#include <optional>
#include <unordered_map>

#include "imgui.h"

namespace chess {

static constexpr auto CellSize = 200.0f;
static constexpr auto PlayerView = Color::kWhite;

static std::string_view StatusToString(Status status) {
  switch (status) {
  case Status::kWhiteToMove:
    return "White to move";
  case Status::kBlackToMove:
    return "Black to move";
  case Status::kWhiteWon:
    return "White won";
  case Status::kBlackWon:
    return "Black won";
  case Status::kDraw:
    return "Draw";
  }
  return "";
}

class ExampleLayer : public chess::Layer {
public:
  void OnUIRender() override {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));



    ImGui::Begin("Chess", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse);
    static Game game{};
    static std::optional<Square> selected;

    ImGui::SetWindowFontScale(3.0f);
    ImGui::Text("%s", StatusToString(game.GetStatus()).data());
    ImGui::SetWindowFontScale(1.0f);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();

    HandleInput(game, selected, origin);
    DrawBoard(draw_list, game, origin, selected);

    ImGui::End();
    ImGui::PopStyleVar();
  }

private:
  void HandleInput(Game& game, std::optional<Square>& selected, const ImVec2& origin) {
    if (!ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
      return;
    }

    ImVec2 mouse = ImGui::GetMousePos();
    Square sq = ScreenToSquare(origin, mouse);

    if (!IsValidSquare(sq)) {
      return;
    }

    const Board& board = game.GetState().board;
    Piece p = board[sq.rank][sq.file];

    if (!selected) {
      if (p != Piece::kNone && game.CanMove(sq)) {
        selected = sq;
      }
    } else {
      game.Move(*selected, sq);
      selected.reset();
    }
  }

  void DrawBoard(ImDrawList* draw_list, Game& game, const ImVec2& origin,
                 const std::optional<Square>& selected) {
    for (int y = 0; y < kBoardSize; y++) {
      for (int x = 0; x < kBoardSize; x++) {
        ImVec2 p0(origin.x + x * CellSize, origin.y + y * CellSize);

        ImVec2 p1(p0.x + CellSize, p0.y + CellSize);

        bool dark = (x + y) % 2 == 1;

        ImU32 color = dark ? IM_COL32(118, 150, 86, 255) : IM_COL32(238, 238, 210, 255);

        draw_list->AddRectFilled(p0, p1, color);
      }
    }

    const auto& board = game.GetState().board;

    if (selected) {
      ImVec2 p0(origin.x + ToFile(selected->file) * CellSize,
                origin.y + ToRank(selected->rank) * CellSize);
      ImVec2 p1(p0.x + CellSize, p0.y + CellSize);
      for (const auto move : game.GetLegalMoves({selected->rank, selected->file})) {
        ImVec2 p3(origin.x + ToFile(move.file) * CellSize + CellSize / 2,
                  origin.y + ToRank(move.rank) * CellSize + CellSize / 2);

        const auto color = IM_COL32(0, 0, 0, 64);
        if (board[move.rank][move.file] != Piece::kNone) {
          const auto thickness = CellSize / 12.f;
          draw_list->AddCircle(p3, CellSize / 2.f - thickness / 2.f + 1.f, color, 0, thickness);
        } else {
          draw_list->AddCircleFilled(p3, CellSize / 6.f, color);
        }
      }
      draw_list->AddRectFilled(p0, p1, IM_COL32(255, 255, 0, 80));
    }

    if (game.IsInCheck()) {
      DrawCheck(draw_list, origin, game.GetState());
    }
    DrawPieces(draw_list, origin, board);
  }

  void DrawCheck(ImDrawList* draw_list, const ImVec2& origin, const State& state) {
    const auto king = state.turn == Color::kWhite ? Piece::kWhiteKing : Piece::kBlackKing;
    for (int y = 0; y < kBoardSize; y++) {
      for (int x = 0; x < kBoardSize; x++) {
        if (state.board[y][x] != king) {
          continue;
        }
        ImVec2 p(origin.x + ToFile(x) * CellSize + CellSize / 2,
                 origin.y + ToRank(y) * CellSize + CellSize / 2);
        const auto color = IM_COL32(128, 0, 0, 64);
        const auto thickness = CellSize / 12.f;
        draw_list->AddCircle(p, CellSize / 2.f - thickness / 2.f + 1.f, color, 0, thickness);
      }
    }
  }

  void DrawPieces(ImDrawList* draw_list, const ImVec2& origin, const Board& board) {

    static std::unordered_map<Piece, GLuint> kTextures{
        {Piece::kWhiteKing, LoadTextureFromSVG("assets/Chess_klt45.svg", CellSize, CellSize)},
        {Piece::kWhiteQueen, LoadTextureFromSVG("assets/Chess_qlt45.svg", CellSize, CellSize)},
        {Piece::kWhiteRook, LoadTextureFromSVG("assets/Chess_rlt45.svg", CellSize, CellSize)},
        {Piece::kWhiteBishop, LoadTextureFromSVG("assets/Chess_blt45.svg", CellSize, CellSize)},
        {Piece::kWhiteKnight, LoadTextureFromSVG("assets/Chess_nlt45.svg", CellSize, CellSize)},
        {Piece::kWhitePawn, LoadTextureFromSVG("assets/Chess_plt45.svg", CellSize, CellSize)},
        {Piece::kBlackKing, LoadTextureFromSVG("assets/Chess_kdt45.svg", CellSize, CellSize)},
        {Piece::kBlackQueen, LoadTextureFromSVG("assets/Chess_qdt45.svg", CellSize, CellSize)},
        {Piece::kBlackRook, LoadTextureFromSVG("assets/Chess_rdt45.svg", CellSize, CellSize)},
        {Piece::kBlackBishop, LoadTextureFromSVG("assets/Chess_bdt45.svg", CellSize, CellSize)},
        {Piece::kBlackKnight, LoadTextureFromSVG("assets/Chess_ndt45.svg", CellSize, CellSize)},
        {Piece::kBlackPawn, LoadTextureFromSVG("assets/Chess_pdt45.svg", CellSize, CellSize)}};

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

        ImVec2 pos(origin.x + ToFile(x) * CellSize, origin.y + ToRank(y) * CellSize);

        draw_list->AddImage((ImTextureID)(intptr_t)it->second, pos,
                            ImVec2(pos.x + CellSize, pos.y + CellSize));
      }
    }
  }

  static Rank ToRank(int y) {
    return PlayerView == Color::kBlack ? static_cast<Rank>(y)
                                       : static_cast<Rank>(kBoardSize - y - 1);
  }

  static File ToFile(int x) {
    return static_cast<File>(x);
  }

  Square ScreenToSquare(const ImVec2& origin, const ImVec2& mouse) {
    int x = (int)((mouse.x - origin.x) / CellSize);
    int y = (int)((mouse.y - origin.y) / CellSize);

    return Square{ToRank(y), ToFile(x)};
  }

  bool IsValidSquare(const Square& s) {
    return s.file >= 0 && s.file < kBoardSize && s.rank >= 0 && s.rank < kBoardSize;
  }
};

std::shared_ptr<Application> CreateApplication() {
  auto specification = chess::Application::Specification{};
  specification.name = "Chess";
  specification.width = kBoardSize * CellSize;
  specification.height = kBoardSize * CellSize + 44; // TODO: fix 44

  auto application = std::make_shared<chess::Application>(specification);
  application->PushLayer(std::make_shared<ExampleLayer>());
  return application;
}

} // namespace chess