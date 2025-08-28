## 📚 Overview

This repository contains my comprehensive coursework portfolio for **Parallel Computing** from my Computer Science degree. The projects demonstrate proficiency in multithreading, parallel algorithms, performance optimization, and concurrent programming using modern C++ and industry-standard libraries.

## 🚀 Key Technical Skills Demonstrated

- **Parallel Programming**: C++17 threads, OpenMP, POSIX threads
- **Performance Analysis**: Benchmarking, speedup analysis, efficiency measurements
- **Memory Management**: Dynamic allocation, RAII principles, memory optimization
- **Concurrent Programming**: Mutexes, condition variables, lock-free data structures
- **Algorithm Optimization**: Load balancing, work distribution strategies
- **System Programming**: Low-level optimization, cache-aware algorithms

## 📁 Project Structure

### 🔧 Practice 1: Multithreading Fundamentals
**Location**: `Practica 1/`

Advanced multithreading application implementing mathematical operations (SUM, SUB, XOR) with configurable thread pools and optional logging system.

**Key Features:**
- Thread-safe array operations with up to 8 worker threads
- Concurrent logging system using condition variables
- Work distribution algorithms for load balancing
- Comprehensive benchmarking suite with performance metrics

**Technologies**: C++17, std::thread, std::mutex, condition_variable

```bash
# Compile and run
g++ -std=c++17 -pthread -O3 prac1.cc -o prac1
./prac1 1000000 SUM --multi-thread 4
```

### 📊 Practice 3: Matrix Multiplication Optimization
**Location**: `Practica 3/`

High-performance matrix multiplication implementations comparing sequential and parallel approaches using OpenMP.

**Key Features:**
- Sequential baseline implementation
- Parallel implementation with OpenMP work-sharing constructs
- Multiple scheduling strategies (static, dynamic, guided)
- Performance analysis with various matrix sizes
- Scalability testing up to 8 threads

**Technologies**: C, OpenMP, performance profiling

### 🎥 Practice 4: Video Processing Pipeline
**Location**: `Practica 4/`

Real-time video processing application applying Gaussian blur filters using OpenMP task parallelism.

**Key Features:**
- Binary video format I/O operations
- 5x5 Gaussian kernel implementation
- Task-based parallelism for frame processing
- Memory-efficient streaming processing
- Performance timing and optimization

**Technologies**: C, OpenMP tasks, image processing algorithms


## 📈 Performance Results

### Benchmarking Highlights
- **Speedup**: Achieved up to 3.74x speedup with 4 threads on matrix operations
- **Efficiency**: Maintained >90% efficiency for CPU-bound tasks up to 4 threads
- **Scalability**: Linear scaling demonstrated for matrix operations up to 4 threads
- **Thread Performance**: Optimal performance achieved with 4 threads, diminishing returns beyond that

### Sample Performance Data (Matrix Multiplication)
```
Matrix Operations:
Sequential Time: 10.1s
Parallel (4 threads): 2.7s
Parallel (8 threads): 4.0s
Speedup 4T: 3.74x
Speedup 8T: 2.53x
Efficiency 4T: 93.5%
Efficiency 8T: 31.6%
```

### Compilation Examples
```bash
# Practice 1 - Multithreading
g++ -std=c++17 -pthread -O3 -Wall prac1.cc -o prac1

# Practice 3 - OpenMP Matrix Operations
gcc -fopenmp -O3 -march=native matmul_p.c -o matmul_parallel

# Practice 4 - Video Processing
gcc -fopenmp -O3 video_task.c -o video_processor
```


