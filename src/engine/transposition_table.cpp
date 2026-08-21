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

TTEntry const* TranspositionTable::GetEntry(Hash hash) const {
  const auto index = hash & (kSize - 1);
  const auto& entry = table_[index];
  return entry.hash == hash ? &entry : nullptr;
}

std::optional<Score> TranspositionTable::Probe(TTEntry const* entry, U32 ply, U32 depth,
                                               Score alpha, Score beta) const {
  if (entry) {
    if (entry->depth >= depth) {
      auto score = entry->score;
      if (score > kMaxScore) {
        score -= static_cast<Score>(ply);
      } else if (score < -kMaxScore) {
        score += static_cast<Score>(ply);
      }
      if (entry->type == TTEntryType::kExact ||
          entry->type == TTEntryType::kLowerBound && score >= beta ||
          entry->type == TTEntryType::kUpperBound && score <= alpha) {
        return score;
      }
    }
  }
  return std::nullopt;
}

void TranspositionTable::Record(Hash hash, U32 ply, U32 depth, Score score, TTEntryType type,
                                Move move) {
  if (score > kMaxScore) {
    score += static_cast<Score>(ply);
  } else if (score < -kMaxScore) {
    score -= static_cast<Score>(ply);
  }
  const auto index = hash & (kSize - 1);
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