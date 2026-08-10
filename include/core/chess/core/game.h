#pragma once

#include "chess/core/status.h"
#include "chess/core/state.h"
#include "chess/core/square.h"
#include "chess/core/moves.h"
#include "chess/core/move.h"

namespace chess {

Status GetStatus(const State& state);
bool IsInCheck(const State& state, Color turn);
void GetCaptures(const State& state, Moves& legalMoves);
void GetLegalMoves(const State& state, Moves& legalMoves);
void MakeMove(State& state, Move move);

} // namespace chess