#pragma once

#include "chess/core/perft.h"

#include "chess/core/game.h"
#include "chess/core/move.h"

namespace chess {
namespace {

U64 PerftInternal(State& state, I32 depth) {
  auto moves = Moves{};
  GetLegalMoves(state, moves);

  if (depth == 1) {
    return moves.size();
  }

  auto nodes = U64{0};
  for (const auto& move : moves) {
    auto undo = Undo{};
    MakeMove(state, move, undo);
    nodes += PerftInternal(state, depth - 1);
    UndoMove(state, move, undo);
  }

  return nodes;
}

U64 PerftFInternal(State& state, I32 depth, std::function<void(const State&, Move)> f) {
  auto moves = Moves{};
  GetLegalMoves(state, moves);

  if (depth == 1) {
    return moves.size();
  }

  auto nodes = U64{0};
  for (const auto& move : moves) {
    auto undo = Undo{};
    MakeMove(state, move, undo);
    f(state, move);
    nodes += PerftFInternal(state, depth - 1, f);
    UndoMove(state, move, undo);
  }

  return nodes;
}

} // namespace

U64 Perft(const State& state, I32 depth) {
  auto copy = State{state};
  return PerftInternal(copy, depth);
}

U64 PerftF(const State& state, I32 depth, std::function<void(const State&, Move)> f) {
  auto copy = State{state};
  return PerftFInternal(copy, depth, f);
}

std::map<std::string, U64> Divide(const State& state, I32 depth) {
  auto result = std::map<std::string, U64>{};

  auto moves = Moves{};
  GetLegalMoves(state, moves);

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