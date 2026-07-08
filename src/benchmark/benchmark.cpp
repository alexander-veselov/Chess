#include <benchmark/benchmark.h>

#include "chess/core/fen.h"
#include "chess/core/perft.h"

#include <vector>
#include <string_view>

// History
// 
// Position          Depth    Nodes/s
// 
// PerftPosition1    1        1.03790 M/s
// PerftPosition1    2        944.071 k/s
// PerftPosition1    3        943.216 k/s
// PerftPosition1    4        912.722 k/s
// 
// PerftPosition2    1        687.453 k/s
// PerftPosition2    2        599.159 k/s
// PerftPosition2    3        666.294 k/s
// PerftPosition2    4        603.877 k/s
// 
// PerftPosition3    1        1.90061 M/s
// PerftPosition3    2        1.57366 M/s
// PerftPosition3    3        1.77199 M/s
// PerftPosition3    4        1.74599 M/s
// 
// PerftPosition4    1        195.491 k/s
// PerftPosition4    2        623.066 k/s
// PerftPosition4    3        626.781 k/s
// PerftPosition4    4        635.984 k/s
// 
// PerftPosition5    1        796.448 k/s
// PerftPosition5    2        637.945 k/s
// PerftPosition5    3        729.767 k/s
// PerftPosition5    4        620.383 k/s
// 
// PerftPosition6    1        634.092 k/s
// PerftPosition6    2        615.384 k/s
// PerftPosition6    3        618.598 k/s
// PerftPosition6    4        647.413 k/s

static void Perft(benchmark::State& state, std::string_view position, uint32_t depth) {
  const auto gameState = chess::StateFromFEN(position.data());
  auto nodes = uint64_t{};
  for (auto _ : state) {
    nodes = chess::Perft(gameState, depth);
    benchmark::DoNotOptimize(nodes);
  }
  state.counters["Nodes/s"] =
      benchmark::Counter(static_cast<double>(nodes), benchmark::Counter::kIsIterationInvariantRate);
}

static void PerftPosition1(benchmark::State& state) {
  constexpr auto kPosition1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Perft(state, kPosition1, static_cast<uint32_t>(state.range(0)));
}
BENCHMARK(PerftPosition1)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);

static void PerftPosition2(benchmark::State& state) {
  constexpr auto kPosition2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
  Perft(state, kPosition2, static_cast<uint32_t>(state.range(0)));
}
BENCHMARK(PerftPosition2)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);

static void PerftPosition3(benchmark::State& state) {
  constexpr auto kPosition3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ";
  Perft(state, kPosition3, static_cast<uint32_t>(state.range(0)));
}
BENCHMARK(PerftPosition3)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);

static void PerftPosition4(benchmark::State& state) {
  constexpr auto kPosition4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  Perft(state, kPosition4, static_cast<uint32_t>(state.range(0)));
}
BENCHMARK(PerftPosition4)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);

static void PerftPosition5(benchmark::State& state) {
  constexpr auto kPosition5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ";
  Perft(state, kPosition5, static_cast<uint32_t>(state.range(0)));
}
BENCHMARK(PerftPosition5)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);

static void PerftPosition6(benchmark::State& state) {
  constexpr auto kPosition6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ";
  Perft(state, kPosition6, static_cast<uint32_t>(state.range(0)));
}
BENCHMARK(PerftPosition6)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);