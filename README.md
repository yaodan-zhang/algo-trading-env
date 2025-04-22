# Algo Trading Environment
*A real-time algorithmic trading simulation in C++20*

## Overview
This project simulates a live algorithmic trading environment designed to test trading strategies under real-time conditions. It focuses on **low-latency data processing**, **multithreaded execution**, and robust handling of asset price streams.

Originally built to explore high-performance trading systems, this simulator provides a flexible framework for developing and evaluating trading signals (*alphas*).

## Key Features
- 🚀 **Multithreaded Asset Streaming**: Simulates real-time market data feeds.
- ⚡ **Low-Latency Execution Engine**: Optimized for speed using C++20 concurrency features.
- 🔒 **RAII and Safe Concurrency**: Ensures memory safety and thread-safe operations.
- 📊 **Customizable Trading Logic**: Plug-and-play architecture for implementing trading strategies.

## Technologies Used
- **C++20**
- Standard Library Threads & Mutexes
- RAII Design Patterns

## Getting Started
### Prerequisites
- GCC supporting C++20
- Linux or macOS environment

### Build & Run
```bash
g++ -std=c++20 -O2 main.cpp -o trading_sim
./trading_sim
```

### Project Status
Core simulation engine: ✔️ Completed  
Alpha library: ✔️ Customizable

### Future work 
Logging enhancements, performance benchmarking

### License
This project is for educational and non-commercial use.
