#include "chess/game/game.h"

namespace chess {

static Board CreateDefaultBoard() {
  auto board = Board{};

  for (auto& row : board) {
    for (auto& c : row) {
      c = Piece::kNone;
    }
  }

  board[_8][_A] = Piece::kBlackRook;
  board[_8][_B] = Piece::kBlackKnight;
  board[_8][_C] = Piece::kBlackBishop;
  board[_8][_D] = Piece::kBlackQueen;
  board[_8][_E] = Piece::kBlackKing;
  board[_8][_F] = Piece::kBlackBishop;
  board[_8][_G] = Piece::kBlackKnight;
  board[_8][_H] = Piece::kBlackRook;

  for (auto i = 0; i < kBoardSize; ++i) {
    board[_7][i] = Piece::kBlackPawn;
  }

  for (auto i = 0; i < kBoardSize; ++i) {
    board[_2][i] = Piece::kWhitePawn;
  }

  board[_1][_A] = Piece::kWhiteRook;
  board[_1][_B] = Piece::kWhiteKnight;
  board[_1][_C] = Piece::kWhiteBishop;
  board[_1][_D] = Piece::kWhiteQueen;
  board[_1][_E] = Piece::kWhiteKing;
  board[_1][_F] = Piece::kWhiteBishop;
  board[_1][_G] = Piece::kWhiteKnight;
  board[_1][_H] = Piece::kWhiteRook;

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

bool Game::Move(Square from, Square to) {
  auto& board = state_.board;
  board[to.rank][to.file] = board[from.rank][from.file];
  board[from.rank][from.file] = Piece::kNone;
  return true;
}

} // namespace chess