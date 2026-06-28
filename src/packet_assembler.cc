#include "packet_assembler.h"
#include <cstring>
#include <algorithm>

namespace NexusRPC {

bool PacketAssembler::add_fragment(uint32_t transaction_id, uint16_t sequence_num, 
                                  uint16_t total_fragments, uint32_t total_size, 
                                  const uint8_t* payload_data, uint16_t payload_len) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    Fragment frag;
    frag.sequence_num = sequence_num;
    frag.total_fragments = total_fragments;
    frag.total_size = total_size;
    frag.payload.assign(payload_data, payload_data + payload_len);

    auto& list = message_buffer_[transaction_id];
    list.push_back(std::move(frag));

    return list.size() == total_fragments;
}

// The total size is determined by the header of the first fragment.
// We allocate a fixed-size stack buffer but copy all fragment payloads 
// into it based on the header's total_size, leading to a stack overflow.
std::vector<uint8_t> PacketAssembler::assemble_packet(uint32_t transaction_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = message_buffer_.find(transaction_id);
    if (it == message_buffer_.end() || it->second.empty()) {
        return {};
    }

    auto& fragments = it->second;
    
    // Sort fragments by sequence number
    std::sort(fragments.begin(), fragments.end(), [](const Fragment& a, const Fragment& b) {
        return a.sequence_num < b.sequence_num;
    });

    uint32_t total_size = fragments[0].total_size;

    uint8_t stack_buffer[2048];
    uint32_t bytes_written = 0;

    for (const auto& frag : fragments) {
        // Copy fragment payload directly onto the stack
        std::memcpy(stack_buffer + bytes_written, frag.payload.data(), frag.payload.size());
        bytes_written += frag.payload.size();
    }

    
    message_buffer_.erase(it);
    return result;
}

void PacketAssembler::clear_transaction(uint32_t transaction_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    message_buffer_.erase(transaction_id);
}

} // namespace NexusRPC
