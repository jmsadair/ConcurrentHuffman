#ifndef CONCURRENT_HUFFMAN_NODE_H
#define CONCURRENT_HUFFMAN_NODE_H
#include <memory>

struct Node
{
    explicit Node(uint64_t frequency_, char symbol_ = '\0', std::unique_ptr<Node> left_ = nullptr, std::unique_ptr<Node> right_ = nullptr)
        : frequency(frequency_)
        , symbol(symbol_)
        , left(std::move(left_))
        , right(std::move(right_))
    {}

    uint64_t frequency;
    char symbol;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};
#endif // CONCURRENT_HUFFMAN_NODE_H
