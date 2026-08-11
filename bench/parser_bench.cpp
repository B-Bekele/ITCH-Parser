#include <benchmark/benchmark.h>

#include "parser.hpp"

static void BM_ParseSampleFixture(benchmark::State& state) {
    for (auto _ : state) {
        Parser parser;
        ParseResult result = parser.parse_file("data/sample.itch");
        benchmark::DoNotOptimize(result);
    }

    Parser parser;
    ParseResult result = parser.parse_file("data/sample.itch");
    state.SetBytesProcessed(static_cast<int64_t>(result.total_bytes) * state.iterations());
    state.counters["msg/sec"] =
        benchmark::Counter(static_cast<double>(result.total_messages) * state.iterations(),
                           benchmark::Counter::kIsRate);
}

BENCHMARK(BM_ParseSampleFixture);
