![workflow](https://github.com/jmsadair/ConcurrentHuffman/actions/workflows/linux.yml/badge.svg)
![workflow](https://github.com/jmsadair/ConcurrentHuffman/actions/workflows/macos.yml/badge.svg)
[![codecov](https://codecov.io/gh/jmsadair/ConcurrentHuffman/branch/main/graph/badge.svg?token=40DYWKQQZ0)](https://codecov.io/gh/jmsadair/ConcurrentHuffman)
# ConcurrentHuffman
This is a parallelized implementation of the [Huffman coding algorithm](https://en.wikipedia.org/wiki/Huffman_coding) that can be used for ultra fast file compression and decompression. 
# Building
To build and test the project, run the following commands: 
```
  mkdir build && cd build

  cmake ..

  make -j

  make test
```
# Usage
Using this file compression tool is simple. To encode a file, provide the name of the file you would like to compress and a name for the compressed file
that will be created.
```cpp
  #include "concurrent_huffman.h"
  
  int main()
  {
    // The file that is going to be compressed.
    std::string file_to_compress = "my_uncompressed_file.txt";
    // The compressed file created from the original file.
    std::string compressed_file = "my_compressed_file.txt";
    // Compress the file.
    ConcurrentHuffman::compressFile(file_to_compress, compressed_file);
  }

```
