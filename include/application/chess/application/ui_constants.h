#pragma once

#include "chess/core/constants.h"
#include "chess/core/color.h"

namespace chess {

constexpr auto kCellSize = 200.f;
constexpr auto kPadding = 18.f;
constexpr auto kWindowWidth = (kBoardSize + 6) * kCellSize + 2 * kPadding;
constexpr auto kWindowHeight = kBoardSize * kCellSize + 2 * kPadding;
constexpr auto kPerspectiveColor = Color::kWhite;

}