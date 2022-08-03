#ifndef CONCURRENT_HUFFMAN_CONCURRENT_HUFFMAN_H
#define CONCURRENT_HUFFMAN_CONCURRENT_HUFFMAN_H
#include <vector>
#include <string>
#include <unordered_map>

class ConcurrentHuffman
{
public:
    static void encodeFile(const std::string &file_to_encode, const std::string &encoded_file);
    static void decodeFile(const std::string &file_to_decode, const std::string &decoded_file);
};
#endif // CONCURRENT_HUFFMAN_CONCURRENT_HUFFMAN_H
