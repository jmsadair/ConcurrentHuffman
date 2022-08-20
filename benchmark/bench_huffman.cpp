#include <benchmark/benchmark.h>
#include <filesystem>
#include "concurrent_huffman.h"

static void BM_Compression(benchmark::State &state)
{
    std::string uncompressed_file = "bench_uncompressed.txt";
    std::string compressed_file = "compressed.txt";
    const uint32_t num_threads = state.range(0);
    for (auto _ : state)
    {
        ConcurrentHuffman::compressFile(uncompressed_file, compressed_file, num_threads);
    }
    std::filesystem::remove(compressed_file);
}

static void BM_Decompression(benchmark::State &state)
{
    std::string compressed_file = "bench_compressed.txt";
    std::string uncompressed_file = "uncompressed.txt";
    const uint32_t num_threads = state.range(0);
    for (auto _ : state)
    {
        ConcurrentHuffman::decompressFile(compressed_file, uncompressed_file, num_threads);
    }
    std::filesystem::remove(uncompressed_file);
}

BENCHMARK(BM_Compression)->Unit(benchmark::kMillisecond)->ArgNames({"Number of threads"})->Args({1})->Args({5})->Args({10});
BENCHMARK(BM_Decompression)->Unit(benchmark::kMillisecond)->ArgNames({"Number of threads"})->Args({1})->Args({5})->Args({10});
BENCHMARK_MAIN();
