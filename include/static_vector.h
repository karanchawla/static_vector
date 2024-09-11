#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <iterator>
#include <optional>
#include <new>

enum class StaticVectorErrorCode : uint8_t {
    NoError,
    OutOfSpace,
    OutOfRange,
    Empty,
    CannotDefaultConstruct
};

template<typename T, std::size_t Capacity>
class static_vector {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /**
     * @brief Default constructor. Initializes an empty static_vector.
     */
    constexpr static_vector() noexcept : size_(0) {}

    /**
     * @brief Copy constructor. Creates a static_vector as a copy of another.
     * @param other The static_vector to copy from.
     */
    constexpr static_vector(const static_vector& other) noexcept
        : size_(other.size_)
    {
        std::uninitialized_copy_n(other.begin(), size_, data());
    }

    /**
     * @brief Move constructor. Creates a static_vector by moving another.
     * @param other The static_vector to move from.
     */
    constexpr static_vector(static_vector&& other) noexcept
        : size_(other.size_)
    {
        std::uninitialized_move_n(other.begin(), size_, data());
        other.size_ = 0;
    }

    /**
     * @brief Copy assignment operator. Copies the contents of another static_vector.
     * @param other The static_vector to copy from.
     * @return Reference to this static_vector.
     */
    constexpr static_vector& operator=(const static_vector& other) noexcept {
        if (this != &other) {
            clear();
            size_ = other.size_;
            std::uninitialized_copy_n(other.begin(), size_, data());
        }
        return *this;
    }

    /**
     * @brief Move assignment operator. Moves the contents of another static_vector.
     * @param other The static_vector to move from.
     * @return Reference to this static_vector.
     */
    constexpr static_vector& operator=(static_vector&& other) noexcept {
        if (this != &other) {
            clear();
            size_ = other.size_;
            std::uninitialized_move_n(other.begin(), size_, data());
            other.clear();
        }
        return *this;
    }

    /**
     * @brief Destructor. Clears the static_vector.
     */
    ~static_vector() {
        clear();
    }

    /**
     * @brief Constructs an element in-place at the end of the static_vector.
     * @tparam Args Types of the arguments.
     * @param args Arguments to forward to the constructor of T.
     * @return StaticVectorErrorCode indicating success or failure.
     */
    template<typename... Args>
    constexpr StaticVectorErrorCode emplace_back(Args&&... args) noexcept {
        if (size_ >= Capacity) {
            return StaticVectorErrorCode::OutOfSpace;
        }
        ::new (static_cast<void*>(data() + size_)) T(std::forward<Args>(args)...);
        ++size_;
        return StaticVectorErrorCode::NoError;
    }

    /**
     * @brief Adds a copy of an element to the end of the static_vector.
     * @param value The value to copy.
     * @return StaticVectorErrorCode indicating success or failure.
     */
    template<typename U = T>
    typename std::enable_if<std::is_copy_constructible<U>::value, StaticVectorErrorCode>::type
    push_back(const T& value) noexcept {
        if (size_ >= Capacity) {
            return StaticVectorErrorCode::OutOfSpace;
        }
        ::new (static_cast<void*>(data() + size_)) T(value);
        ++size_;
        return StaticVectorErrorCode::NoError;
    }

    /**
     * @brief Adds a moved element to the end of the static_vector.
     * @param value The value to move.
     * @return StaticVectorErrorCode indicating success or failure.
     */
    template<typename U = T>
    typename std::enable_if<std::is_move_constructible<U>::value, StaticVectorErrorCode>::type
    push_back(T&& value) noexcept {
        if (size_ >= Capacity) {
            return StaticVectorErrorCode::OutOfSpace;
        }
        ::new (static_cast<void*>(data() + size_)) T(std::move(value));
        ++size_;
        return StaticVectorErrorCode::NoError;
    }

    /**
     * @brief Removes the last element from the static_vector.
     * @return StaticVectorErrorCode indicating success or failure.
     */
    constexpr StaticVectorErrorCode pop_back() noexcept {
        if (size_ == 0) {
            return StaticVectorErrorCode::Empty;
        }
        --size_;
        data()[size_].~T();
        return StaticVectorErrorCode::NoError;
    }

    /**
     * @brief Accesses the element at the given position.
     * @param pos Position of the element to access.
     * @return Reference to the element at the given position.
     */
    [[nodiscard]] constexpr reference operator[](size_type pos) noexcept {
        return data()[pos];
    }

    /**
     * @brief Accesses the element at the given position.
     * @param pos Position of the element to access.
     * @return Const reference to the element at the given position.
     */
    [[nodiscard]] constexpr const_reference operator[](size_type pos) const noexcept {
        return data()[pos];
    }

    /**
     * @brief Accesses the element at the given position, if it exists.
     * @param pos Position of the element to access.
     * @return Pointer to the element at the given position, or nullptr if out of range.
     */
    [[nodiscard]] constexpr pointer at_if(size_type pos) noexcept {
        return (pos < size_) ? &data()[pos] : nullptr;
    }

    /**
     * @brief Accesses the element at the given position, if it exists.
     * @param pos Position of the element to access.
     * @return Const pointer to the element at the given position, or nullptr if out of range.
     */
    [[nodiscard]] constexpr const_pointer at_if(size_type pos) const noexcept {
        return (pos < size_) ? &data()[pos] : nullptr;
    }

    /**
     * @brief Accesses the first element, if it exists.
     * @return Pointer to the first element, or nullptr if the static_vector is empty.
     */
    [[nodiscard]] constexpr pointer front_if() noexcept {
        return size_ > 0 ? &data()[0] : nullptr;
    }

    /**
     * @brief Accesses the first element, if it exists.
     * @return Const pointer to the first element, or nullptr if the static_vector is empty.
     */
    [[nodiscard]] constexpr const_pointer front_if() const noexcept {
        return size_ > 0 ? &data()[0] : nullptr;
    }

    /**
     * @brief Accesses the last element, if it exists.
     * @return Pointer to the last element, or nullptr if the static_vector is empty.
     */
    [[nodiscard]] constexpr pointer back_if() noexcept {
        return size_ > 0 ? &data()[size_ - 1] : nullptr;
    }

    /**
     * @brief Accesses the last element, if it exists.
     * @return Const pointer to the last element, or nullptr if the static_vector is empty.
     */
    [[nodiscard]] constexpr const_pointer back_if() const noexcept {
        return size_ > 0 ? &data()[size_ - 1] : nullptr;
    }

    /**
     * @brief Returns an iterator to the beginning of the static_vector.
     * @return Iterator to the beginning of the static_vector.
     */
    [[nodiscard]] constexpr iterator begin() noexcept { return data(); }

    /**
     * @brief Returns a const iterator to the beginning of the static_vector.
     * @return Const iterator to the beginning of the static_vector.
     */
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }

    /**
     * @brief Returns an iterator to the end of the static_vector.
     * @return Iterator to the end of the static_vector.
     */
    [[nodiscard]] constexpr iterator end() noexcept { return data() + size_; }

    /**
     * @brief Returns a const iterator to the end of the static_vector.
     * @return Const iterator to the end of the static_vector.
     */
    [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size_; }

    /**
     * @brief Returns a reverse iterator to the beginning of the reversed static_vector.
     * @return Reverse iterator to the beginning of the reversed static_vector.
     */
    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    /**
     * @brief Returns a const reverse iterator to the beginning of the reversed static_vector.
     * @return Const reverse iterator to the beginning of the reversed static_vector.
     */
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    /**
     * @brief Returns a reverse iterator to the end of the reversed static_vector.
     * @return Reverse iterator to the end of the reversed static_vector.
     */
    [[nodiscard]] constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    /**
     * @brief Returns a const reverse iterator to the end of the reversed static_vector.
     * @return Const reverse iterator to the end of the reversed static_vector.
     */
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    /**
     * @brief Returns a const iterator to the beginning of the static_vector.
     * @return Const iterator to the beginning of the static_vector.
     */
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }

    /**
     * @brief Returns a const iterator to the end of the static_vector.
     * @return Const iterator to the end of the static_vector.
     */
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

    /**
     * @brief Returns a const reverse iterator to the beginning of the reversed static_vector.
     * @return Const reverse iterator to the beginning of the reversed static_vector.
     */
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

    /**
     * @brief Returns a const reverse iterator to the end of the reversed static_vector.
     * @return Const reverse iterator to the end of the reversed static_vector.
     */
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

    /**
     * @brief Checks if the static_vector is empty.
     * @return True if the static_vector is empty, false otherwise.
     */
    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }

    /**
     * @brief Returns the number of elements in the static_vector.
     * @return The number of elements in the static_vector.
     */
    [[nodiscard]] constexpr size_type size() const noexcept { return size_; }

    /**
     * @brief Returns the maximum number of elements the static_vector can hold.
     * @return The capacity of the static_vector.
     */
    [[nodiscard]] constexpr size_type capacity() const noexcept { return Capacity; }

    /**
     * @brief Clears the static_vector, destroying all elements.
     */
    constexpr void clear() noexcept {
        for (size_type i = 0; i < size_; ++i) {
            data()[i].~T();
        }
        size_ = 0;
    }

private:
    alignas(T) std::array<std::byte, sizeof(T) * Capacity> storage_;
    size_type size_;

    /**
     * @brief Returns a pointer to the underlying data array.
     * @return Pointer to the underlying data array.
     */
    [[nodiscard]] constexpr T* data() noexcept {
        return std::launder(reinterpret_cast<T*>(storage_.data()));
    }

    /**
     * @brief Returns a const pointer to the underlying data array.
     * @return Const pointer to the underlying data array.
     */
    [[nodiscard]] constexpr const T* data() const noexcept {
        return std::launder(reinterpret_cast<const T*>(storage_.data()));
    }
};