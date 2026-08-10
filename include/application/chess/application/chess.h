#pragma once

#include "chess/core/move.h"
#include "chess/core/square.h"
#include "chess/core/state.h"
#include "chess/core/status.h"

#include <string_view>
#include <vector>

namespace chess {

class Chess {
public:
  Chess();
  Chess(std::string_view fen);
  Chess(const State& state);

  const State& GetState() const;
  const Status GetStatus() const;
  bool IsInCheck() const;
  bool MakeMove(Move move);
  std::vector<Move> GetLegalMoves(Square square) const;
  std::vector<Move> GetHistory() const;

private:
  State state_;
  std::vector<Move> history_;
};

} // namespace chess