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
    /**
     * Compresses the provided file. Creates a new file and does not modify the original file.
     *
     * @param file_to_compress the name of the file that will be compressed, require that the file
     *                         exists and is not already compressed.
     * @param compressed_file the name of the compressed file that will be created.
     */
    static void compressFile(const std::string &file_to_compress, const std::string &compressed_file);

private:
    /**
     * Creates a hash table that maps symbols to their code (a bit string).
     *
     * @param huffman_tree_root the root of the huffman tree, require that the huffman_tree_root is not a null pointer.
     * @return a hash table that maps symbols to their code.
     */
    static std::unordered_map<char, std::string> constructHuffmanTable(std::unique_ptr<Node> huffman_tree_root);

    /**
     * Constructs a new Huffman tree.
     *
     * @param character_frequencies a hashmap that maps characters to their frequency in the unencoded text.
     * @return
     */
    static std::unique_ptr<Node> constructHuffmanTree(const std::unordered_map<char, uint64_t> &character_frequencies);

    /**
     * Given a string of unencoded text, counts the number of times each character occurs in the text.
     *
     * @param pool the thread pool that will be used for task submission, require that the thread pool has already been started.
     * @param unencoded_text the unencoded text that characters will be counted from.
     * @return a hashmap that maps a character to the number of times it occurred in the provided text.
     */
    static std::unordered_map<char, uint64_t> countCharacterFrequencies(Concurrent::ThreadPool &pool, const std::string &unencoded_text);

    /**
     * Counts the number of times each character occurs in unencoded text.
     *
     * @param start an iterator to a string of unencoded text, characters will be counted starting from this position.
     * @param end an iterator to a string of unencoded text, characters will be not be counted from this position onwards.
     * @return a hashmap that maps characters to the number of times that they appeared in the unencoded text.
     */
    static std::unordered_map<char, uint64_t> countCharacterFrequencies(std::string::const_iterator start, std::string::const_iterator end);

    /**
     * Converts a string of unencoded text to a string of ones and zeros that is the encoded text.
     *
     * @param pool the thread pool that will be used for task submission, require that the thread pool has already been started.
     * @param encoding_table a hashmap that maps symbols to their respective code value.
     * @param unencoded_text the unencoded text that the bit string will be created from.
     * @return a string of ones and zeros that is the encoded text.
     */
    static std::pair<std::string, std::vector<uint32_t>> toBitString(
        Concurrent::ThreadPool &pool, const std::unordered_map<char, std::string> &encoding_table, const std::string &unencoded_text);

    /**
     * Converts a string of unencoded text to a string of ones and zeros that is the encoded text.
     *
     * @param encoding_table a hashmap that maps symbols to their respective code value.
     * @param start an iterator to a string of unencoded text, characters will be converted to their code starting from this position.
     * @param end an iterator to a string of unencoded text, characters will not be converted to their code from this position onwards.
     * @return a string of ones and zeros that is the encoded text.
     */
    static std::string toBitString(
        const std::unordered_map<char, std::string> &encoding_table, std::string::const_iterator start, std::string::const_iterator end);

    /**
     * Given a bit string, appends the minimum number of zeros to the string such that it has
     * a size that is divisible by eight.
     *
     * @param bit_string a string of ones and zeros that will be padded with zeros (if necessary).
     * @return the number of zeros that the bit string was padded with.
     */
    static uint8_t padBitString(std::string &bit_string);

    /**
     * Converts a bit string to bytes.
     *
     * @param pool the thread pool that will be used for task submission, require that the thread pool has already been started.
     * @param bit_string the bit string that the bytes will be created from, require that bit_string has a size that is divisible
     *                   by eight.
     * @return a vector containing the bytes created from the bit string.
     */
    static std::vector<unsigned char> toBytes(Concurrent::ThreadPool &pool, const std::string &bit_string);

    /**
     * Convert a bit string to bytes, require that the distance between start and end is divisible by eight.
     *
     * @param start an iterator to a bit string, the bit string will be converted to bytes starting from this position.
     * @param end an iterator to a bit string, the bit string will not be converted to bytes from this position onward.
     * @return a vector containing the bytes created from the bit string.
     */
    static std::vector<unsigned char> toBytes(std::string::const_iterator start, std::string::const_iterator end);

    // The size of the string that that will be submitted to the thread pool for character counting.
    // Note that using small numbers will result in poor performance.
    static constexpr uint32_t count_character_block_size = 1000;
    // The size of the bit string that will be submitted to the thread pool for conversion to bytes.
    // Must be divisible by eight since the string is being converted to bytes.
    // As before, using small numbers will result in poor performance.
    static constexpr uint32_t to_bytes_block_size = 800;
    // The number of threads that the thread pool will use.
    inline static uint32_t num_threads = 11;
};
#endif // CONCURRENT_HUFFMAN_ENCODER_H
