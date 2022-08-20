#include "concurrent_huffman.h"
#include "encoder.h"
#include "decoder.h"

void ConcurrentHuffman::compressFile(const std::string &file_to_compress, const std::string &compressed_file, uint32_t num_threads)
{
    Encoder::compressFile(file_to_compress, compressed_file, num_threads);
}

void ConcurrentHuffman::decompressFile(const std::string &file_to_decompress, const std::string &decompressed_file, uint32_t num_threads)
{
    Decoder::decompressFile(file_to_decompress, decompressed_file, num_threads);
}
