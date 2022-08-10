#ifndef CONCURRENT_HUFFMAN_DECODER_H
#define CONCURRENT_HUFFMAN_DECODER_H
#include <string>
#include <unordered_map>
#include <vector>
#include "thread_pool.h"

// Used to store the data needed for file decompression that
// is retrieved from the compressed file.
struct HeaderData
{
    std::unordered_map<std::string, char> decoding_table;
    std::vector<uint32_t> block_offsets;
    uint8_t padding;
};

class Decoder
{
public:
    /**
     * Decompresses a compressed file.
     *
     * @param file_to_decompress the name of the file that will be decompressed, require that the file
     *                           exists and is compressed.
     * @param decompressed_file the name of the decompressed file that will be created.
     */
    static void decompressFile(const std::string &file_to_decompress, const std::string &decompressed_file);

private:
    /**
     * Decodes a bit string from a compressed file.
     *
     * @param pool the thread pool that will be used for task submission, require that the thread pool has already been started.
     * @param header_data the decoding table, the block offsets, and the padding that has been added to bit string during the encoding
     *                    process.
     * @param bit_string the string of ones and zeros that will be decoded.
     * @return the string created from decoding the bit string.
     */
    static std::string decodeBitString(Concurrent::ThreadPool &pool, const HeaderData &header_data, const std::string &bit_string);

    /**
     * Decodes a bit string from a compressed file.
     *
     * @param decoding_table a hashmap that maps codes to their respective symbol.
     * @param start an iterator to a bit string, decoding will start from this position.
     * @param end an iterator to a bit string, decoding will stop at this position.
     * @return a string created from decoding the bit string.
     */
    static std::string decodeBitString(
        const std::unordered_map<std::string, char> &decoding_table, std::string::const_iterator start, std::string::const_iterator end);

    /**
     * Retrieves the decoding table, block offsets, and padding stored in the compressed file.
     *
     * @param input_file the compressed file that the data will retrieved from.
     * @return the decoding table, block offsets, and padding stored in the compressed file.
     */
    static HeaderData getHeaderData(std::ifstream &input_file);

    /**
     * Converts a string of encoded text to a bit string that can be decoded.
     *
     * @param pool the thread pool that will be used for task submission, require that the thread pool has already been started.
     * @param encoded_text the text that the bit string will be created from.
     * @return a bit string created from the encoded text.
     */
    static std::string toBitString(Concurrent::ThreadPool &pool, const std::string &encoded_text);

    /**
     * Converts a string of encoded text to a string that can be decoded.
     *
     * @param start an iterator to a string of encoded text, the bit string will be created starting from this position.
     * @param end an iterator to a string of encoded text, the bit string will stop being created at this position.
     * @return a bit string created from the encoded text.
     */
    static std::string toBitString(std::string::const_iterator start, std::string::const_iterator end);

    // The size of the string that will be submitted to the thread pool for conversion to a bit string.
    // Note that using small numbers will result in poor performance.
    static constexpr uint32_t bit_string_block_size = 500;
};
#endif // CONCURRENT_HUFFMAN_DECODER_H
