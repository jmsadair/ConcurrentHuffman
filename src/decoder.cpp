#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <bitset>
#include "decoder.h"

void Decoder::decompressFile(const std::string &file_to_decompress, const std::string &decompressed_file)
{
    // Start up the thread pool for decoding task submission.
    Concurrent::ThreadPool thread_pool(num_threads);

    // Try to open the encoded file.
    std::ifstream input_stream;
    input_stream.exceptions(std::ifstream::failbit);
    try
    {
        input_stream.open(file_to_decompress, std::ios::binary);
    }
    catch (const std::exception &e)
    {
        std::ostringstream msg;
        msg << "Opening file '" << decompressed_file << "' failed, it either doesn't exist or is not accessible.";
        throw std::runtime_error(msg.str());
    }

    // Get decoding table, block offsets, and padding from input_stream header.
    const HeaderData header_data = getHeaderData(input_stream);

    // Read the rest of the input_stream into memory.
    std::stringstream buffer;
    buffer << input_stream.rdbuf();
    const std::string text = buffer.str();
    input_stream.close();

    // Get the encoded text as a bit string and remove any padding zeros.
    std::string bit_string = toBitString(thread_pool, text);
    bit_string.erase(bit_string.length() - header_data.padding);

    // Decode the encoded text from the file.
    const std::string decoded_text = decodeBitString(thread_pool, header_data, bit_string);

    // Write the decoded string to the specified input_stream.
    std::ofstream output_file(decompressed_file);
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
    const uint32_t num_blocks = header_data.block_offsets.size();
    auto block_start = bit_string.begin();

    std::vector<std::future<std::string>> futures(num_blocks);

    // Submit each block of the encoded string to the thread pool for decoding.
    for (auto i = 0; i < num_blocks; ++i)
    {
        auto block_end = block_start;
        std::advance(block_end, header_data.block_offsets[i]);
        futures[i] = pool.submitTask([&table = std::as_const(header_data.decoding_table), start = block_start, end = block_end] {
            return decodeBitString(table, start, end);
        });
        block_start = block_end;
    }
    const auto block_end = bit_string.end();
    const std::string last_block = decodeBitString(header_data.decoding_table, block_start, block_end);

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

    // Construct decoding table.
    std::getline(input_file, header);
    std::stringstream table_stream(header);
    std::string code;
    std::string symbol;
    while (table_stream >> code && table_stream >> symbol)
        decoding_table.insert({code, static_cast<char>(std::stoi(symbol))});

    // Get padding amount.
    std::getline(input_file, header);
    const uint8_t padding = std::stoi(header);

    // Get block offsets.
    std::getline(input_file, header);
    std::stringstream offset_stream(header);
    std::string offset;
    while (offset_stream >> offset)
        block_offsets.push_back(std::stoi(offset));

    return {decoding_table, block_offsets, padding};
}

std::string Decoder::toBitString(Concurrent::ThreadPool &pool, const std::string &encoded_text)
{
    // The entirety of the file encoded as a bit string.
    std::string bit_string;

    const uint32_t block_size = 500;
    const uint32_t num_blocks = encoded_text.length() / block_size;
    auto block_start = encoded_text.begin();
    std::vector<std::future<std::string>> futures(num_blocks);

    // Submit blocks to thread pool for conversion to bit string.
    for (auto i = 0; i < num_blocks; ++i)
    {
        auto block_end = block_start;
        std::advance(block_end, block_size);
        futures[i] = pool.submitTask([start = block_start, end = block_end] { return toBitString(start, end); });
        block_start = block_end;
    }
    const auto block_end = encoded_text.end();
    const std::string last_encoded_block = toBitString(block_start, block_end);

    // Combine the text that each thread encoded into a single string.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        const std::string encoded_block = futures[i].get();
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
        const std::bitset<8> bits(*start);
        bit_string += bits.to_string();
        ++start;
    }
    return bit_string;
}
