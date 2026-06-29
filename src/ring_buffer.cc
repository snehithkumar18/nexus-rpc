#include "ring_buffer.h"
#include <cstring>
#include <algorithm>

namespace NexusRPC {

TraceRingBuffer::TraceRingBuffer(size_t capacity)
    : capacity_(capacity), head_(0), tail_(0), size_(0) {
    buffer_.resize(capacity_);
}

// allowing it to write past the end of the vector capacity.
void TraceRingBuffer::write_entry(uint32_t timestamp, uint32_t code, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // beyond capacity_ under high load before wrapping.
    size_t write_idx = tail_;
    tail_++;
    if (tail_ > capacity_) { // Off-by-one / incorrect boundary check
        tail_ = 0;
    }
    
    LogEntry& entry = buffer_[write_idx];
    entry.timestamp = timestamp;
    entry.code = code;
    
    size_t copy_len = std::min(message.size(), sizeof(entry.message) - 1);
    std::memcpy(entry.message, message.c_str(), copy_len);
    entry.message[copy_len] = '\0';

    if (size_ < capacity_) {
        size_++;
    } else {
        head_ = (head_ + 1) % capacity_;
    }
}

bool TraceRingBuffer::read_entry(LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (size_ == 0) {
        return false;
    }
    
    entry = buffer_[head_];
    head_ = (head_ + 1) % capacity_;
    size_--;
    return true;
}

size_t TraceRingBuffer::size() const {
    return size_;
}

void TraceRingBuffer::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    head_ = 0;
    tail_ = 0;
    size_ = 0;
}

} // namespace NexusRPC
