#include "chess/engine/pv_table.h"

namespace chess {

std::span<const Move> PVTable::GetLine(U32 depth) const {
  return {table_.data(), depth};
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