#ifndef CONCURRENT_HUFFMAN_CONCURRENT_HUFFMAN_H
#define CONCURRENT_HUFFMAN_CONCURRENT_HUFFMAN_H
#include <vector>
#include <string>
#include <unordered_map>

class ConcurrentHuffman
{
public:
    void encodeFile(const std::string &file_to_encode, const std::string &encoded_file);
    void decodeFile(const std::string &file_to_decode, const std::string &decoded_file);

private:
    std::unordered_map<char, std::string> encoding_table;
    std::unordered_map<std::string, char> decoding_table;
    std::vector<uint64_t> decoding_block_offsets;
};
#endif // CONCURRENT_HUFFMAN_CONCURRENT_HUFFMAN_H
