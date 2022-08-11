#include "concurrent_huffman.h"

int main()
{
    // The file that is going to be compressed.
    std::string file_to_compress = "example_uncompressed.txt";
    // The compressed file created from the original file.
    std::string compressed_file = "compressed.txt";
    // The decompressed file created from the compressed file.
    std::string decompressed_file = "decompressed.txt";
    // Compress and then decompress the file.
    ConcurrentHuffman::compressFile(file_to_compress, compressed_file);
    ConcurrentHuffman::decompressFile(compressed_file, decompressed_file);
}