#include <algorithm>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <bitset>
#include "encoder.h"

void Encoder::encode(const std::string &file_to_encode, const std::string &encoded_file)
{
    // Start up the thread pool for encoding task submission.
    Concurrent::ThreadPool thread_pool;

    // Read the file into memory.
    std::ifstream input_file(file_to_encode);
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    std::string file_text = buffer.str();
    input_file.close();

    // Build the Huffman tree and create the encoding table.
    std::unordered_map<char, uint64_t> character_frequencies = countCharacterFrequencies(thread_pool, file_text);
    std::unique_ptr<Node> huffman_tree_root = constructHuffmanTree(character_frequencies);
    std::unordered_map<char, std::string> huffman_table = constructHuffmanTable(std::move(huffman_tree_root));

    // Encode the text and get the bytes to write to the encoded file.
    auto [bit_string, block_offsets] = toBitString(thread_pool, huffman_table, file_text, encoded_file);
    uint8_t padding = padBitString(bit_string);
    std::vector<unsigned char> bytes = toBytes(thread_pool, bit_string);

    std::ofstream output_file(encoded_file, std::ios::binary);

    // Write the table, offsets, and padding to the file.
    for (const auto& [symbol, code] : huffman_table)
        output_file << code << ',' << std::to_string(static_cast<int>(symbol)) << ' ';
    output_file << '\n';
    for (const auto& offset : block_offsets)
        output_file << std::to_string(offset) << ' ';
    output_file << '\n';
    output_file << std::to_string(padding) << '\n';

    // Write the encoded text to the file.
    std::copy(bytes.begin(), bytes.end(), std::ostreambuf_iterator<char>(output_file));

    output_file.close();
}

std::unordered_map<char, uint64_t> Encoder::countCharacterFrequencies(Concurrent::ThreadPool &pool, const std::string &file_text)
{
    // The hashmap that will hold the frequency of each character in the file.
    std::unordered_map<char, uint64_t> character_frequencies;

    // Blocks that will be submitted to thread pool for counting.
    uint32_t block_size = 500;
    uint32_t num_blocks = file_text.length() / block_size;
    auto block_start = file_text.begin();

    // A vector to hold the futures returned by the thread pool.
    std::vector<std::future<std::unordered_map<char, uint64_t>>> futures(num_blocks);

    // Submit blocks to thread pool for counting.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        auto block_end = block_start;
        std::advance(block_end, block_size);
        futures[i] = pool.submitTask([=] { return countBlock(block_start, block_end); });
        block_start = block_end;
    }

    // Count any remaining characters in the file string.
    character_frequencies = std::move(countBlock(block_start, file_text.end()));

    // Sum up all the counts from the blocks.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        std::unordered_map<char, uint64_t> block_counts = futures[i].get();
        for (const auto [character, count] : block_counts)
            character_frequencies[character] += count;
    }

    return character_frequencies;
}

std::unordered_map<char, uint64_t> Encoder::countBlock(std::string::const_iterator start, std::string::const_iterator end)
{
    std::unordered_map<char, uint64_t> character_counts;
    while (start != end)
    {
        ++character_counts[*start];
        ++start;
    }
    return character_counts;
}

std::unique_ptr<Node> Encoder::constructHuffmanTree(const std::unordered_map<char, uint64_t> &character_frequencies)
{
    auto sort = [](const std::unique_ptr<Node> &left, const std::unique_ptr<Node> &right) -> bool {
        return left->frequency > right->frequency;
    };

    std::vector<std::unique_ptr<Node>> heap;
    heap.reserve(character_frequencies.size());
    for (const auto [character, frequency] : character_frequencies)
        heap.push_back(std::make_unique<Node>(frequency, character));
    std::make_heap(heap.begin(), heap.end(), sort);

    while (heap.size() > 1)
    {
        // Get the two nodes with the lowest frequencies.
        std::unique_ptr<Node> right = std::move(heap.front());
        std::pop_heap(heap.begin(), heap.end(), sort);
        heap.pop_back();
        std::unique_ptr<Node> left = std::move(heap.front());
        std::pop_heap(heap.begin(), heap.end(), sort);
        heap.pop_back();
        // Note that a null character is used for nodes that do not have symbols.
        heap.push_back(std::make_unique<Node>(left->frequency + right->frequency, '\0', std::move(left), std::move(right)));
        std::push_heap(heap.begin(), heap.end(), sort);
    }

    return std::move(heap.front());
}

std::unordered_map<char, std::string> Encoder::constructHuffmanTable(std::unique_ptr<Node> huffman_tree_root)
{
    assert(huffman_tree_root && "Root must not be a null pointer!");
    std::unordered_map<char, std::string> encoding_table;
    std::deque<std::pair<Node *, std::string>> node_queue{std::make_pair(huffman_tree_root.get(), "")};

    if (!huffman_tree_root->left && !huffman_tree_root->right)
    {
        encoding_table.insert({huffman_tree_root->symbol, "0"});
        return encoding_table;
    }

    while (!node_queue.empty())
    {
        auto [node, code] = node_queue.front();
        node_queue.pop_front();
        // If the node has no children, then its symbol and code is added to the table.
        if (!node->left && !node->right)
        {
            encoding_table.insert({node->symbol, code});
            continue;
        }
        // A '0' is appended to the code string if it is the left node and
        // a '1' is appended to the code string if is a right node.
        if (node->left)
            node_queue.push_front(std::make_pair(node->left.get(), code + '0'));
        if (node->right)
            node_queue.push_front(std::make_pair(node->right.get(), code + '1'));
    }

    return encoding_table;
}

std::string Encoder::toBitString(
    const std::unordered_map<char, std::string> &huffman_table, std::string::const_iterator start, std::string::const_iterator end)
{
    std::string bit_string;
    while (start != end)
    {
        bit_string += huffman_table.at(*start);
        ++start;
    }
    return bit_string;
}

std::pair<std::string, std::vector<uint32_t>> Encoder::toBitString(Concurrent::ThreadPool &pool,
    const std::unordered_map<char, std::string> &huffman_table, const std::string &file_text, const std::string &encoded_file)
{
    // The entirety of the file encoded as a bit string.
    std::string bit_string;

    // Get the blocks of the file that each thread will encode.
    std::vector<uint32_t> block_offsets;
    uint32_t block_size = 500;
    uint32_t num_blocks = file_text.length() / block_size;
    auto block_start = file_text.begin();
    std::vector<std::future<std::string>> futures(num_blocks);

    // Submit blocks to thread pool for encoding.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        auto block_end = block_start;
        std::advance(block_end, block_size);
        futures[i] = pool.submitTask(
            [&table = std::as_const(huffman_table), start = block_start, end = block_end] { return toBitString(table, start, end); });
        block_start = block_end;
    }
    std::string last_encoded_block = toBitString(huffman_table, block_start, file_text.end());

    // Combine the text that each thread encoded into a single string.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        std::string encoded_block = futures[i].get();
        block_offsets.push_back(encoded_block.length());
        bit_string += encoded_block;
    }
    bit_string += last_encoded_block;

    return std::make_pair(bit_string, block_offsets);
}

uint8_t Encoder::padBitString(std::string &bit_string)
{
    uint8_t padding = 8 * ((bit_string.length() / 8) + 1) - bit_string.length();
    std::string padding_zeros(padding, '0');
    bit_string += padding_zeros;
    return padding;
}

std::vector<unsigned char> Encoder::toBytes(Concurrent::ThreadPool &pool, const std::string &bit_string)
{
    // Holds the entirety of the bytes produced by the bit string.
    std::vector<unsigned char> bytes;

    // Get the blocks of the file that each thread will encode.
    uint32_t block_size = 400;
    uint32_t num_blocks = bit_string.length() / block_size;
    auto block_start = bit_string.begin();
    std::vector<std::future<std::vector<unsigned char>>> futures(num_blocks);

    // Submit blocks to thread pool for encoding.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        auto block_end = block_start;
        std::advance(block_end, block_size);
        futures[i] = pool.submitTask([start = block_start, end = block_end] { return toBytes(start, end); });
        block_start = block_end;
    }
    std::vector<unsigned char> last_block = toBytes(block_start, bit_string.end());

    // Combine the bytes from each block.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        std::vector<unsigned char> block_bytes = futures[i].get();
        bytes.insert(bytes.end(), block_bytes.begin(), block_bytes.end());
    }
    bytes.insert(bytes.end(), last_block.begin(), last_block.end());

    return bytes;
}

std::vector<unsigned char> Encoder::toBytes(std::string::const_iterator start, std::string::const_iterator end)
{
    std::vector<unsigned char> bytes;
    bytes.reserve(std::distance(start, end) / 8);
    while (start != end)
    {
        std::string bit_string(start, start + 8);
        std::bitset<8> bits{bit_string};
        unsigned char byte = (bits.to_ulong() & 0xFF);
        bytes.push_back(byte);
        std::advance(start, 8);
    }
    return bytes;
}