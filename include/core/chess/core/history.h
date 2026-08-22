#pragma once

#include "chess/core/fake_vector.h"
#include "chess/core/hash.h"

namespace chess {

using History = FakeVector<Hash, 128>;

} // namespace chess