#ifndef NEXUS_RPC_QUERY_ENGINE_COMPILER_PARTITION_OPTIMIZER_H
#define NEXUS_RPC_QUERY_ENGINE_COMPILER_PARTITION_OPTIMIZER_H

#include "query_engine_compiler_partition.h"
#include <string>
#include <vector>

namespace NexusRPC {

class PartitionOptimizer {
public:
    PartitionOptimizer() = default;
    ~PartitionOptimizer() = default;

    // Prune partitions based on compound query filters (AND/OR ranges)
    std::vector<std::string> prune_partitions_optimized(const PartitionManager& pm, const QueryNode& filter);
};

} // namespace NexusRPC

#endif // NEXUS_RPC_QUERY_ENGINE_COMPILER_PARTITION_OPTIMIZER_H
