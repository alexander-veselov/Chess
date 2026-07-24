#pragma once

#include "chess/core/perft.h"
#include "chess/core/game.h"
#include "chess/core/move.h"

#include <iostream>

namespace chess {

U64 Perft(const State& state, I32 depth) {
  auto moves = Moves{};
  GetAllLegalMoves(state, moves);

  if (depth == 1) {
    return moves.size();
  }

  auto nodes = U64{0};
  for (const auto& move : moves) {
    auto newState = State{state};
    MakeMove(newState, move);
    nodes += Perft(newState, depth - 1);
  }

  return nodes;
}

std::map<std::string, U64> Divide(const State& state, I32 depth) {
  auto result = std::map<std::string, U64>{};

  auto moves = Moves{};
  GetAllLegalMoves(state, moves);

  if (depth == 1) {
    for (const auto& move : moves) {
      result[MoveToString(move)] = 1;
    }
  } else {
    auto nodes = U64{0};
    for (const auto& move : moves) {
      auto newState = State{state};
      MakeMove(newState, move);
      const auto newNodes = Perft(newState, depth - 1);
      nodes += newNodes;

      result[MoveToString(move)] = newNodes;
    }
  }

  return result;
}

} // namespace chess