#pragma once

#include "chess/core/move.h"
#include "chess/core/state.h"
#include "chess/core/types.h"
#include "chess/core/zobrist_hash.h"
#include "chess/engine/score.h"

#include <array>

namespace chess {

enum class TTEntryType {
  kExact,
  kLowerBound,
  kUpperBound
};

struct TTEntry {
  U64 hash;
  U32 depth;
  Score score;
  TTEntryType type;
  Move move;
};

class TranspositionTable {
public:
  TranspositionTable();
  bool GetEntry(Hash hash, TTEntry& entry) const;
  bool ProbeHash(Hash hash, U32 ply, U32 depth, Score alpha, Score beta, TTEntry& entry) const;
  void RecordHash(Hash hash, U32 ply, U32 depth, Score score, TTEntryType type, Move move);

private:
  constexpr static auto kSize = 1ULL << 25; 
  std::array<TTEntry, kSize> table_;
};

}