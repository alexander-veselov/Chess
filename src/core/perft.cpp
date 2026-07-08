#pragma once

#include "chess/core/perft.h"
#include "chess/core/game.h"
#include "chess/core/move.h"

#include <iostream>

namespace chess {

uint64_t Perft(const State& state, int32_t depth) {
  auto moves = std::vector<Move>{};
  GetAllLegalMoves(state, moves);

  if (depth == 1) {
    return moves.size();
  }

  auto nodes = uint64_t{0};
  for (const auto& move : moves) {
    auto newState = State{state};
    MakeMove(newState, move);
    nodes += Perft(newState, depth - 1);
  }

  return nodes;
}

std::map<std::string, uint64_t> Divide(const State& state, int32_t depth) {
  auto result = std::map<std::string, uint64_t>{};

  auto moves = std::vector<Move>{};
  GetAllLegalMoves(state, moves);

  if (depth == 1) {
    for (const auto& move : moves) {
      result[MoveToString(move)] = 1;
    }
  } else {
    auto nodes = uint64_t{0};
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