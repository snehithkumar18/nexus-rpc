#ifndef NEXUS_RPC_ROUTING_ENGINE_H
#define NEXUS_RPC_ROUTING_ENGINE_H

#include "payload.h"
#include <string>
#include <vector>
#include <memory>

namespace NexusRPC {

enum class QueryOp : uint8_t {
    EQ = 0,
    NEQ = 1,
    GT = 2,
    LT = 3,
    GTE = 4,
    LTE = 5,
    LIKE = 6,
    IN = 7
};

struct QueryNode {
    std::string field;
    QueryOp op;
    Payload value;
};

class RoutingEngine {
private:
    std::vector<QueryNode> filters_;

    // Detailed parser helper methods to build up query grammar.
    bool parse_filter(const std::string& filter_str, QueryNode& node);
    QueryOp string_to_op(const std::string& op_str) const;
    Payload parse_value(const std::string& val_str) const;

public:
    RoutingEngine() = default;
    ~RoutingEngine() = default;

    // Parses a routing query statement (e.g., "WHERE temperature > 25 AND topic LIKE 'sensor/%'")
    bool compile_query(const std::string& query_str);
    
    // Evaluates a published payload against compiled filters
    bool evaluate(const Payload& document) const;
    
    const std::vector<QueryNode>& get_filters() const { return filters_; }
    void clear() { filters_.clear(); }
};

} // namespace NexusRPC

#endif // NEXUS_RPC_ROUTING_ENGINE_H
