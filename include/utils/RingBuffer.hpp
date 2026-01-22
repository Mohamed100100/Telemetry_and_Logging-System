#pragma once

#include <vector>
#include <cstddef>
#include <stdexcept>

template <typename T>
class RingBuffer {
private:
    std::vector<T> buffer_;     // Use underscore suffix
    size_t head_;               // Next write position
    size_t tail_;               // Oldest element position
    size_t count_;              // Current number of elements
    size_t capacity_;           // Maximum capacity - RENAMED from 'capacity'

public:
    // ============================================
    // Constructors
    // ============================================
    
    RingBuffer() = delete;

    explicit RingBuffer(size_t cap)  // Parameter name different from member
        : buffer_(), 
          head_(0), 
          tail_(0), 
          count_(0), 
          capacity_(cap) {
        if (capacity_ == 0) {
            throw std::invalid_argument("RingBuffer capacity must be > 0");
        }
        buffer_.reserve(capacity_);  // Use reserve(), not resize()
    }

    RingBuffer(const RingBuffer& other) = delete;
    RingBuffer& operator=(const RingBuffer& other) = delete;

    RingBuffer(RingBuffer&& other) noexcept = default;
    RingBuffer& operator=(RingBuffer&& other) noexcept = default;

    ~RingBuffer() = default;

    // ============================================
    // Core Functions
    // ============================================

    // Add element to buffer (overwrites oldest if full)
    void push_back(const T& value) {
        if (count_ < capacity_) {
            // Buffer not full yet - add new element
            buffer_.push_back(value);
            head_ = (head_ + 1) % capacity_;
            count_++;
        } else {
            // Buffer full - overwrite oldest element
            buffer_[head_] = value;
            head_ = (head_ + 1) % capacity_;
            tail_ = (tail_ + 1) % capacity_;
        }
    }

    // Move version
    void push_back(T&& value) {
        if (count_ < capacity_) {
            buffer_.push_back(std::move(value));
            head_ = (head_ + 1) % capacity_;
            count_++;
        } else {
            buffer_[head_] = std::move(value);
            head_ = (head_ + 1) % capacity_;
            tail_ = (tail_ + 1) % capacity_;
        }
    }

    // Clear all elements
    void clear() {
        buffer_.clear();
        head_ = 0;
        tail_ = 0;
        count_ = 0;
    }

    // ============================================
    // Accessors
    // ============================================

    size_t size() const { return count_; }
    size_t capacity() const { return capacity_; }  // Now no conflict
    bool empty() const { return count_ == 0; }
    bool full() const { return count_ == capacity_; }

    // Access element at logical index (0 = oldest)
    T& operator[](size_t index) {
        if (index >= count_) {
            throw std::out_of_range("RingBuffer index out of range");
        }
        return buffer_[(tail_ + index) % capacity_];
    }

    const T& operator[](size_t index) const {
        if (index >= count_) {
            throw std::out_of_range("RingBuffer index out of range");
        }
        return buffer_[(tail_ + index) % capacity_];
    }
};