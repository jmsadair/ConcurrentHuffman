#ifndef CONCURRENTHUFFMAN_ENCODER_H
#define CONCURRENTHUFFMAN_ENCODER_H
#include <string>
#include <atomic>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <cmath>
#include <fstream>
#include "concurrent/thread_pool.h"

class Encoder
{
public:
    explicit Encoder(std::string encode_file_);

private:
    void countCharacterFrequenciesConcurrent();
    void countCharacterFrequencies();
    static std::unordered_map<char, uint64_t> countCharacterFrequencies(std::string::iterator start, std::string::iterator end);
    Concurrent::ThreadPool thread_pool;
    std::unordered_map<char, uint64_t> frequencies;
    std::string encode_file;
};
#endif // CONCURRENTHUFFMAN_ENCODER_H
