#include "chess/engine/pv_table.h"

namespace chess {

std::vector<Move> PVTable::GetLine(U32 depth) const {
  auto line = std::vector<Move>{};
  line.reserve(depth);
  for (auto i = 0; i < depth; ++i) {
    if (table_[i] != kInvalidMove) {
      line.push_back(table_[i]);
    } else {
      break;
    }
  }
  return line;
}

void PVTable::Update(U32 ply, U32 depth, Move move) {
  auto it = At(ply, ply + depth);
  *it = move;
  std::copy_n(it + depth, depth - 1, it + 1);
}

PVTable::TableType::iterator PVTable::At(U32 ply, U32 maxDepth) {
  return table_.begin() + static_cast<size_t>(ply * (2 * maxDepth + 1 - ply) / 2);
}

} // namespace chess