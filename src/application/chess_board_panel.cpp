#include "chess/application/chess_board_panel.h"

#include "chess/application/chess.h"
#include "chess/application/texture.h"
#include "chess/application/ui_constants.h"
#include "chess/core/move.h"
#include "chess/core/piece.h"
#include "chess/core/square.h"

#include <utility>
#include <unordered_map>

#include <imgui.h>

namespace chess {
namespace {

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

Square ScreenToSquare(const ImVec2& origin, const ImVec2& mouse) {
  const auto fileIndex = std::floor((mouse.x - origin.x) / kCellSize);
  const auto rankIndex = std::floor((mouse.y - origin.y) / kCellSize);
  if (fileIndex < File::_A || fileIndex > File::_H ||
      rankIndex < Rank::_1 || rankIndex > Rank::_8) {
    return Square::kInvalid;
  }
  const auto file = static_cast<File>(fileIndex);
  const auto rank = static_cast<Rank>(rankIndex);
  return CreateSquare(AdjustFile(file), AdjustRank(rank));
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

MoveType DetermineMoveType(Square from, Square to, const Board& board) {
  const auto fromPiece = board[from];
  const auto fromBasePiece = GetBasePiece(fromPiece);
  const auto fileDiff = std::abs(GetFile(from) - GetFile(to));
  if (fromBasePiece == BasePiece::kKing && fileDiff == 2) {
    switch (to) {
    case G1:
    case G8:
      return MoveType::kKingCastle;
    case C1:
    case C8:
      return MoveType::kQueenCastle;
    }
  } else if (fromBasePiece == BasePiece::kPawn) {
    if (GetRank(to) == Rank::_1 || GetRank(to) == Rank::_8) {
      return MoveType::kQueenPromotion; // TODO: implement promotion GUI
    }
    const auto rankDiff = std::abs(GetRank(from) - GetRank(to));
    const auto toPiece = board[to];
    if (fileDiff == 1 && rankDiff == 1 && toPiece == Piece::kNone) {
      return MoveType::kEnPassant;
    }
  }
  return MoveType::kNormal;
}

} // namespace

ChessBoardPanel::ChessBoardPanel()
  : highlightedSquare_{Square::kInvalid},
    drawData_{{}, Color::kWhite, false, {}},
    dirty_{true} {
}

void ChessBoardPanel::UpdateDrawData(const Chess& chess) {
  const auto& chessState = chess.GetState();
  drawData_ = DrawData{};
  drawData_.board = chessState.board;
  drawData_.turn = chessState.turn;
  drawData_.isInCheck = chess.IsInCheck();
  drawData_.legalMoves = chess.GetLegalMoves(highlightedSquare_);
}

void ChessBoardPanel::OnSquareClickedEvent(Chess& chess, Square clickedSquare) {
  if (clickedSquare == Square::kInvalid) {
    highlightedSquare_ = Square::kInvalid;
    return;
  }

  const auto& gameState = chess.GetState();
  const auto& board = gameState.board;
  const auto clickedPiece = board[clickedSquare];
  const auto clickedOnAllyPiece =
      clickedPiece != Piece::kNone && GetPieceColor(clickedPiece) == gameState.turn;

  if (highlightedSquare_ == Square::kInvalid) {
    if (clickedOnAllyPiece) {
      highlightedSquare_ = clickedSquare;
    }
  } else {
    const auto highlightedPiece = board[highlightedSquare_];
    if (clickedSquare == highlightedSquare_) {
      highlightedSquare_ = Square::kInvalid;
    } else if (clickedOnAllyPiece) {
      highlightedSquare_ = clickedSquare;
    } else {
      const auto moveType = DetermineMoveType(highlightedSquare_, clickedSquare, board);
      const auto move = CreateMove(highlightedSquare_, clickedSquare, moveType);
      chess.MakeMove(move);
      highlightedSquare_ = Square::kInvalid;
    }
  }

  dirty_ = true;
}

void ChessBoardPanel::OnUIRender(Chess& chess) {
  ImGui::BeginChild("ChessBoardPanel", ImVec2(kCellSize * kBoardSize, kCellSize * kBoardSize));

  const auto origin = ImGui::GetCursorScreenPos();
  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    const auto square = ScreenToSquare(origin, ImGui::GetMousePos());
    OnSquareClickedEvent(chess, square);
  }

  if (dirty_) {
    UpdateDrawData(chess);
    dirty_ = false;
  }

  auto drawList = ImGui::GetWindowDrawList();
  DrawSquares(drawList, origin);
  if (highlightedSquare_ != Square::kInvalid) {
    DrawMoves(drawList, origin, drawData_.board, drawData_.legalMoves);
    const auto [topLeft, bottomRight] = RenderBox(origin, highlightedSquare_);
    drawList->AddRectFilled(topLeft, bottomRight, kHighlightedSquareColor);
  }
  if (drawData_.isInCheck) {
    DrawCheck(drawList, origin, drawData_.board, drawData_.turn);
  }
  DrawPieces(drawList, origin, drawData_.board);

  ImGui::EndChild();
}

} // namespace chess