#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "concurrent_huffman.h"

// Tests encoding / decoding a file that only consists of a single, repeated character.
TEST(Huffman, EncodingAndDecodingTest1)
{
    // Read the file to encode into memory.
    std::string file_to_encode = "test1_input.txt";
    std::ifstream file1(file_to_encode);
    std::stringstream buffer1;
    buffer1 << file1.rdbuf();
    std::string expected_decoded_text = buffer1.str();

    std::string encoded_file = "test1_encoded.txt";
    std::string decoded_file = "test1_decoded.txt";
    ConcurrentHuffman::encodeFile(file_to_encode, encoded_file);
    ConcurrentHuffman::decodeFile(encoded_file, decoded_file);

    // Read the decoded file into memory.
    std::ifstream file2(decoded_file);
    std::stringstream buffer2;
    buffer2 << file2.rdbuf();
    std::string actual_decoded_text = buffer2.str();

    ASSERT_EQ(expected_decoded_text, actual_decoded_text);

    // Clean up the files created during the tests.
    std::remove("test1_encoded.txt");
    std::remove("test1_decoded.txt");
}

// Tests larger file to ensure that multiple threads encoding / decoding works.
TEST(Huffman, EncodingAndDecodingTest2)
{
    // Read the file to encode into memory.
    std::string file_to_encode = "test2_input.txt";
    std::ifstream file1(file_to_encode);
    std::stringstream buffer1;
    buffer1 << file1.rdbuf();
    std::string expected_decoded_text = buffer1.str();

    std::string encoded_file = "test2_encoded.txt";
    std::string decoded_file = "test2_decoded.txt";
    ConcurrentHuffman::encodeFile(file_to_encode, encoded_file);
    ConcurrentHuffman::decodeFile(encoded_file, decoded_file);

    // Read the decoded file into memory.
    std::ifstream file2(decoded_file);
    std::stringstream buffer2;
    buffer2 << file2.rdbuf();
    std::string actual_decoded_text = buffer2.str();

    ASSERT_EQ(expected_decoded_text, actual_decoded_text);

    // Clean up the files created during the tests.
    std::remove("test2_encoded.txt");
    std::remove("test2_decoded.txt");
}

// Tests encoding / decoding a file that consists of various ASCII characters.
TEST(Huffman, EncodingAndDecodingTest3)
{
    // Read the file to encode into memory.
    std::string file_to_encode = "test3_input.txt";
    std::ifstream file1(file_to_encode);
    std::stringstream buffer1;
    buffer1 << file1.rdbuf();
    std::string expected_decoded_text = buffer1.str();

    std::string encoded_file = "test3_encoded.txt";
    std::string decoded_file = "test3_decoded.txt";
    ConcurrentHuffman::encodeFile(file_to_encode, encoded_file);
    ConcurrentHuffman::decodeFile(encoded_file, decoded_file);

    // Read the decoded file into memory.
    std::ifstream file2(decoded_file);
    std::stringstream buffer2;
    buffer2 << file2.rdbuf();
    std::string actual_decoded_text = buffer2.str();

    ASSERT_EQ(expected_decoded_text, actual_decoded_text);

    // Clean up the files created during the tests.
    std::remove("test3_encoded.txt");
    std::remove("test3_decoded.txt");
}

// Tests encoding / decoding a file that consists of various ASCII characters.
TEST(Huffman, EncodingAndDecodingTest4)
{
    // Read the file to encode into memory.
    std::string file_to_encode = "test4_input.txt";
    std::ifstream file1(file_to_encode);
    std::stringstream buffer1;
    buffer1 << file1.rdbuf();
    std::string expected_decoded_text = buffer1.str();

    std::string encoded_file = "test4_encoded.txt";
    std::string decoded_file = "test4_decoded.txt";
    ConcurrentHuffman::encodeFile(file_to_encode, encoded_file);
    ConcurrentHuffman::decodeFile(encoded_file, decoded_file);

    // Read the decoded file into memory.
    std::ifstream file2(decoded_file);
    std::stringstream buffer2;
    buffer2 << file2.rdbuf();
    std::string actual_decoded_text = buffer2.str();

    ASSERT_EQ(expected_decoded_text, actual_decoded_text);

    // Clean up the files created during the tests.
    std::remove("test4_encoded.txt");
    std::remove("test4_decoded.txt");
}