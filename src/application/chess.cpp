#include "chess/application/chess.h"
#include "chess/application/texture.h"

#include "chess/core/board.h"
#include "chess/core/game.h"

#include "chess/engine/engine.h"

#include <optional>
#include <unordered_map>

#include <imgui.h>

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

// TODO: implement properly
static Game game{};
static std::optional<Square> selected;

static Status status;
static bool statusNeedsRefresh = true;
Status GetStatus() {
  if (statusNeedsRefresh) {
    status = game.GetStatus();
    statusNeedsRefresh = false;
  }
  return status;
}

static std::vector<Move> legalMoves;
static Square legalMovesSquare;
static bool legalMovesNeedsRefresh = true;
std::vector<Move> GetLegalMoves(Square square) {
  if (legalMovesNeedsRefresh || !(square == legalMovesSquare)) {
    legalMoves = game.GetLegalMoves(square);
    legalMovesNeedsRefresh = false;
    legalMovesSquare = square;
  }
  return legalMoves;
}

static bool isInCheck = false;
static bool isInCheckNeedsRefresh = true;
bool IsInCheck() {
  if (isInCheckNeedsRefresh) {
    isInCheck = game.IsInCheck();
    isInCheckNeedsRefresh = false;
  }
  return isInCheck;
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

    const auto status = GetStatus();
    ImGui::SetWindowFontScale(3.0f);
    ImGui::Text("%s", StatusToString(status).data());
    ImGui::SetWindowFontScale(1.0f);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();

    HandleInput(game, selected, origin);
    DrawBoard(draw_list, game, origin, selected);

    if (status == Status::kBlackToMove) {
      const auto engineMove = BestMove(game.GetState());
      game.MakeMove(engineMove);
      statusNeedsRefresh = true;
      legalMovesNeedsRefresh = true;
      isInCheckNeedsRefresh = true;
    }

    ImGui::End();
    ImGui::PopStyleVar();
  }

private:
  void HandleInput(Game& game, std::optional<Square>& fromSquare, const ImVec2& origin) {
    if (!ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
      return;
    }

    ImVec2 mouse = ImGui::GetMousePos();
    Square toSquare = ScreenToSquare(origin, mouse);

    if (!IsValidSquare(toSquare)) {
      return;
    }

    const Board& board = game.GetState().board;

    Piece toPiece = board[toSquare];

    if (!fromSquare) {
      if (toPiece != Piece::kNone && game.CanMove(toSquare)) {
        fromSquare = toSquare;
      }
    } else {
      Piece fromPiece = board[*fromSquare];
      if (toSquare == *fromSquare) {
        fromSquare = std::nullopt;
      } else if (GetPieceColor(toPiece) == GetPieceColor(fromPiece)) {
        fromSquare = toSquare;
      } else {
        
        auto moveType = MoveType::kNormal;
        if (GetBasePiece(fromPiece) == BasePiece::kPawn &&
            (GetRank(toSquare) == Rank::_1 || GetRank(toSquare) == Rank::_8)) {
          moveType = MoveType::kQueenPromotion; // TODO: implement promotion GUI
        }
        auto move = CreateMove(*fromSquare, toSquare, moveType);
        game.MakeMove(move);
        statusNeedsRefresh = true;
        legalMovesNeedsRefresh = true;
        isInCheckNeedsRefresh = true;
        fromSquare.reset();
      }
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
      ImVec2 p0(origin.x + ToFile(GetFile(*selected)) * CellSize,
                origin.y + ToRank(GetRank(*selected)) * CellSize);
      ImVec2 p1(p0.x + CellSize, p0.y + CellSize);
      for (const auto move : GetLegalMoves(*selected)) {
        ImVec2 p3(origin.x + ToFile(GetFile(GetTo(move))) * CellSize + CellSize / 2,
                  origin.y + ToRank(GetRank(GetTo(move))) * CellSize + CellSize / 2);

        const auto color = IM_COL32(0, 0, 0, 64);
        if (board[GetTo(move)] != Piece::kNone) {
          const auto thickness = CellSize / 12.f;
          draw_list->AddCircle(p3, CellSize / 2.f - thickness / 2.f + 1.f, color, 0, thickness);
        } else {
          draw_list->AddCircleFilled(p3, CellSize / 6.f, color);
        }
      }
      draw_list->AddRectFilled(p0, p1, IM_COL32(255, 255, 0, 80));
    }

    if (IsInCheck()) {
      DrawCheck(draw_list, origin, game.GetState());
    }
    DrawPieces(draw_list, origin, board);
  }

  void DrawCheck(ImDrawList* draw_list, const ImVec2& origin, const State& state) {
    const auto king = state.turn == Color::kWhite ? Piece::kWhiteKing : Piece::kBlackKing;
    for (int y = 0; y < kBoardSize; y++) {
      for (int x = 0; x < kBoardSize; x++) {
        if (state.board[CreateSquare(static_cast<File>(x), static_cast<Rank>(y))] != king) {
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
        const auto square = CreateSquare(static_cast<File>(x), static_cast<Rank>(y));
        Piece p = board[square];
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

    return CreateSquare(ToFile(x), ToRank(y));
  }

  bool IsValidSquare(const Square& s) {
    return GetFile(s) >= 0 && GetFile(s) < kBoardSize && GetRank(s) >= 0 && GetRank(s) < kBoardSize;
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