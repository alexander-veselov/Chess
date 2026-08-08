#include "chess/application/chess_controller.h"

#include "chess/core/board.h"
#include "chess/core/color.h"
#include "chess/core/game.h"
#include "chess/core/move.h"
#include "chess/core/square.h"

namespace chess {
namespace {

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

ChessController::ChessController()
  : highlightedSquare_{Square::kInvalid} {
}

Square ChessController::GetHighlightedSquare() const {
  return highlightedSquare_;
}

void ChessController::SquareClickedEvent(Game& game, Square clickedSquare) {
  if (clickedSquare == Square::kInvalid) {
    return;
  }

  const auto& gameState = game.GetState();
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
      game.MakeMove(move);
      highlightedSquare_ = Square::kInvalid;
    }
  }
}

} // namespace chess