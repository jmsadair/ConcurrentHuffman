#ifndef CONCURRENT_HUFFMAN_DECODER_H
#define CONCURRENT_HUFFMAN_DECODER_H
#include <string>
#include <unordered_map>
#include <vector>
#include "thread_pool.h"

struct HeaderData
{
    std::unordered_map<std::string, char> decoding_table;
    std::vector<uint32_t> block_offsets;
    uint8_t padding;
};

class Decoder
{
public:
    static void decode(const std::string &file_to_decode_, const std::string &decoded_file_);

private:
    static std::string decodeBitString(Concurrent::ThreadPool &pool, const HeaderData &header_data, const std::string &bit_string);
    static std::string decodeBitString(
        const std::unordered_map<std::string, char> &decoding_table, std::string::const_iterator start, std::string::const_iterator end);
    static HeaderData getHeaderData(std::ifstream &input_file);
    static std::string toBitString(Concurrent::ThreadPool &pool, const std::string &encoded_text);
    static std::string toBitString(std::string::const_iterator start, std::string::const_iterator end);
};
#endif // CONCURRENT_HUFFMAN_DECODER_H
