#ifndef NEXUS_RPC_CACHE_H
#define NEXUS_RPC_CACHE_H

#include <unordered_map>
#include <vector>
#include <string>
#include "storage.h"
#include "errors.h"

namespace NexusRPC {

class BufferPoolManager {
private:
    size_t pool_size;
    DiskManager& disk_manager;
    std::unordered_map<uint32_t, Page*> page_directory;
    std::vector<uint32_t> lru_queue;


public:
    BufferPoolManager(size_t size, DiskManager& disk_mgr);
    ~BufferPoolManager();

    Page* fetch_page(uint32_t page_id);
    DBErrorCode flush_page(uint32_t page_id);
    void flush_all();
    void clear();

    size_t get_size() const { return page_directory.size(); }
};

} // namespace NexusRPC

#endif // NEXUS_RPC_CACHE_H
