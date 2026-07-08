#include <benchmark/benchmark.h>

#include <vector>

static void Test(benchmark::State& state) {
  for (auto _ : state) {
    auto v = std::vector<int>(state.range(0));
    benchmark::DoNotOptimize(v.data());
  }
}

BENCHMARK(Test)->Range(8, 64);
