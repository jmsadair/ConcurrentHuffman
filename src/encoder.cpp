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
    std::ifstream file(file_to_encode);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_text = buffer.str();
    file.close();

    // Encode the file.
    std::unordered_map<char, uint64_t> character_frequencies = countCharacterFrequencies(thread_pool, file_text);
    std::unique_ptr<Node> huffman_tree_root = constructHuffmanTree(character_frequencies);
    std::unordered_map<char, std::string> huffman_table = constructHuffmanTable(std::move(huffman_tree_root));
    encodeFile(thread_pool, huffman_table, file_text, encoded_file);
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
    std::unordered_map<char, std::string> huffman_table;
    std::deque<std::pair<Node *, std::string>> node_queue{std::make_pair(huffman_tree_root.get(), "")};

    while (!node_queue.empty())
    {
        auto [node, code] = node_queue.front();
        node_queue.pop_front();
        // If the node has no children, then its symbol and code is added to the table.
        if (!node->left && !node->right)
        {
            huffman_table.insert({node->symbol, code});
            continue;
        }
        // A '0' is appended to the code string if it is the left node and
        // a '1' is appended to the code string if is a right node.
        if (node->left)
            node_queue.push_front(std::make_pair(node->left.get(), code + '0'));
        if (node->right)
            node_queue.push_front(std::make_pair(node->right.get(), code + '1'));
    }

    return huffman_table;
}

std::string Encoder::encodeBlock(
    const std::unordered_map<char, std::string> &huffman_table, std::string::const_iterator start, std::string::const_iterator end)
{
    std::string encoded;
    while (start != end)
    {
        encoded += huffman_table.at(*start);
        ++start;
    }
    return encoded;
}

void Encoder::encodeFile(Concurrent::ThreadPool &pool, const std::unordered_map<char, std::string> &huffman_table, const std::string &file_text, const std::string &encoded_file)
{
    // The entirety of the provided file encoded.
    std::string encoded_text;

    // Get the blocks of the file that each thread will encode.
    std::vector<uint64_t> block_offsets;
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
            [&table = std::as_const(huffman_table), start = block_start, end = block_end] { return encodeBlock(table, start, end); });
        block_start = block_end;
    }
    std::string last_encoded_block = encodeBlock(huffman_table, block_start, file_text.end());

    // Combine the text that each thread encoded into a single string.
    for (uint32_t i = 0; i < num_blocks; ++i)
    {
        std::string encoded_block = futures[i].get();
        block_offsets.push_back(encoded_block.size());
        encoded_text += encoded_block;
    }
    encoded_text += last_encoded_block;

    std::ofstream file(encoded_file);
    file << std::to_string(huffman_table.size()) << '\n';
    // Write symbols and corresponding codes to file.
    for (const auto& [symbol, code] : huffman_table)
        file << code << ':' << static_cast<int>(symbol) << '\n';
    file << std::to_string(block_offsets.size()) << '\n';
    // Write block offsets to file.
    for (const auto& offset : block_offsets)
        file << std::to_string(offset) << '\n';
    // Write the encoded text to the file.
    file << encoded_text;
    file.close();
}