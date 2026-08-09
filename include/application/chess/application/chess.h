#pragma once

#include "chess/core/status.h"
#include "chess/core/state.h"
#include "chess/core/square.h"
#include "chess/core/moves.h"
#include "chess/core/move.h"

#include <string_view>

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
  Moves GetLegalMoves(Square square) const;

private:
  State state_;
};

} // namespace chess