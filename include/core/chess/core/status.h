#pragma once

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

} // namespace chess