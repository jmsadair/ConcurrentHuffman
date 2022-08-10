#include "node.h"

Node::Node(uint64_t frequency_, char symbol_, std::unique_ptr<Node> left_, std::unique_ptr<Node> right_)
    : frequency(frequency_)
    , symbol(symbol_)
    , left(std::move(left_))
    , right(std::move(right_))
{}
