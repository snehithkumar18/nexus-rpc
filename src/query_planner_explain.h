#ifndef NEXUS_RPC_QUERY_PLANNER_EXPLAIN_H
#define NEXUS_RPC_QUERY_PLANNER_EXPLAIN_H

#include "query_planner.h"
#include "optimizer_cbo.h"
#include <string>
#include <vector>

namespace NexusRPC {

class QueryPlanExplainer {
private:
    void explain_logical_node(const LogicalPlanNode* node, int depth, std::string& output) const;
    void explain_physical_node(const PlanNode& node, int depth, std::string& output) const;

public:
    QueryPlanExplainer() = default;
    ~QueryPlanExplainer() = default;

    std::string explain_logical(const LogicalPlanNode* root) const;
    std::string explain_physical(const PlanNode& root) const;
};

} // namespace NexusRPC

#endif // NEXUS_RPC_QUERY_PLANNER_EXPLAIN_H
