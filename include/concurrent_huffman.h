#ifndef CONCURRENT_HUFFMAN_CONCURRENT_HUFFMAN_H
#define CONCURRENT_HUFFMAN_CONCURRENT_HUFFMAN_H
#include <vector>
#include <string>
#include <unordered_map>

struct ConcurrentHuffman
{
    /**
     * Compresses a file.
     *
     * @param file_to_compress the file that will be compressed, require that the file exists
     *                         and that the file is not already compressed.
     * @param compressed_file the name of the compressed file that will be created.
     */
    static void compressFile(const std::string &file_to_compress, const std::string &compressed_file);

    /**
     * Decompresses a file.
     *
     * @param file_to_decompress the file that will be decompressed, require that the file exists
     *                           and that file is compressed.
     * @param decompressed_file the name of the decompressed file that will be created.
     */
    static void decompressFile(const std::string &file_to_decompress, const std::string &decompressed_file);
};
#endif // CONCURRENT_HUFFMAN_CONCURRENT_HUFFMAN_H
