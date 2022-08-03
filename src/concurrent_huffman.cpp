#include "concurrent_huffman.h"
#include "encoder.h"
#include "decoder.h"

void ConcurrentHuffman::encodeFile(const std::string &file_to_encode, const std::string &encoded_file)
{
    Encoder::encode(file_to_encode, encoded_file);
}
void ConcurrentHuffman::decodeFile(const std::string &file_to_decode, const std::string &decoded_file)
{
    Decoder::decode(file_to_decode, decoded_file);
}
