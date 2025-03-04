# Lock-Free Data Structures Implementation

A C++ implementation of lock-free linked lists and skip lists for concurrent systems, focusing on thread safety and performance.

## Overview

This project implements thread-safe, lock-free versions of:
- Linked Lists
- Skip Lists

The implementations avoid traditional locking mechanisms to provide better concurrency and performance in multi-threaded environments.

## Features

- Lock-free synchronization using atomic operations
- Thread-safe operations (insert, delete, find)
- Comprehensive test suite
- Performance benchmarking tools

## Implementation Details

### Lock-free Linked List
- Uses atomic Compare-and-Swap (CAS) operations
- Supports concurrent insertions and deletions
- Maintains list integrity without locks

### Lock-free Skip List
- Probabilistic data structure for faster search
- Lock-free operations at all levels
- Optimized for concurrent access

## Testing

The project includes:
- Stress tests with configurable parameters
- Correctness verification tests
- Performance comparison with single-threaded implementations

## Contributors

- **Jin Park**
  - Lock-free linked list implementation
  - Stress testing framework
  - Performance analysis

- **WonHyeong Chae**
  - Lock-free skip list implementation
  - Correctness testing
  - Benchmarking framework

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
