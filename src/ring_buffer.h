#ifndef NEXUS_RPC_RING_BUFFER_H
#define NEXUS_RPC_RING_BUFFER_H

#include <string>
#include <vector>
#include <mutex>

namespace NexusRPC {

struct LogEntry {
    uint32_t timestamp;
    uint32_t code;
    char message[64];
};

class TraceRingBuffer {
private:
    std::vector<LogEntry> buffer_;
    size_t capacity_;
    size_t head_;
    size_t tail_;
    size_t size_;
    std::mutex mutex_;

public:
    explicit TraceRingBuffer(size_t capacity);
    ~TraceRingBuffer() = default;

    // INJECTED BUG 5 (Heap Buffer Overflow): The wrap-around logic 
    // in write_entry has a bounds calculation error that can write 
    // past the allocated vector buffer capacity.
    void write_entry(uint32_t timestamp, uint32_t code, const std::string& message);
    
    bool read_entry(LogEntry& entry);
    size_t size() const;
    void clear();
};

} // namespace NexusRPC

#endif // NEXUS_RPC_RING_BUFFER_H
