#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include "decoder.h"

void Decoder::decode(const std::string &file_to_decode_, const std::string &decoded_file_,
    const std::unordered_map<char, std::string> &huffman_table, const std::vector<uint64_t> &block_offsets)
{
    // Start up the thread pool for decoding task submission.
    Concurrent::ThreadPool thread_pool;

    // Read the file into memory.
    std::ifstream file(file_to_decode_);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_text = buffer.str();

    // Create a hashmap that maps codes to symbols.
    std::unordered_map<std::string, char> decoding_table = getDecodingTable(huffman_table);

    // Decode the file.
    decodeFile(thread_pool, decoding_table, block_offsets, file_text, decoded_file_);
}

std::string Decoder::decodeBlock(
    const std::unordered_map<std::string, char> &decoding_table, std::string::const_iterator start, std::string::const_iterator end)
{
    std::string decoded_block;
    std::string current;
    while (start != end)
    {
        // Add characters to current until it matches a key in the decoding table.
        current += *start;
        if (decoding_table.find(current) != decoding_table.end())
        {
            decoded_block += decoding_table.at(current);
            current = "";
        }
        ++start;
    }
    return decoded_block;
}

std::unordered_map<std::string, char> Decoder::getDecodingTable(const std::unordered_map<char, std::string> &huffman_table)
{
    std::unordered_map<std::string, char> decoding_table;
    for (const auto &[symbol, code] : huffman_table)
        decoding_table[code] = symbol;
    return decoding_table;
}

void Decoder::decodeFile(Concurrent::ThreadPool &pool, const std::unordered_map<std::string, char> &decoding_table,
    const std::vector<uint64_t> &block_offsets, const std::string &file_text, const std::string &decoded_file)
{
    // The entirety of the encoded text, decoded.
    std::string decoded_text;

    // Get the number of blocks to use.
    uint32_t num_blocks = block_offsets.size();
    auto block_start = file_text.begin();

    std::vector<std::future<std::string>> futures(num_blocks);

    // Submit each block of the encoded string to the thread pool for decoding.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        auto block_end = block_start;
        std::advance(block_end, block_offsets[i]);
        futures[i] = pool.submitTask(
            [&table = std::as_const(decoding_table), start = block_start, end = block_end] { return decodeBlock(table, start, end); });
        block_start = block_end;
    }
    std::string last_block = decodeBlock(decoding_table, block_start, file_text.end());

    // Combine all the decoded blocks into a single string.
    for (uint32_t i = 0; i < num_blocks; ++i)
        decoded_text += futures[i].get();
    decoded_text += last_block;

    // Write the decoded string to the specified file.
    std::ofstream file(decoded_file);
    file << decoded_text;
    file.close();
}
