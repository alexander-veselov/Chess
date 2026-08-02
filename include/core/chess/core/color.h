#pragma once

#include "chess/core/types.h"

#include <string>

namespace chess {

enum class Color : U8 {
  kWhite,
  kBlack
};

constexpr Color FlipColor(Color color) {
  return color == Color::kWhite ? Color::kBlack : Color::kWhite;
}

std::string ColorToString(Color color);
bool ParseColor(const std::string_view& string, Color& color);

}