#include "chess/game/game.h"

namespace chess {

static Board CreateDefaultBoard() {
  auto board = Board{};

  for (auto& row : board) {
    for (auto& c : row) {
      c = Piece::kNone;
    }
  }

  board[0][0] = Piece::kBlackRook;
  board[0][1] = Piece::kBlackKnight;
  board[0][2] = Piece::kBlackBishop;
  board[0][3] = Piece::kBlackQueen;
  board[0][4] = Piece::kBlackKing;
  board[0][5] = Piece::kBlackBishop;
  board[0][6] = Piece::kBlackKnight;
  board[0][7] = Piece::kBlackRook;

  for (auto i = 0; i < kBoardSize; ++i) {
    board[1][i] = Piece::kBlackPawn;
  }

  for (auto i = 0; i < kBoardSize; ++i) {
    board[6][i] = Piece::kWhitePawn;
  }

  board[7][0] = Piece::kWhiteRook;
  board[7][1] = Piece::kWhiteKnight;
  board[7][2] = Piece::kWhiteBishop;
  board[7][3] = Piece::kWhiteQueen;
  board[7][4] = Piece::kWhiteKing;
  board[7][5] = Piece::kWhiteBishop;
  board[7][6] = Piece::kWhiteKnight;
  board[7][7] = Piece::kWhiteRook;

  return board;
}

Game::Game()
  : state_{State{CreateDefaultBoard()}} {
}

Game::Game(const State& state)
  : state_{state} {
}

const State& Game::GetState() const {
  return state_;
}

} // namespace chess