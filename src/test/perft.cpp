#pragma once

#include "chess/test/perft.h"
#include "chess/test/utils.h"

#include <iostream>

namespace chess {
namespace test {

uint64_t Perft(const State& state, int32_t depth) {
  auto moves = std::vector<Move>{};
  const auto nMoves = GetAllLegalMoves(state, moves);

  if (depth == 1) {
    return nMoves;
  }

  auto nodes = uint64_t{0};
  for (const auto& move : moves) {
    auto newState = State{state};
    MakeMove(newState, move);
    nodes += Perft(newState, depth - 1);
  }

  return nodes;
}

std::map<std::string, uint64_t> PerftDebug(const State& state, int32_t depth) {
  auto result = std::map<std::string, uint64_t>{};

  auto moves = std::vector<Move>{};
  const auto nMoves = GetAllLegalMoves(state, moves);

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

} // namespace test
} // namespace chess