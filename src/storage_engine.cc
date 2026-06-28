#include "storage_engine.h"
#include <chrono>

namespace NexusRPC {

StorageEngine::StorageEngine() : next_id_(1) {}

uint64_t StorageEngine::insert_record(const std::string& topic, const Payload& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    Record record;
    record.id = next_id_++;
    record.topic = topic;
    record.data = data;
    
    auto now = std::chrono::steady_clock::now();
    record.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    database_.push_back(record);
    index_by_topic_[topic].push_back(record.id);
    
    return record.id;
}

bool StorageEngine::get_record_by_id(uint64_t id, Record& out_record) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Linear scan for simplicity and correctness
    for (const auto& rec : database_) {
        if (rec.id == id) {
            out_record = rec;
            return true;
        }
    }
    return false;
}

std::vector<Record> StorageEngine::get_records_by_topic(const std::string& topic) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<Record> results;
    auto it = index_by_topic_.find(topic);
    if (it != index_by_topic_.end()) {
        for (uint64_t id : it->second) {
            for (const auto& rec : database_) {
                if (rec.id == id) {
                    results.push_back(rec);
                    break;
                }
            }
        }
    }
    return results;
}

void StorageEngine::rebuild_indexes() {
    index_by_topic_.clear();
    for (const auto& rec : database_) {
        index_by_topic_[rec.topic].push_back(rec.id);
    }
}

size_t StorageEngine::purge_old_records(uint64_t max_age_ms) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::steady_clock::now();
    uint64_t current_time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    auto original_size = database_.size();
    
    database_.erase(
        std::remove_if(database_.begin(), database_.end(), [current_time, max_age_ms](const Record& rec) {
            return (current_time - rec.timestamp) > max_age_ms;
        }),
        database_.end()
    );
    
    if (database_.size() != original_size) {
        rebuild_indexes();
    }
    
    return original_size - database_.size();
}

size_t StorageEngine::size() const {
    return database_.size();
}

void StorageEngine::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    database_.clear();
    index_by_topic_.clear();
    next_id_ = 1;
}

} // namespace NexusRPC
