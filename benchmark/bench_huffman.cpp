#include <benchmark/benchmark.h>
#include <filesystem>
#include "concurrent_huffman.h"

static void BM_Compression(benchmark::State &state)
{
    std::string uncompressed_file = "bench_uncompressed.txt";
    std::string compressed_file = "compressed.txt";
    for (auto _ : state)
    {
        ConcurrentHuffman::compressFile(uncompressed_file, compressed_file);
    }
    std::filesystem::remove(compressed_file);
}

static void BM_Decompression(benchmark::State &state)
{
    std::string compressed_file = "bench_compressed.txt";
    std::string uncompressed_file = "uncompressed.txt";
    for (auto _ : state)
    {
        ConcurrentHuffman::decompressFile(compressed_file, uncompressed_file);
    }
    std::filesystem::remove(uncompressed_file);
}

BENCHMARK(BM_Compression)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Decompression)->Unit(benchmark::kMillisecond);
BENCHMARK_MAIN();
