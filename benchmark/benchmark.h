#include <benchmark/benchmark.h>

static void BM_example(benchmark::State &state) {
  for (auto _ : state) {
    int x = 42;
    benchmark::DoNotOptimize(x);
  }
}

BENCHMARK(BM_example);