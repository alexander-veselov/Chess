#pragma once

#include "chess/core/move.h"
#include "chess/core/types.h"

#include <array>
#include <span>
#include <vector>

namespace chess {

class PVTable {
private:
  constexpr static auto kMaxDepth = U32{64};
  constexpr static auto kTableSize = kMaxDepth * (kMaxDepth + 1) / 2;
  using TableType = std::array<Move, kTableSize>;
  TableType table_;

public:
  std::span<const Move> GetLine(U32 depth) const;
  void Update(U32 ply, U32 depth, Move move);

private:
  TableType::iterator At(U32 ply, U32 maxDepth);
};

}