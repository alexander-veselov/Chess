#include "chess/engine/move_picker.h"

#include "chess/engine/piece_values.h"
#include "chess/engine/score.h"

#include <array>

namespace chess {
namespace {

constexpr auto kBasePieceCount = static_cast<size_t>(BasePiece::kBasePieceCount);
constexpr auto kMvvLva = [] {
  auto table = std::array<std::array<Score, kBasePieceCount>, kBasePieceCount>{};
  for (auto attacker = 0; attacker < kBasePieceCount; ++attacker) {
    for (auto victim = 0; victim < kBasePieceCount; ++victim) {
      const auto victimPiece = static_cast<BasePiece>(victim);
      if (victimPiece == BasePiece::kNone || victimPiece == BasePiece::kKing) {
        continue;
      }
      table[attacker][victim] = kPieceValues[victim] * 10 - kPieceValues[attacker];
    }
  }
  return table;
}();

Score MvvLva(const Board& board, Move move) {
  const auto fromPiece = GetBasePiece(board[GetFrom(move)]);
  const auto toPiece = GetBasePiece(board[GetTo(move)]);
  return kMvvLva[static_cast<size_t>(fromPiece)][static_cast<size_t>(toPiece)];
}

Score ScoreMove(const Board& board, Move ttMove, Move move) {
  return move == ttMove ? kInfinity : MvvLva(board, move);
}

} // namespace

MovePicker::MovePicker(Moves& moves, const Board& board, Move ttMove)
  : moves_{moves},
    board_{board},
    ttMove_{ttMove},
    next_{0} {
}

size_t MovePicker::Next() {
  auto best = next_;
  for (auto i = next_ + 1; i < moves_.size(); ++i) {
    if (ScoreMove(board_, ttMove_, moves_[i]) > ScoreMove(board_, ttMove_, moves_[best])) {
      best = i;
    }
  }
  std::swap(moves_[next_], moves_[best]);
  return next_++;
}

} // namespace chess