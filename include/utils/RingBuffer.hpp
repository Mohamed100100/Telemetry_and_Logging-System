#pragma once

#include <vector>
#include <optional>
#include <mutex>
#include <stdexcept>

/**
 * @brief Thread-safe circular buffer (ring buffer) implementation
 * 
 * A fixed-size buffer that operates in a circular manner. When the buffer
 * is full, try_push() returns false (does not overwrite).
 * 
 * Thread-Safety: All public methods are protected by a mutex.
 * 
 * @tparam T The type of elements stored in the buffer
 */
template <typename T>
class RingBuffer {
private:
    // ============================================
    // Member Variables
    // ============================================

    std::vector<std::optional<T>> buffer_;  // Storage using optional to represent empty slots
    size_t head_;                           // Index where next element will be written
    size_t tail_;                           // Index where next element will be read (oldest element)
    size_t count_;                          // Current number of elements in buffer
    size_t capacity_;                       // Maximum number of elements buffer can hold

    mutable std::mutex mutex_;              // Mutex for thread-safety
                                            // 'mutable' allows locking in const methods

public:
    // ============================================
    // Constructors & Destructor
    // ============================================
    
    /**
     * @brief Default constructor is deleted
     * 
     * RingBuffer must be created with a capacity.
     */
    RingBuffer() = delete;

    /**
     * @brief Constructs a RingBuffer with specified capacity
     * 
     * @param cap Maximum number of elements the buffer can hold
     * @throws std::invalid_argument if cap is 0
     * 
     * Note: buffer_(cap) creates 'cap' empty optionals (nullopt),
     * no T objects are constructed yet.
     */
    explicit RingBuffer(size_t cap)
        : buffer_(cap)      // Pre-allocate 'cap' empty optional slots
        , head_(0)          // Start writing at index 0
        , tail_(0)          // Start reading at index 0
        , count_(0)         // Buffer starts empty
        , capacity_(cap)    // Store capacity
    {
        if (capacity_ == 0) {
            throw std::invalid_argument("RingBuffer capacity must be > 0");
        }
    }

    /**
     * @brief Copy constructor - DELETED
     * 
     * RingBuffer should not be copied because:
     * 1. Mutex cannot be copied
     * 2. Copying a buffer in multi-threaded context is dangerous
     */
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    /**
     * @brief Move constructor - DEFAULT
     * 
     * Moving is allowed for transferring ownership.
     * Note: Moving a buffer that's being used by other threads is unsafe.
     * Only move before sharing with threads.
     */
    RingBuffer(RingBuffer&&) noexcept = default;
    RingBuffer& operator=(RingBuffer&&) noexcept = default;

    /**
     * @brief Destructor - DEFAULT
     * 
     * Vector and optional handle their own cleanup.
     */
    ~RingBuffer() = default;

    // ============================================
    // Core Functions
    // ============================================

    /**
     * @brief Attempts to push an item into the buffer
     * 
     * @param value The item to push (will be moved)
     * @return true if item was pushed successfully
     * @return false if buffer is full (item not pushed)
     * 
     * Thread-safe: Locks mutex during operation.
     * Non-blocking: Returns immediately with result.
     * 
     * Visual (capacity=5, pushing 'D'):
     * 
     * Before: [A][B][C][ ][ ]  count=3, head=3, tail=0
     *                  ^head
     * 
     * After:  [A][B][C][D][ ]  count=4, head=4, tail=0
     *                     ^head
     */
    template <typename U=T>
    bool try_push(U&& value) {
        std::lock_guard<std::mutex> lock(mutex_);   // Lock mutex (auto-unlocks when scope ends)

        // Check if buffer has space
        if (count_ >= capacity_) {
            return false;   // Buffer full, cannot push
        }

        // Place value at head position using move semantics
        // std::forward<U> preserves the value category (rvalue in this case)
        buffer_[head_] = std::move(std::forward<U>(value));

        // Move head to next position (wrap around using modulo)
        head_ = (head_ + 1) % capacity_;

        // Increment element count
        count_++;

        return true;    // Successfully pushed
    }

    /**
     * @brief Attempts to pop an item from the buffer
     * 
     * @return std::optional<T> containing the item, or nullopt if buffer is empty
     * 
     * Thread-safe: Locks mutex during operation.
     * Non-blocking: Returns immediately with result.
     * 
     * Visual (capacity=5, popping):
     * 
     * Before: [A][B][C][ ][ ]  count=3, head=3, tail=0
     *          ^tail
     * 
     * After:  [ ][B][C][ ][ ]  count=2, head=3, tail=1
     *             ^tail
     * 
     * Returns: optional containing 'A'
     */
    template <typename U=T>
    std::optional<U> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);   // Lock mutex

        // Check if buffer has items
        if (count_ == 0) {
            return std::nullopt;    // Buffer empty, nothing to pop
        }

        // Get value from tail position (oldest element)
        // *buffer_[tail_] dereferences the optional to get U value
        U value = std::move(*buffer_[tail_]);

        // Mark slot as empty
        buffer_[tail_] = std::nullopt;

        // Move tail to next position (wrap around)
        tail_ = (tail_ + 1) % capacity_;

        // Decrement element count
        count_--;

        return value;   // Return the popped value (wrapped in optional)
    }

    /**
     * @brief Clears all elements from the buffer
     * 
     * Thread-safe: Locks mutex during operation.
     * Resets buffer to initial empty state.
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);

        // Reset indices to starting positions
        head_ = 0;
        tail_ = 0;
        count_ = 0;

        // Reset all slots to empty (nullopt)
        buffer_.assign(capacity_, std::nullopt);
    }

    // ============================================
    // Getters
    // ============================================

    /**
     * @brief Returns current number of elements in buffer
     * 
     * Thread-safe: Locks mutex to ensure consistent read.
     */
    size_t size() const { 
        std::lock_guard<std::mutex> lock(mutex_);
        return count_; 
    }

    /**
     * @brief Returns maximum capacity of buffer
     * 
     * No lock needed: capacity never changes after construction.
     */
    size_t capacity() const { 
        return capacity_;
    }

    /**
     * @brief Checks if buffer is empty
     * 
     * Thread-safe: Locks mutex to ensure consistent read.
     * 
     * @return true if buffer has no elements
     */
    bool empty() const { 
        std::lock_guard<std::mutex> lock(mutex_);
        return count_ == 0; 
    }

    /**
     * @brief Checks if buffer is full
     * 
     * Thread-safe: Locks mutex to ensure consistent read.
     * 
     * @return true if buffer has reached capacity
     */
    bool full() const { 
        std::lock_guard<std::mutex> lock(mutex_);
        return count_ == capacity_; 
    }
};