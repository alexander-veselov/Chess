#include <gtest/gtest.h>

#include "chess/core/fen.h"
#include "chess/core/game.h"
#include "chess/core/state.h"
#include "chess/engine/engine.h"

TEST(Chess, EngineMateIn2_1) {
  constexpr auto kDepth = 5;
  auto state = chess::StateFromFEN("8/1k6/7R/6R1/8/4K3/8/8 w - - 0 1");
  auto status = chess::Status{};
  for (auto i = 0; i < kDepth; ++i) {
    status = chess::GetStatus(state);
    if (IsGameOver(status)) {
      break;
    }
    const auto move = chess::BestMove(state, kDepth);
    chess::MakeMove(state, move);
  }
  EXPECT_EQ(status, chess::Status::kWhiteWon);
}

TEST(Chess, EngineMateIn2_2) {
  constexpr auto kDepth = 4;
  auto state = chess::StateFromFEN("r4br1/3b1kpp/1q1P4/1pp1RP1N/p7/6Q1/PPB3PP/2KR4 w - - 0 1");
  auto status = chess::Status{};
  for (auto i = 0; i < kDepth; ++i) {
    status = chess::GetStatus(state);
    if (IsGameOver(status)) {
      break;
    }
    const auto move = chess::BestMove(state, kDepth);
    chess::MakeMove(state, move);
  }
  EXPECT_EQ(status, chess::Status::kWhiteWon);
}