# static_vector

A high-performance, fixed-capacity vector implementation for C++17.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Building the Project](#building-the-project)
- [Running Tests](#running-tests)
- [Usage](#usage)
- [Performance](#performance)
- [Contributing](#contributing)
- [License](#license)

## Introduction

`static_vector` is a C++17 implementation of a fixed-capacity vector-like container. It provides the functionality of `std::vector` with the added benefit of not using dynamic memory allocation, making it ideal for performance-critical applications or environments where dynamic memory allocation is not desirable.

## Features

- Fixed-capacity vector implementation
- STL-compatible interface
- No dynamic memory allocation
- Supports move-only and non-movable types
- Comprehensive error handling
- Extensive test suite

## Requirements

- C++17 compatible compiler
- CMake 3.10 or higher
- Google Test (for running tests)

## Building the Project

1. Clone the repository:
   ```
   git clone https://github.com/your-username/static_vector.git
   cd static_vector
   ```

2. Create a build directory:
   ```
   mkdir build && cd build
   ```

3. Configure the project with CMake:
   ```
   cmake ..
   ```

4. Build the project:
   ```
   cmake --build .
   ```

## Running Tests

After building the project, you can run the tests using:

```
ctest
```

or directly execute the test binary:

```
./tests/test_static_vector
```

The test suite covers a wide range of scenarios, including edge cases and performance comparisons with `std::vector`.

## Usage

To use `static_vector` in your project, include the header file:

```cpp
#include "static_vector.h"
```

Here's a basic example of how to use `static_vector`:

```cpp
#include "static_vector.h"
#include <iostream>

int main() {
    static_vector<int, 5> vec;
    
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    
    for (const auto& item : vec) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
```

For more detailed usage examples, please refer to the test file `tests/test_static_vector.cpp`.

## Performance

`static_vector` is designed to be a high-performance alternative to `std::vector` for scenarios where the maximum size is known at compile-time. It avoids dynamic memory allocation, which can lead to better performance in many cases.

A performance comparison test is included in the test suite:

```cpp
// Performance test for push_back
TEST(StaticVectorPerformanceTest, PushBackPerformance) {
    constexpr size_t numElements = 100000;
    
    auto staticVectorTime = measureExecutionTime([&]() {
        static_vector<int, numElements> sv;
        for (size_t i = 0; i < numElements; ++i) {
            sv.push_back(i);
        }
    });

    auto stdVectorTime = measureExecutionTime([&]() {
        std::vector<int> v;
        v.reserve(numElements);  // Fair comparison: preallocate memory
        for (size_t i = 0; i < numElements; ++i) {
            v.push_back(i);
        }
    });

    std::cout << "Push back " << numElements << " elements:\n";
    std::cout << "static_vector time: " << staticVectorTime << " microseconds\n";
    std::cout << "std::vector time:   " << stdVectorTime << " microseconds\n";
}
```

This test compares the performance of `static_vector` against `std::vector` for push_back operations.

## Contributing

Contributions to `static_vector` are welcome! Please follow these steps:

1. Fork the repository
2. Create a new branch for your feature or bug fix
3. Make your changes and commit them with clear, descriptive messages
4. Push your changes to your fork
5. Submit a pull request

Please ensure that your code adheres to the existing style and that all tests pass before submitting a pull request.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

```
MIT License

Copyright (c) 2024 Karan Chawla

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```