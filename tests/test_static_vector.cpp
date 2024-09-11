/**
 * @file test_static_vector.cpp
 * @brief Unit tests for the static_vector class template.
 * 
 * This file contains unit tests for the static_vector class template,
 * which provides a fixed-capacity vector-like container.
 * 
 * These tests ensure the correctness and robustness of the static_vector
 * implementation.
 * 
 * @license MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @date 2024
 * @author Karan Chawla
 */

#include "static_vector.h"
#include <gtest/gtest.h>
#include <string>
#include <memory>
#include <chrono>

// Helper classes for testing complex types
class NonCopyable {
public:
    NonCopyable(const int& v) : value(v) {}
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
    bool operator==(const NonCopyable& other) const {
        return value == other.value;
    }
    int getValue() const { return value; }
private:
    int value;
};

class NonMovableAndNonCopyable {
public:
    NonMovableAndNonCopyable(int v) : value(v) {}
    
    // Delete copy constructor and assignment operator
    NonMovableAndNonCopyable(const NonMovableAndNonCopyable&) = delete;
    NonMovableAndNonCopyable& operator=(const NonMovableAndNonCopyable&) = delete;
    
    // Delete move constructor and assignment operator
    NonMovableAndNonCopyable(NonMovableAndNonCopyable&&) = delete;
    NonMovableAndNonCopyable& operator=(NonMovableAndNonCopyable&&) = delete;
    
    int getValue() const { return value; }

    bool operator==(const NonMovableAndNonCopyable& other) const {
        return value == other.value;
    }

private:
    int value;
};

class NonMovableType {
public:
    NonMovableType(int v) : value(v) {}
    NonMovableType(const NonMovableType&) = default;
    NonMovableType& operator=(const NonMovableType&) = default;
    NonMovableType(NonMovableType&&) = delete;
    NonMovableType& operator=(NonMovableType&&) = delete;
    
    bool operator==(const NonMovableType& other) const {
        return value == other.value;
    }
    
    int getValue() const { return value; }

private:
    int value;
};

class CustomDestructor {
public:
    CustomDestructor(int v, bool* destroyed) : value(v), destroyed_(destroyed) {}
    ~CustomDestructor() { if (destroyed_) *destroyed_ = true; }
    int getValue() const { return value; }
private:
    int value;
    bool* destroyed_;
};

class ComplexType {
public:
    ComplexType(int v, const std::string& s) : value(v), str(s) {}
    bool operator==(const ComplexType& other) const {
        return value == other.value && str == other.str;
    }
    int getValue() const { return value; }
    std::string getString() const { return str; }
private:
    int value;
    std::string str;
};

// Test fixture
template <typename T>
class StaticVectorTestWrapper : public ::testing::Test {
protected:
    static constexpr size_t TestCapacity = 5;
    using VectorType = static_vector<T, TestCapacity>;
    VectorType vec;

    template<typename U>
    U createTestValue(int n) {
        if constexpr (std::is_same_v<U, int>) {
            return n;
        } else if constexpr (std::is_same_v<U, std::string>) {
            return std::to_string(n);
        } else if constexpr (std::is_same_v<U, ComplexType>) {
            return ComplexType(n, std::to_string(n));
        } else if constexpr (std::is_same_v<U, NonCopyable>) {
            return NonCopyable(n);
        } else if constexpr (std::is_same_v<U, NonMovableType>) {
            return NonMovableType(n);
        } else {
            static_assert(always_false<U>, "Unsupported type for createTestValue");
        }
    }
    
    template<typename U>
    static constexpr bool always_false = false;
};

// Type-parameterized test case
using TestedTypes = ::testing::Types<int, std::string, ComplexType, NonCopyable, NonMovableType>;
TYPED_TEST_SUITE(StaticVectorTestWrapper, TestedTypes);

TYPED_TEST(StaticVectorTestWrapper, DefaultConstructor) {
    EXPECT_TRUE(this->vec.empty());
    EXPECT_EQ(this->vec.size(), 0);
    EXPECT_EQ(this->vec.capacity(), this->TestCapacity);
}

TYPED_TEST(StaticVectorTestWrapper, CopyConstructor) {
    if constexpr (std::is_copy_constructible_v<TypeParam>) {
        TypeParam value1 = this->template createTestValue<TypeParam>(1);
        TypeParam value2 = this->template createTestValue<TypeParam>(2);
        
        this->vec.push_back(value1);
        this->vec.push_back(value2);
        
        auto copy_vec = this->vec;
        EXPECT_EQ(copy_vec.size(), this->vec.size());
        EXPECT_EQ(copy_vec[0], this->vec[0]);
        EXPECT_EQ(copy_vec[1], this->vec[1]);
    }
}

TYPED_TEST(StaticVectorTestWrapper, MoveConstructor) {
    if constexpr (std::is_move_constructible_v<TypeParam>) {
        this->vec.push_back(this->template createTestValue<TypeParam>(1));
        this->vec.push_back(this->template createTestValue<TypeParam>(2));
        auto moved_vec = std::move(this->vec);
        EXPECT_EQ(moved_vec.size(), 2);
        EXPECT_TRUE(this->vec.empty());
    }
}

TYPED_TEST(StaticVectorTestWrapper, CopyAssignment) {
    if constexpr (std::is_copy_assignable_v<TypeParam>) {
        this->vec.push_back(this->template createTestValue<TypeParam>(1));
        this->vec.push_back(this->template createTestValue<TypeParam>(2));
        decltype(this->vec) other_vec;
        other_vec = this->vec;
        EXPECT_EQ(other_vec.size(), this->vec.size());
        EXPECT_EQ(other_vec[0], this->vec[0]);
    }
}

TYPED_TEST(StaticVectorTestWrapper, MoveAssignment) {
    if constexpr (std::is_move_assignable_v<TypeParam>) {
        this->vec.push_back(this->template createTestValue<TypeParam>(1));
        this->vec.push_back(this->template createTestValue<TypeParam>(2));
        decltype(this->vec) other_vec;
        other_vec = std::move(this->vec);
        EXPECT_EQ(other_vec.size(), 2);
        EXPECT_TRUE(this->vec.empty());
    }
}

TYPED_TEST(StaticVectorTestWrapper, SubscriptOperator) {
    this->vec.push_back(this->template createTestValue<TypeParam>(1));
    this->vec.push_back(this->template createTestValue<TypeParam>(2));
    EXPECT_EQ(this->vec[0], this->template createTestValue<TypeParam>(1));
    EXPECT_EQ(this->vec[1], this->template createTestValue<TypeParam>(2));
}

TYPED_TEST(StaticVectorTestWrapper, AtIf) {
    EXPECT_EQ(this->vec.at_if(0), nullptr);

    this->vec.push_back(this->template createTestValue<TypeParam>(1));
    this->vec.push_back(this->template createTestValue<TypeParam>(2));

    auto ptr0 = this->vec.at_if(0);
    auto ptr1 = this->vec.at_if(1);
    ASSERT_NE(ptr0, nullptr);
    ASSERT_NE(ptr1, nullptr);
    EXPECT_EQ(*ptr0, this->template createTestValue<TypeParam>(1));
    EXPECT_EQ(*ptr1, this->template createTestValue<TypeParam>(2));
    EXPECT_EQ(this->vec.at_if(2), nullptr);
}

TYPED_TEST(StaticVectorTestWrapper, FrontIf) {
    EXPECT_EQ(this->vec.front_if(), nullptr);
    this->vec.push_back(this->template createTestValue<TypeParam>(1));
    auto front = this->vec.front_if();
    ASSERT_NE(front, nullptr);
    EXPECT_EQ(*front, this->template createTestValue<TypeParam>(1));
}

TYPED_TEST(StaticVectorTestWrapper, BackIf) {
    EXPECT_EQ(this->vec.back_if(), nullptr);
    this->vec.push_back(this->template createTestValue<TypeParam>(1));
    this->vec.push_back(this->template createTestValue<TypeParam>(2));
    auto back = this->vec.back_if();
    ASSERT_NE(back, nullptr);
    EXPECT_EQ(*back, this->template createTestValue<TypeParam>(2));
}

// Capacity Tests
TYPED_TEST(StaticVectorTestWrapper, Empty) {
    EXPECT_TRUE(this->vec.empty());
    this->vec.push_back(this->template createTestValue<TypeParam>(1));
    EXPECT_FALSE(this->vec.empty());
}

TYPED_TEST(StaticVectorTestWrapper, Size) {
    EXPECT_EQ(this->vec.size(), 0);
    this->vec.push_back(this->template createTestValue<TypeParam>(1));
    EXPECT_EQ(this->vec.size(), 1);
    this->vec.push_back(this->template createTestValue<TypeParam>(2));
    EXPECT_EQ(this->vec.size(), 2);
}

TYPED_TEST(StaticVectorTestWrapper, Capacity) {
    EXPECT_EQ(this->vec.capacity(), this->TestCapacity);
}

// Modifier tests
TYPED_TEST(StaticVectorTestWrapper, Clear) {
    this->vec.push_back(this->template createTestValue<TypeParam>(1));
    this->vec.push_back(this->template createTestValue<TypeParam>(2));
    this->vec.clear();
    EXPECT_TRUE(this->vec.empty());
    EXPECT_EQ(this->vec.size(), 0);
}

TYPED_TEST(StaticVectorTestWrapper, PushBack) {
    EXPECT_EQ(this->vec.push_back(this->template createTestValue<TypeParam>(1)), StaticVectorErrorCode::NoError);
    EXPECT_EQ(this->vec.size(), 1);
    EXPECT_EQ(this->vec.push_back(this->template createTestValue<TypeParam>(2)), StaticVectorErrorCode::NoError);
    EXPECT_EQ(this->vec.size(), 2);
}

TYPED_TEST(StaticVectorTestWrapper, EmplaceBack) {
    if constexpr (std::is_same_v<TypeParam, int>) {
        EXPECT_EQ(this->vec.emplace_back(1), StaticVectorErrorCode::NoError);
        EXPECT_EQ(this->vec.size(), 1);
        EXPECT_EQ(this->vec.emplace_back(2), StaticVectorErrorCode::NoError);
        EXPECT_EQ(this->vec.size(), 2);
    } else if constexpr (std::is_same_v<TypeParam, std::string>) {
        EXPECT_EQ(this->vec.emplace_back("one"), StaticVectorErrorCode::NoError);
        EXPECT_EQ(this->vec.size(), 1);
        EXPECT_EQ(this->vec.emplace_back("two"), StaticVectorErrorCode::NoError);
        EXPECT_EQ(this->vec.size(), 2);
    } else if constexpr (std::is_same_v<TypeParam, ComplexType>) {
        EXPECT_EQ(this->vec.emplace_back(1, "one"), StaticVectorErrorCode::NoError);
        EXPECT_EQ(this->vec.size(), 1);
        EXPECT_EQ(this->vec.emplace_back(2, "two"), StaticVectorErrorCode::NoError);
        EXPECT_EQ(this->vec.size(), 2);
    } else if constexpr (std::is_same_v<TypeParam, NonCopyable>) {
        EXPECT_EQ(this->vec.emplace_back(1), StaticVectorErrorCode::NoError);
        EXPECT_EQ(this->vec.size(), 1);
        EXPECT_EQ(this->vec.emplace_back(2), StaticVectorErrorCode::NoError);
        EXPECT_EQ(this->vec.size(), 2);
    } else if constexpr (std::is_same_v<TypeParam, NonMovableType>) {
        EXPECT_EQ(this->vec.emplace_back(1), StaticVectorErrorCode::NoError);
        EXPECT_EQ(this->vec.size(), 1);
        EXPECT_EQ(this->vec.emplace_back(2), StaticVectorErrorCode::NoError);
        EXPECT_EQ(this->vec.size(), 2);
    }
}

TYPED_TEST(StaticVectorTestWrapper, PopBack) {
    this->vec.push_back(this->template createTestValue<TypeParam>(1));
    this->vec.push_back(this->template createTestValue<TypeParam>(2));
    EXPECT_EQ(this->vec.pop_back(), StaticVectorErrorCode::NoError);
    EXPECT_EQ(this->vec.size(), 1);
    EXPECT_EQ(this->vec.pop_back(), StaticVectorErrorCode::NoError);
    EXPECT_TRUE(this->vec.empty());
    EXPECT_EQ(this->vec.pop_back(), StaticVectorErrorCode::Empty);
}

TYPED_TEST(StaticVectorTestWrapper, Iterators) {
    this->vec.push_back(this->template createTestValue<TypeParam>(1));
    this->vec.push_back(this->template createTestValue<TypeParam>(2));
    this->vec.push_back(this->template createTestValue<TypeParam>(3));

    // Forward iteration
    auto it = this->vec.begin();
    EXPECT_EQ(*it, this->template createTestValue<TypeParam>(1));
    ++it;
    EXPECT_EQ(*it, this->template createTestValue<TypeParam>(2));
    ++it;
    EXPECT_EQ(*it, this->template createTestValue<TypeParam>(3));
    ++it;
    EXPECT_EQ(it, this->vec.end());

    // Reverse iteration
    auto rit = this->vec.rbegin();
    EXPECT_EQ(*rit, this->template createTestValue<TypeParam>(3));
    ++rit;
    EXPECT_EQ(*rit, this->template createTestValue<TypeParam>(2));
    ++rit;
    EXPECT_EQ(*rit, this->template createTestValue<TypeParam>(1));
    ++rit;
    EXPECT_EQ(rit, this->vec.rend());
}


// Specific tests for complex types
TEST(StaticVectorComplexTypeTest, NonCopyableType) {
    static_vector<NonCopyable, 5> vec;
    EXPECT_EQ(vec.emplace_back(1), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.emplace_back(2), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0].getValue(), 1);
    EXPECT_EQ(vec[1].getValue(), 2);
}

TEST(StaticVectorComplexTypeTest, NonMovableType) {
    static_vector<NonMovableType, 5> vec;
    EXPECT_EQ(vec.emplace_back(1), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.emplace_back(2), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0].getValue(), 1);
    EXPECT_EQ(vec[1].getValue(), 2);

    // Test copy of NonMovableType
    NonMovableType nmv(3);
    EXPECT_EQ(vec.push_back(nmv), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[2].getValue(), 3);
}

TEST(StaticVectorComplexTypeTest, CustomDestructorType) {
    bool destroyed = false;
    {
        static_vector<CustomDestructor, 5> vec;
        EXPECT_EQ(vec.emplace_back(1, &destroyed), StaticVectorErrorCode::NoError);
        EXPECT_EQ(vec.size(), 1);
        EXPECT_EQ(vec[0].getValue(), 1);
    }
    EXPECT_TRUE(destroyed);
}

TEST(StaticVectorComplexTypeTest, ComplexTypeOperations) {
    static_vector<ComplexType, 5> vec;
    EXPECT_EQ(vec.emplace_back(1, "one"), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.emplace_back(2, "two"), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0].getValue(), 1);
    EXPECT_EQ(vec[0].getString(), "one");
    EXPECT_EQ(vec[1].getValue(), 2);
    EXPECT_EQ(vec[1].getString(), "two");
}

// Edge case tests
TEST(StaticVectorEdgeCaseTest, PushBackWhenFull) {
    static_vector<int, 3> vec;
    EXPECT_EQ(vec.push_back(1), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.push_back(2), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.push_back(3), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.push_back(4), StaticVectorErrorCode::OutOfSpace);
    EXPECT_EQ(vec.size(), 3);
}

TEST(StaticVectorEdgeCaseTest, EmplaceBackWhenFull) {
    static_vector<std::string, 3> vec;
    EXPECT_EQ(vec.emplace_back("one"), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.emplace_back("two"), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.emplace_back("three"), StaticVectorErrorCode::NoError);
    EXPECT_EQ(vec.emplace_back("four"), StaticVectorErrorCode::OutOfSpace);
    EXPECT_EQ(vec.size(), 3);
}

// Helper function to measure execution time
template<typename Func>
long long measureExecutionTime(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

// Performance test for push_back
TEST(PerformanceTest, PushBack) {
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

// Performance test for iteration
TEST(PerformanceTest, Iteration) {
    constexpr size_t numElements = 1000000;
    
    static_vector<int, numElements> sv;
    std::vector<int> v;
    v.reserve(numElements);

    for (size_t i = 0; i < numElements; ++i) {
        sv.push_back(i);
        v.push_back(i);
    }

    auto staticVectorTime = measureExecutionTime([&]() {
        long long sum = 0;
        for (const auto& item : sv) {
            sum += item;
        }
        // Prevent compiler from optimizing away the loop
        volatile long long result = sum;
    });

    auto stdVectorTime = measureExecutionTime([&]() {
        long long sum = 0;
        for (const auto& item : v) {
            sum += item;
        }
        // Prevent compiler from optimizing away the loop
        volatile long long result = sum;
    });

    std::cout << "Iterate over " << numElements << " elements:\n";
    std::cout << "static_vector time: " << staticVectorTime << " microseconds\n";
    std::cout << "std::vector time:   " << stdVectorTime << " microseconds\n";
}