#include "concurrent_huffman.h"

int main()
{
    // Number of threads to use during compression and decompression process.
    const uint32_t num_threads = 5;
    // The file that is going to be compressed.
    std::string file_to_compress = "example_uncompressed.txt";
    // The compressed file created from the original file.
    std::string compressed_file = "compressed.txt";
    // The decompressed file created from the compressed file.
    std::string decompressed_file = "decompressed.txt";
    // Compress and then decompress the file.
    ConcurrentHuffman::compressFile(file_to_compress, compressed_file, num_threads);
    ConcurrentHuffman::decompressFile(compressed_file, decompressed_file, num_threads);
}