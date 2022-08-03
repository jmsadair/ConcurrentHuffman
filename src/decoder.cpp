#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include "decoder.h"

void Decoder::decode(const std::string &file_to_decode_, const std::string &decoded_file_)
{
    // Start up the thread pool for decoding task submission.
    Concurrent::ThreadPool thread_pool;

    // The block sizes and huffman table that will be read from the encoded file.
    std::unordered_map<std::string, char> decoding_table;
    std::vector<uint64_t> block_offsets;

    std::ifstream file(file_to_decode_);
    std::string text;

    // Get the number of symbols from the first line of the file.
    std::getline(file, text, '\n');
    uint num_symbols  = std::stoi(text);

    // Get the symbol and code from each line.
    while (num_symbols-- != 0)
    {
        std::getline(file, text, '\n');
        std::string code = text.substr(0, text.find(':'));
        char symbol = static_cast<char>(std::stoi(text.substr(text.find(':') + 1, text.size())));
        decoding_table.insert({code, symbol});
    }

    // Get the number of blocks from the current line.
    std::getline(file, text);
    uint32_t num_blocks = std::stoi(text);
    block_offsets.reserve(num_blocks);

    // Get each block size.
    while (num_blocks-- != 0)
    {
        std::getline(file, text);
        block_offsets.push_back(std::stoi(text));
    }

    // Read the rest of the file into memory.
    std::stringstream  buffer;
    buffer << file.rdbuf();
    text = buffer.str();
    file.close();

    // Decode the file.
    decodeFile(thread_pool, decoding_table, block_offsets, text, decoded_file_);
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
    std::ofstream file(decoded_file, std::ios::binary);
    file << decoded_text;
    file.close();
}
