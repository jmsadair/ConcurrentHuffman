#include "encoder.h"

Encoder::Encoder(std::string encode_file_)
    : encode_file(std::move(encode_file_))
{}

void Encoder::countCharacterFrequenciesConcurrent()
{

    // Read the file into memory.
    std::ifstream file(encode_file);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_string = buffer.str();

    // Blocks that will be submitted to thread pool for counting.
    uint32_t block_size = 1000;
    uint32_t num_blocks = file_string.length() / block_size;
    auto block_start = file_string.begin();

    // A vector to hold the futures returned by the thread pool.
    std::vector<std::future<std::unordered_map<char, uint64_t>>> futures(num_blocks);

    // Submit blocks to thread pool for counting.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        auto block_end = block_start;
        std::advance(block_end, block_size);
        futures[i] = thread_pool.submitTask([=] { return countCharacterFrequencies(block_start, block_end); });
        block_start = block_end;
    }

    // Count any remaining characters in the file string.
    frequencies = std::move(countCharacterFrequencies(block_start, file_string.end()));

    // Sum up all the counts from the blocks.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        std::unordered_map<char, uint64_t> block_counts = futures[i].get();
        for (const auto [character, count] : block_counts)
            frequencies[character] += count;
    }
}

void Encoder::countCharacterFrequencies()
{
    // Read the file into memory.
    std::ifstream file(encode_file);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_string = buffer.str();
    frequencies = std::move(countCharacterFrequencies(file_string.begin(), file_string.end()));
}

std::unordered_map<char, uint64_t> Encoder::countCharacterFrequencies(std::string::iterator start, std::string::iterator end)
{
    std::unordered_map<char, uint64_t> character_counts;
    while (start != end)
    {
        ++character_counts[*start];
        ++start;
    }
    return character_counts;
}
