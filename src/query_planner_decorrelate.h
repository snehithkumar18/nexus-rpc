#ifndef NEXUS_RPC_QUERY_PLANNER_DECORRELATE_H
#define NEXUS_RPC_QUERY_PLANNER_DECORRELATE_H

#include "query_planner.h"
#include <memory>
#include <string>
#include <vector>

namespace NexusRPC {

class SubqueryDecorrelator {
private:
    bool detect_correlation(const LogicalPlanNode* subquery, const std::string& outer_table);

public:
    SubqueryDecorrelator() = default;
    ~SubqueryDecorrelator() = default;

    // Decorrelate correlated filters to standard Joins
    std::unique_ptr<LogicalPlanNode> decorrelate(std::unique_ptr<LogicalPlanNode> plan);
};

} // namespace NexusRPC

#endif // NEXUS_RPC_QUERY_PLANNER_DECORRELATE_H
