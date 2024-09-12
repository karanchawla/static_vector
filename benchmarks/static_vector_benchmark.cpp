#include <benchmark/benchmark.h>
#include <vector>
#include "static_vector.h"
#include <random>
#include <algorithm>

// Helper function to generate random data
template<typename T>
std::vector<T> generate_random_data(size_t size) {
    std::vector<T> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dis(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        std::generate(data.begin(), data.end(), [&]() { return dis(gen); });
    } else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dis(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        std::generate(data.begin(), data.end(), [&]() { return dis(gen); });
    } else {
        // For complex types, just use a simple incrementing value
        T value{};
        std::generate(data.begin(), data.end(), [&]() { return value++; });
    }
    return data;
}

// Benchmark: Construction
template<typename VectorType>
static void BM_Construction(benchmark::State& state) {
    const size_t N = state.range(0);
    auto data = generate_random_data<typename VectorType::value_type>(N);
    for (auto _ : state) {
        VectorType v;
        for (const auto& item : data) {
            v.push_back(item);
        }
        benchmark::DoNotOptimize(v);
    }
    state.SetItemsProcessed(N * state.iterations());
}

// Benchmark: push_back
template<typename VectorType>
static void BM_PushBack(benchmark::State& state) {
    const size_t N = state.range(0);
    auto data = generate_random_data<typename VectorType::value_type>(N);
    for (auto _ : state) {
        VectorType v;
        if constexpr (std::is_same_v<VectorType, std::vector<typename VectorType::value_type>>) {
            v.reserve(N);  // Fair comparison: preallocate for std::vector
        }
        for (const auto& item : data) {
            v.push_back(item);
        }
        benchmark::DoNotOptimize(v);
    }
    state.SetItemsProcessed(N * state.iterations());
}

// Benchmark: emplace_back
template<typename VectorType>
static void BM_EmplaceBack(benchmark::State& state) {
    const size_t N = state.range(0);
    auto data = generate_random_data<typename VectorType::value_type>(N);
    for (auto _ : state) {
        VectorType v;
        if constexpr (std::is_same_v<VectorType, std::vector<typename VectorType::value_type>>) {
            v.reserve(N);  // Fair comparison: preallocate for std::vector
        }
        for (const auto& item : data) {
            v.emplace_back(item);
        }
        benchmark::DoNotOptimize(v);
    }
    state.SetItemsProcessed(N * state.iterations());
}

// Benchmark: Random Access
template<typename VectorType>
static void BM_RandomAccess(benchmark::State& state) {
    const size_t N = state.range(0);
    auto data = generate_random_data<typename VectorType::value_type>(N);
    VectorType v;
    for (const auto& item : data) {
        v.push_back(item);
    }
    std::vector<size_t> indices(N);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), std::mt19937{std::random_device{}()});
    
    for (auto _ : state) {
        for (size_t i = 0; i < N; ++i) {
            benchmark::DoNotOptimize(v[indices[i]]);
        }
    }
    state.SetItemsProcessed(N * state.iterations());
}

// Benchmark: Iteration
template<typename VectorType>
static void BM_Iteration(benchmark::State& state) {
    const size_t N = state.range(0);
    auto data = generate_random_data<typename VectorType::value_type>(N);
    VectorType v;
    for (const auto& item : data) {
        v.push_back(item);
    }
    
    for (auto _ : state) {
        for (auto& item : v) {
            benchmark::DoNotOptimize(item);
            benchmark::ClobberMemory();
        }
    }
    state.SetItemsProcessed(N * state.iterations());
}

// Benchmark: pop_back
template<typename VectorType>
static void BM_PopBack(benchmark::State& state) {
    const size_t N = state.range(0);
    auto data = generate_random_data<typename VectorType::value_type>(N);
    for (auto _ : state) {
        state.PauseTiming();
        VectorType v;
        for (const auto& item : data) {
            v.push_back(item);
        }
        state.ResumeTiming();
        while (!v.empty()) {
            v.pop_back();
        }
        benchmark::DoNotOptimize(v);
    }
    state.SetItemsProcessed(N * state.iterations());
}

// Define the vectors to benchmark
using IntVector = std::vector<int>;
using IntStaticVector = static_vector<int, 1000000>;  // Adjust max size as needed

// Register benchmarks
BENCHMARK_TEMPLATE(BM_Construction, IntVector)->Range(8, 8<<10);
BENCHMARK_TEMPLATE(BM_Construction, IntStaticVector)->Range(8, 8<<10);

BENCHMARK_TEMPLATE(BM_PushBack, IntVector)->Range(8, 8<<10);
BENCHMARK_TEMPLATE(BM_PushBack, IntStaticVector)->Range(8, 8<<10);

BENCHMARK_TEMPLATE(BM_EmplaceBack, IntVector)->Range(8, 8<<10);
BENCHMARK_TEMPLATE(BM_EmplaceBack, IntStaticVector)->Range(8, 8<<10);

BENCHMARK_TEMPLATE(BM_RandomAccess, IntVector)->Range(8, 8<<10);
BENCHMARK_TEMPLATE(BM_RandomAccess, IntStaticVector)->Range(8, 8<<10);

BENCHMARK_TEMPLATE(BM_Iteration, IntVector)->Range(8, 8<<10);
BENCHMARK_TEMPLATE(BM_Iteration, IntStaticVector)->Range(8, 8<<10);

BENCHMARK_TEMPLATE(BM_PopBack, IntVector)->Range(8, 8<<10);
BENCHMARK_TEMPLATE(BM_PopBack, IntStaticVector)->Range(8, 8<<10);

BENCHMARK_MAIN();