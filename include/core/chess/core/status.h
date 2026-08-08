#pragma once

#include <string>

namespace chess {

enum class Status {
  kWhiteToMove,
  kBlackToMove,
  kWhiteWon,
  kBlackWon,
  kDraw
};

constexpr bool IsGameOver(Status status) {
  switch (status) {
  case Status::kWhiteToMove:
  case Status::kBlackToMove:
    return false;
  case Status::kWhiteWon:
  case Status::kBlackWon:
  case Status::kDraw:
    return true;
  }
  return true;
}

static std::string_view StatusToString(Status status) {
  switch (status) {
  case Status::kWhiteToMove:
    return "White to move";
  case Status::kBlackToMove:
    return "Black to move";
  case Status::kWhiteWon:
    return "White won";
  case Status::kBlackWon:
    return "Black won";
  case Status::kDraw:
    return "Draw";
  }
  return "";
}

} // namespace chess