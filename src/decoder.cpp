#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <bitset>
#include "decoder.h"

void Decoder::decode(const std::string &file_to_decode_, const std::string &decoded_file_)
{
    // Start up the thread pool for decoding task submission.
    Concurrent::ThreadPool thread_pool;

    std::ifstream input_file(file_to_decode_, std::ios::binary);

    // Get decoding table, block offsets, and padding from input_file header.
    HeaderData header_data = getHeaderData(input_file);

    // Read the rest of the input_file into memory.
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    std::string text = buffer.str();
    input_file.close();

    // Get the encoded text as a bit string and remove padding.
    std::string bit_string = toBitString(thread_pool, text);
    bit_string.erase(bit_string.length() - header_data.padding);

    // Decode the input_file.
    std::string decoded_text = decodeBitString(thread_pool, header_data, bit_string);

    // Write the decoded string to the specified input_file.
    std::ofstream output_file(decoded_file_);
    output_file << decoded_text;
    output_file.close();
}

std::string Decoder::decodeBitString(
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

std::string Decoder::decodeBitString(Concurrent::ThreadPool &pool, const HeaderData &header_data, const std::string &bit_string)
{
    // The entirety of the encoded text, decoded.
    std::string decoded_text;

    // Get the number of blocks to use.
    uint32_t num_blocks = header_data.block_offsets.size();
    auto block_start = bit_string.begin();

    std::vector<std::future<std::string>> futures(num_blocks);

    // Submit each block of the encoded string to the thread pool for decoding.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        auto block_end = block_start;
        std::advance(block_end, header_data.block_offsets[i]);
        futures[i] = pool.submitTask([&table = std::as_const(header_data.decoding_table), start = block_start, end = block_end] {
            return decodeBitString(table, start, end);
        });
        block_start = block_end;
    }
    std::string last_block = decodeBitString(header_data.decoding_table, block_start, bit_string.end());

    // Combine all the decoded text into a single string.
    for (uint32_t i = 0; i < num_blocks; ++i)
        decoded_text += futures[i].get();
    decoded_text += last_block;

    return decoded_text;
}

HeaderData Decoder::getHeaderData(std::ifstream &input_file)
{
    std::unordered_map<std::string, char> decoding_table;
    std::vector<uint32_t> block_offsets;

    std::string header;
    std::vector<std::string> header_data;
    std::getline(input_file, header);

    char delimiter = ',';
    size_t delimiter_pos = 0;
    while ((delimiter_pos = header.find(delimiter)) != std::string::npos)
    {
        header_data.push_back(header.substr(0, delimiter_pos));
        header.erase(0, delimiter_pos + 1);
    }

    uint8_t num_symbols = std::stoi(header_data[0]);
    uint8_t padding = std::stoi(header_data[2 * num_symbols + 1]);
    for (int i = 1; i < 2 * num_symbols + 1; i += 2)
    {
        std::string code = header_data[i];
        char symbol = static_cast<char>(std::stoi(header_data[i + 1]));
        decoding_table.insert({code, symbol});
    }
    for (int i = 2 * num_symbols + 2; i < header_data.size(); ++i)
        block_offsets.push_back(std::stoi(header_data[i]));

    return {decoding_table, block_offsets, padding};
}

std::string Decoder::toBitString(Concurrent::ThreadPool &pool, const std::string &encoded_text)
{
    // The entirety of the file encoded as a bit string.
    std::string bit_string;

    uint32_t block_size = 500;
    uint32_t num_blocks = encoded_text.length() / block_size;
    auto block_start = encoded_text.begin();
    std::vector<std::future<std::string>> futures(num_blocks);

    // Submit blocks to thread pool for encoding.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        auto block_end = block_start;
        std::advance(block_end, block_size);
        futures[i] = pool.submitTask([start = block_start, end = block_end] { return toBitString(start, end); });
        block_start = block_end;
    }
    std::string last_encoded_block = toBitString(block_start, encoded_text.end());

    // Combine the text that each thread encoded into a single string.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        std::string encoded_block = futures[i].get();
        bit_string += encoded_block;
    }
    bit_string += last_encoded_block;

    return bit_string;
}

std::string Decoder::toBitString(std::string::const_iterator start, std::string::const_iterator end)
{
    std::string bit_string;
    while (start != end)
    {
        std::bitset<8> bits(*start);
        bit_string += bits.to_string();
        ++start;
    }
    return bit_string;
}
