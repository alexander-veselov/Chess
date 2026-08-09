#include "chess/application/chess_view.h"

#include "chess/application/chess.h"
#include "chess/application/texture.h"
#include "chess/core/constants.h"
#include "chess/core/square.h"

#include <utility>
#include <unordered_map>

#include <imgui.h>

namespace chess {
namespace {

// TODO: make configurable, remove global variables
constexpr auto kPerspectiveColor = Color::kWhite;
constexpr auto kCellSize = 200.0f;

constexpr auto kCheckCircleColor = IM_COL32(128, 0, 0, 64);
constexpr auto kDarkSquareColor = IM_COL32(118, 150, 86, 255);
constexpr auto kHighlightedSquareColor = IM_COL32(255, 255, 0, 80);
constexpr auto kLightSquareColor = IM_COL32(238, 238, 210, 255);
constexpr auto kMoveColor = IM_COL32(0, 0, 0, 64);

constexpr auto kMoveCircleSize = kCellSize / 6.f;
constexpr auto kCaptureCircleThickness = kCellSize / 12.f;
constexpr auto kCaptureCircleSize = kCellSize / 2.f - kCaptureCircleThickness / 2.f + 1.f;
constexpr auto kCheckCircleThickness = kCellSize / 12.f;
constexpr auto kCheckCircleRadius = kCellSize / 2.f - kCheckCircleThickness / 2.f + 1.f;

ImTextureID GetTexture(Piece piece) {
  // TODO: refactor
  constexpr auto kSize = 512;
  static auto kTextures = std::unordered_map<Piece, GLuint>{
      {Piece::kWhiteKing,   LoadTextureFromSVG("assets/Chess_klt45.svg", kSize, kSize)},
      {Piece::kWhiteQueen,  LoadTextureFromSVG("assets/Chess_qlt45.svg", kSize, kSize)},
      {Piece::kWhiteRook,   LoadTextureFromSVG("assets/Chess_rlt45.svg", kSize, kSize)},
      {Piece::kWhiteBishop, LoadTextureFromSVG("assets/Chess_blt45.svg", kSize, kSize)},
      {Piece::kWhiteKnight, LoadTextureFromSVG("assets/Chess_nlt45.svg", kSize, kSize)},
      {Piece::kWhitePawn,   LoadTextureFromSVG("assets/Chess_plt45.svg", kSize, kSize)},
      {Piece::kBlackKing,   LoadTextureFromSVG("assets/Chess_kdt45.svg", kSize, kSize)},
      {Piece::kBlackQueen,  LoadTextureFromSVG("assets/Chess_qdt45.svg", kSize, kSize)},
      {Piece::kBlackRook,   LoadTextureFromSVG("assets/Chess_rdt45.svg", kSize, kSize)},
      {Piece::kBlackBishop, LoadTextureFromSVG("assets/Chess_bdt45.svg", kSize, kSize)},
      {Piece::kBlackKnight, LoadTextureFromSVG("assets/Chess_ndt45.svg", kSize, kSize)},
      {Piece::kBlackPawn,   LoadTextureFromSVG("assets/Chess_pdt45.svg", kSize, kSize)}};

  auto it = kTextures.find(piece);
  if (it == kTextures.end()) {
    return ImTextureID{0};
  }

  return static_cast<ImTextureID>(static_cast<intptr_t>(it->second));
}

Rank AdjustRank(Rank rank) {
  return kPerspectiveColor == Color::kBlack ? rank : static_cast<Rank>(kBoardSize - rank - 1);
}

File AdjustFile(File file) {
  return kPerspectiveColor == Color::kBlack ? static_cast<File>(kBoardSize - file - 1) : file;
}

std::pair<ImVec2, ImVec2> RenderBox(const ImVec2& origin, Square square) {
  const auto rank = AdjustRank(GetRank(square));
  const auto file = AdjustFile(GetFile(square));
  const auto topLeft = ImVec2{origin.x + file * kCellSize, origin.y + rank * kCellSize};
  const auto bottomRight = ImVec2{topLeft.x + kCellSize, topLeft.y + kCellSize};
  return {topLeft, bottomRight};
}

ImVec2 Center(ImVec2 vector1, ImVec2 vector2) {
  return ImVec2{
      (vector1.x + vector2.x) / 2,
      (vector1.y + vector2.y) / 2,
  };
}

void DrawPieces(ImDrawList* drawList, const ImVec2& origin, const Board& board) {
  for (auto squareIndex = 0; squareIndex < kBoardTotalSize; ++squareIndex) {
    const auto square = static_cast<Square>(squareIndex);
    const auto piece = board[square];
    if (piece == Piece::kNone) {
      continue;
    }
    const auto texture = GetTexture(piece);
    if (texture == 0) {
      continue;
    }
    const auto [topLeft, bottomRight] = RenderBox(origin, square);
    drawList->AddImage(texture, topLeft, bottomRight);
  }
}

void DrawCheck(ImDrawList* drawList, const ImVec2& origin, const Board& board, Color turn) {
  const auto allyKing = turn == Color::kWhite ? Piece::kWhiteKing : Piece::kBlackKing;
  for (auto squareIndex = 0; squareIndex < kBoardTotalSize; ++squareIndex) {
    const auto square = static_cast<Square>(squareIndex);
    const auto piece = board[square];
    if (piece != allyKing) {
      continue;
    }
    const auto [topLeft, bottomRight] = RenderBox(origin, square);
    const auto center = Center(topLeft, bottomRight);
    drawList->AddCircle(center, kCheckCircleRadius, kCheckCircleColor, 0, kCheckCircleThickness);
  }
}

void DrawSquares(ImDrawList* drawList, const ImVec2& origin) {
  for (auto squareIndex = 0; squareIndex < kBoardTotalSize; ++squareIndex) {
    const auto square = static_cast<Square>(squareIndex);
    const auto [topLeft, bottomRight] = RenderBox(origin, square);
    const auto color = GetColor(square) == Color::kWhite ? kLightSquareColor : kDarkSquareColor;
    drawList->AddRectFilled(topLeft, bottomRight, color);
  }
}

void DrawMoves(ImDrawList* drawList, const ImVec2& origin, const Board& board, const Moves& moves) {
  for (const auto move : moves) {
    const auto to = GetTo(move);
    const auto [topLeft, bottomRight] = RenderBox(origin, to);
    const auto center = Center(topLeft, bottomRight);
    if (board[to] != Piece::kNone) {
      drawList->AddCircle(center, kCaptureCircleSize, kMoveColor, 0, kCaptureCircleThickness);
    } else {
      drawList->AddCircleFilled(center, kMoveCircleSize, kMoveColor);
    }
  }
}

void DrawBoard(ImDrawList* drawList, const Chess& game, const ImVec2& origin,
               Square highlightedSquare) {
  const auto& state = game.GetState();
  const auto& board = state.board;
  const auto turn = state.turn;
  DrawSquares(drawList, origin);
  if (highlightedSquare != Square::kInvalid) {
    const auto moves = game.GetLegalMoves(highlightedSquare);
    DrawMoves(drawList, origin, board, moves);
    const auto [topLeft, bottomRight] = RenderBox(origin, highlightedSquare);
    drawList->AddRectFilled(topLeft, bottomRight, kHighlightedSquareColor);
  }
  if (game.IsInCheck()) {
    DrawCheck(drawList, origin, board, turn);
  }
  DrawPieces(drawList, origin, board);
}

} // namespace

Square ChessView::ScreenToSquare(const ImVec2& mouse) {
  const auto origin = ImGui::GetCursorScreenPos();
  const auto file = static_cast<File>((mouse.x - origin.x) / kCellSize);
  const auto rank = static_cast<Rank>((mouse.y - origin.y) / kCellSize);
  return CreateSquare(AdjustFile(file), AdjustRank(rank));
}

void ChessView::Draw(const Chess& game, Square highlightedSquare) {
  const auto drawList = ImGui::GetWindowDrawList();
  const auto origin = ImGui::GetCursorScreenPos();
  DrawBoard(drawList, game, origin, highlightedSquare);
}

} // namespace chess