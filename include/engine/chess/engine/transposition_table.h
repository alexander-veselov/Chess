#pragma once

#include "chess/core/hash.h"
#include "chess/core/move.h"
#include "chess/core/state.h"
#include "chess/core/types.h"
#include "chess/engine/score.h"

#include <array>
#include <optional>

namespace chess {

enum class TTEntryType {
  kExact,
  kLowerBound,
  kUpperBound
};

struct TTEntry {
  Hash hash;
  U32 depth;
  Score score;
  TTEntryType type;
  Move move;
};

class TranspositionTable {
public:
  TranspositionTable();
  TTEntry const* GetEntry(Hash hash) const;
  std::optional<Score> Probe(TTEntry const* entry, U32 ply, U32 depth, Score alpha, Score beta) const;
  void Record(Hash hash, U32 ply, U32 depth, Score score, TTEntryType type, Move move);

private:
  constexpr static auto kSize = 1ULL << 25;
  std::array<TTEntry, kSize> table_;
};

}