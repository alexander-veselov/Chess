#include "chess/core/color.h"

namespace chess {

std::string ColorToString(Color color) {
  return color == Color::kWhite ? "w" : "b";
}

bool ParseColor(const std::string_view& string, Color& color) {
  if (string.size() != 1) {
    return false;
  }

  switch (string[0]) {
  case 'w':
    color = Color::kWhite;
    return true;
  case 'b':
    color = Color::kBlack;
    return true;
  }

  return false;
}

} // namespace chess