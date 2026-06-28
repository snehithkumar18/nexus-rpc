#ifndef NEXUS_RPC_STORAGE_ENGINE_H
#define NEXUS_RPC_STORAGE_ENGINE_H

#include "payload.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace NexusRPC {

struct Record {
    uint64_t id;
    std::string topic;
    Payload data;
    uint64_t timestamp;
};

class StorageEngine {
private:
    std::vector<Record> database_;
    std::unordered_map<std::string, std::vector<uint64_t>> index_by_topic_;
    uint64_t next_id_;
    std::mutex mutex_;

    void rebuild_indexes();

public:
    StorageEngine();
    ~StorageEngine() = default;

    // Writes a new record to the database
    uint64_t insert_record(const std::string& topic, const Payload& data);
    
    // Reads a record by its unique ID
    bool get_record_by_id(uint64_t id, Record& out_record);
    
    // Searches records under a specific topic
    std::vector<Record> get_records_by_topic(const std::string& topic);
    
    // Deletes records older than a specific timestamp threshold
    size_t purge_old_records(uint64_t max_age_ms);
    
    size_t size() const;
    void clear();
};

} // namespace NexusRPC

#endif // NEXUS_RPC_STORAGE_ENGINE_H
