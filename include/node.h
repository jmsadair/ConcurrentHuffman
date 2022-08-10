#ifndef CONCURRENT_HUFFMAN_NODE_H
#define CONCURRENT_HUFFMAN_NODE_H
#include <memory>

/**
 * Represents a node in the Huffman tree.
 */
struct Node
{
    /**
     * A constructor for the node.
     *
     * @param frequency_ the number of times that symbol_ appears in unencoded text.
     * @param symbol_ a character that occurs in unencoded text.
     * @param left_ the left child of this node.
     * @param right_ the right child of this node.
     */
    explicit Node(uint64_t frequency_, char symbol_ = '\0', std::unique_ptr<Node> left_ = nullptr, std::unique_ptr<Node> right_ = nullptr);

    uint64_t frequency;
    char symbol;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};
#endif // CONCURRENT_HUFFMAN_NODE_H
