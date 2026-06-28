#include "routing_engine.h"
#include <sstream>
#include <algorithm>

namespace NexusRPC {

// Clean leading/trailing whitespace
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

QueryOp RoutingEngine::string_to_op(const std::string& op_str) const {
    std::string op = trim(op_str);
    if (op == "=" || op == "==") return QueryOp::EQ;
    if (op == "!=") return QueryOp::NEQ;
    if (op == ">") return QueryOp::GT;
    if (op == "<") return QueryOp::LT;
    if (op == ">=") return QueryOp::GTE;
    if (op == "<=") return QueryOp::LTE;
    if (op == "LIKE" || op == "like") return QueryOp::LIKE;
    if (op == "IN" || op == "in") return QueryOp::IN;
    return QueryOp::EQ;
}

Payload RoutingEngine::parse_value(const std::string& val_str) const {
    std::string val = trim(val_str);
    if (val.empty()) return Payload();

    // String literal
    if (val.front() == '\'' && val.back() == '\'') {
        return Payload(val.substr(1, val.size() - 2));
    }
    if (val.front() == '"' && val.back() == '"') {
        return Payload(val.substr(1, val.size() - 2));
    }

    // Boolean
    if (val == "true" || val == "TRUE") return Payload(true);
    if (val == "false" || val == "FALSE") return Payload(false);

    // Number
    try {
        size_t idx;
        int32_t num = std::stoi(val, &idx);
        if (idx == val.size()) {
            return Payload(num);
        }
    } catch (...) {}

    return Payload(val); // Default to string if parsing fails
}

bool RoutingEngine::parse_filter(const std::string& filter_str, QueryNode& node) {
    std::vector<std::string> operators = {"!=", ">=", "<=", "=", ">", "<", "LIKE", "like", "IN", "in"};
    std::string matched_op = "";
    size_t op_pos = std::string::npos;

    for (const auto& op : operators) {
        op_pos = filter_str.find(op);
        if (op_pos != std::string::npos) {
            matched_op = op;
            break;
        }
    }

    if (op_pos == std::string::npos) return false;

    node.field = trim(filter_str.substr(0, op_pos));
    node.op = string_to_op(matched_op);
    node.value = parse_value(filter_str.substr(op_pos + matched_op.size()));

    return !node.field.empty();
}

bool RoutingEngine::compile_query(const std::string& query_str) {
    filters_.clear();
    std::string working = trim(query_str);
    if (working.empty()) return true;

    // Strip "WHERE " prefix if present
    if (working.size() > 6 && working.substr(0, 6) == "WHERE ") {
        working = working.substr(6);
    } else if (working.size() > 6 && working.substr(0, 6) == "where ") {
        working = working.substr(6);
    }

    std::stringstream ss(working);
    std::string token;
    
    // Split by " AND " or " and "
    while (std::getline(ss, token, '&')) { // Simplified parse using '&' for splitting filters
        QueryNode node;
        if (parse_filter(token, node)) {
            filters_.push_back(std::move(node));
        } else {
            return false;
        }
    }

    return !filters_.empty();
}

bool RoutingEngine::evaluate(const Payload& document) const {
    if (document.type != PayloadType::MAP) return false;
    auto doc_map = document.get_map();

    for (const auto& filter : filters_) {
        auto it = doc_map.find(filter.field);
        if (it == doc_map.end()) return false; // Field missing

        const Payload& doc_val = it->second;
        
        // Match operation
        if (filter.op == QueryOp::EQ) {
            if (doc_val != filter.value) return false;
        } else if (filter.op == QueryOp::NEQ) {
            if (doc_val == filter.value) return false;
        } else if (filter.op == QueryOp::GT) {
            if (doc_val.type == PayloadType::INT && filter.value.type == PayloadType::INT) {
                if (doc_val.get_int() <= filter.value.get_int()) return false;
            } else if (doc_val.type == PayloadType::STRING && filter.value.type == PayloadType::STRING) {
                if (doc_val.get_string() <= filter.value.get_string()) return false;
            } else {
                return false;
            }
        } else if (filter.op == QueryOp::LT) {
            if (doc_val.type == PayloadType::INT && filter.value.type == PayloadType::INT) {
                if (doc_val.get_int() >= filter.value.get_int()) return false;
            } else if (doc_val.type == PayloadType::STRING && filter.value.type == PayloadType::STRING) {
                if (doc_val.get_string() >= filter.value.get_string()) return false;
            } else {
                return false;
            }
        } else if (filter.op == QueryOp::GTE) {
            if (doc_val.type == PayloadType::INT && filter.value.type == PayloadType::INT) {
                if (doc_val.get_int() < filter.value.get_int()) return false;
            } else if (doc_val.type == PayloadType::STRING && filter.value.type == PayloadType::STRING) {
                if (doc_val.get_string() < filter.value.get_string()) return false;
            } else {
                return false;
            }
        } else if (filter.op == QueryOp::LTE) {
            if (doc_val.type == PayloadType::INT && filter.value.type == PayloadType::INT) {
                if (doc_val.get_int() > filter.value.get_int()) return false;
            } else if (doc_val.type == PayloadType::STRING && filter.value.type == PayloadType::STRING) {
                if (doc_val.get_string() > filter.value.get_string()) return false;
            } else {
                return false;
            }
        }
    }

    return true;
}

} // namespace NexusRPC
