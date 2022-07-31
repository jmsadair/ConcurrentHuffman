#ifndef CONCURRENT_HUFFMAN_DECODER_H
#define CONCURRENT_HUFFMAN_DECODER_H
#include <string>
#include <unordered_map>
#include <vector>
#include "thread_pool.h"

class Decoder
{
public:
    void decode(const std::string &file_to_decode_, const std::string &decoded_file_,
        const std::unordered_map<char, std::string> &huffman_table, const std::vector<uint64_t> &block_offsets);

private:
    void decodeFile(const std::unordered_map<std::string, char> &decoding_table, const std::vector<uint64_t> &block_offsets,
        const std::string &file_text, const std::string &decoded_file);
    static std::string decodeBlock(
        const std::unordered_map<std::string, char> &decoding_table, std::string::const_iterator start, std::string::const_iterator end);
    static std::unordered_map<std::string, char> getDecodingTable(const std::unordered_map<char, std::string> &huffman_table);

    Concurrent::ThreadPool thread_pool;
};
#endif // CONCURRENT_HUFFMAN_DECODER_H
