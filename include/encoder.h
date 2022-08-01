#ifndef CONCURRENT_HUFFMAN_ENCODER_H
#define CONCURRENT_HUFFMAN_ENCODER_H
#include <string>
#include <unordered_map>
#include <filesystem>
#include "node.h"
#include "thread_pool.h"

class Encoder
{
public:
    static std::pair<std::unordered_map<char, std::string>, std::vector<uint64_t>> encode(
        const std::string &file_to_encode_, const std::string &encoded_file_);

private:
    static std::unordered_map<char, std::string> constructHuffmanTable(std::unique_ptr<Node> huffman_tree_root);
    static std::unique_ptr<Node> constructHuffmanTree(const std::unordered_map<char, uint64_t> &character_frequencies);
    static std::unordered_map<char, uint64_t> countCharacterFrequencies(Concurrent::ThreadPool &pool, const std::string &file_text);
    static std::unordered_map<char, uint64_t> countBlock(std::string::const_iterator start, std::string::const_iterator end);
    static std::vector<uint64_t> encodeFile(Concurrent::ThreadPool &pool, const std::unordered_map<char, std::string> &huffman_table,
        const std::string &file_text, const std::string &encoded_file);
    static std::string encodeBlock(
        const std::unordered_map<char, std::string> &huffman_table, std::string::const_iterator start, std::string::const_iterator end);
};
#endif // CONCURRENT_HUFFMAN_ENCODER_H
