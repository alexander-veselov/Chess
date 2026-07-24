#include <benchmark/benchmark.h>

#include "chess/core/fen.h"
#include "chess/core/perft.h"
#include "chess/core/types.h"

#include <vector>
#include <string_view>

// History
// 
// Position          Depth    Nodes/s            Nodes/s            Nodes/s
//                            Initial version    IsInCheck rework   Better Moves class
// 
// PerftPosition1    1        1.03885 M/s        12.0724 M/s        26.2985M/s
// PerftPosition1    2        909.241 k/s        11.3329 M/s        16.6214M/s
// PerftPosition1    3        949.547 k/s        11.6764 M/s        23.8966M/s
// PerftPosition1    4        870.758 k/s        11.3634 M/s        16.8346M/s
//                                                                  
// PerftPosition2    1        665.507 k/s        6.57553 M/s        18.9046M/s
// PerftPosition2    2        571.095 k/s        5.11010 M/s        12.6535M/s
// PerftPosition2    3        640.551 k/s        6.59281 M/s        18.1701M/s
// PerftPosition2    4        582.358 k/s        5.94270 M/s        13.0739M/s
//                                                                  
// PerftPosition3    1        1.51189 M/s        11.9467 M/s        19.8226M/s
// PerftPosition3    2        1.30863 M/s        8.49042 M/s        16.6454M/s
// PerftPosition3    3        1.31913 M/s        10.6586 M/s        17.6607M/s
// PerftPosition3    4        1.39916 M/s        8.20842 M/s        14.9791M/s
//                                                                  
// PerftPosition4    1        193.294 k/s        2.52988 M/s        7.2404M/s
// PerftPosition4    2        580.096 k/s        5.73666 M/s        12.6579M/s
// PerftPosition4    3        619.658 k/s        9.32135 M/s        25.25M/s
// PerftPosition4    4        614.303 k/s        6.16973 M/s        12.9396M/s
//                                                                  
// PerftPosition5    1        787.499 k/s        9.77582 M/s        24.449M/s
// PerftPosition5    2        605.207 k/s        11.2721 M/s        17.1528M/s
// PerftPosition5    3        737.699 k/s        8.94816 M/s        19.3715M/s
// PerftPosition5    4        598.325 k/s        10.3556 M/s        16.4175M/s
//                                                                  
// PerftPosition6    1        644.306 k/s        13.1760 M/s        31.0332M/s
// PerftPosition6    2        622.457 k/s        12.0416 M/s        18.9235M/s
// PerftPosition6    3        653.745 k/s        12.0749 M/s        27.6013M/s
// PerftPosition6    4        640.756 k/s        11.3297 M/s        18.5615M/s

static void Perft(benchmark::State& state, std::string_view position, U32 depth) {
  const auto gameState = chess::StateFromFEN(position.data());
  auto nodes = U64{};
  for (auto _ : state) {
    nodes = chess::Perft(gameState, depth);
    benchmark::DoNotOptimize(nodes);
  }
  state.counters["Nodes/s"] =
      benchmark::Counter(static_cast<double>(nodes), benchmark::Counter::kIsIterationInvariantRate);
}

static void PerftPosition1(benchmark::State& state) {
  constexpr auto kPosition1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Perft(state, kPosition1, static_cast<U32>(state.range(0)));
}
BENCHMARK(PerftPosition1)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);

static void PerftPosition2(benchmark::State& state) {
  constexpr auto kPosition2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
  Perft(state, kPosition2, static_cast<U32>(state.range(0)));
}
BENCHMARK(PerftPosition2)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);

static void PerftPosition3(benchmark::State& state) {
  constexpr auto kPosition3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ";
  Perft(state, kPosition3, static_cast<U32>(state.range(0)));
}
BENCHMARK(PerftPosition3)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);

static void PerftPosition4(benchmark::State& state) {
  constexpr auto kPosition4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  Perft(state, kPosition4, static_cast<U32>(state.range(0)));
}
BENCHMARK(PerftPosition4)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);

static void PerftPosition5(benchmark::State& state) {
  constexpr auto kPosition5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ";
  Perft(state, kPosition5, static_cast<U32>(state.range(0)));
}
BENCHMARK(PerftPosition5)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);

static void PerftPosition6(benchmark::State& state) {
  constexpr auto kPosition6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ";
  Perft(state, kPosition6, static_cast<U32>(state.range(0)));
}
BENCHMARK(PerftPosition6)->DenseRange(1, 4)->MinTime(1.0)->Unit(benchmark::kMillisecond);