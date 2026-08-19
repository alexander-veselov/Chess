#include "chess/engine/transposition_table.h"

namespace chess {

TranspositionTable::TranspositionTable() {
  for (auto& entry : table_) {
    entry = TTEntry{};
    entry.hash = Hash{0};
    entry.depth = 0;
    entry.score = Score{0};
    entry.type = TTEntryType::kExact;
    entry.move = kInvalidMove;
  }
}

bool TranspositionTable::GetEntry(Hash hash, TTEntry& entry) const {
  const auto index = hash % kSize;
  entry = table_[index];
  return entry.hash == hash;
}

bool TranspositionTable::ProbeHash(Hash hash, U32 ply, U32 depth, Score alpha, Score beta,
                                   TTEntry& entry) const {
  const auto index = hash % kSize;
  entry = TTEntry{table_[index]};
  if (entry.hash == hash) {
    if (entry.depth >= depth) {
      auto& score = entry.score;
      if (score > kMaxScore) {
        score -= static_cast<Score>(ply);
      } else if (score < -kMaxScore) {
        score += static_cast<Score>(ply);
      }
      if (entry.type == TTEntryType::kExact ||
          entry.type == TTEntryType::kLowerBound && score >= beta ||
          entry.type == TTEntryType::kUpperBound && score <= alpha) {
        return true;
      }
    }
  }
  return false;
}

void TranspositionTable::RecordHash(Hash hash, U32 ply, U32 depth, Score score, TTEntryType type,
                                    Move move) {
  if (score > kMaxScore) {
    score += static_cast<Score>(ply);
  } else if (score < -kMaxScore) {
    score -= static_cast<Score>(ply);
  }
  const auto index = hash % kSize;
  auto& entry = table_[index];
  if (entry.hash == 0ULL || entry.depth <= depth) {
    entry = TTEntry{};
    entry.hash = hash;
    entry.depth = depth;
    entry.score = score;
    entry.type = type;
    entry.move = move;
  }
}

} // namespace chess