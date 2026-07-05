#include "chess/core/square.h"

namespace chess {

bool operator==(const Square& square1, const Square& square2) {
  return square1.rank == square2.rank && square1.file == square2.file;
}

} // namespace chess