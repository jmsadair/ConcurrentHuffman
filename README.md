![workflow](https://github.com/jmsadair/ConcurrentHuffman/actions/workflows/linux.yml/badge.svg)
![workflow](https://github.com/jmsadair/ConcurrentHuffman/actions/workflows/macos.yml/badge.svg)
[![codecov](https://codecov.io/gh/jmsadair/ConcurrentHuffman/branch/main/graph/badge.svg?token=40DYWKQQZ0)](https://codecov.io/gh/jmsadair/ConcurrentHuffman)
# ConcurrentHuffman
This is a parallelized implementation of the [Huffman coding algorithm](https://en.wikipedia.org/wiki/Huffman_coding) that can be used for ultra fast file compression and decompression. 
## Building
To build and test the project, run the following commands: 
```
  mkdir build && cd build

  cmake ..

  make -j

  make test
```
## Usage
Using this file compression tool is simple. To compress a file, provide the name of the file you would like to compress and a name for the compressed file that will be created.
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
Decompressing a file is similar. To decompress a file, provide a name of the file that you would like to decompress and a name for the decompressed file
that will be created.
```cpp
  #include "concurrent_huffman.h"
  
  int main()
  {
    // The file that is going to be decompressed.
    std::string file_to_decompress = "my_compressed_file.txt";
    // The decompressed file created from the compressed file.
    std::string decompressed_file = "my_decompressed_file.txt";
    // Decompress the file.
    ConcurrentHuffman::decompressFile(file_to_decompress, decompressed_file);
  }

```
Note that, in order to decompress a file, the compressed file must have been compressed with this tool.
## Benchmarks
The compression process was benchmarked using a 1 MB file consisting of various numeric characters. The decompression process was benchmarked using a 470 kB file (the compressed 1 MB file). All benchmarks were ran on an Intel Core i7-8700 processor, which supports up to 12 threads.
```
  -----------------------------------------------------------
  Benchmark                 Time             CPU   Iterations
  -----------------------------------------------------------
  BM_Compression         16.9 ms         15.9 ms           44
  BM_Decompression       17.9 ms         6.60 ms          106
```
