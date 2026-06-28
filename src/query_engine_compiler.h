#ifndef NEXUS_RPC_QUERY_ENGINE_COMPILER_H
#define NEXUS_RPC_QUERY_ENGINE_COMPILER_H

#include "sql_parser.h"
#include "optimizer.h"
#include <string>
#include <vector>

namespace NexusRPC {

struct CompilerWarning {
    std::string message;
    std::string column;
};

class QueryEngineCompiler {
private:
    QueryOptimizer& optimizer;
    std::vector<CompilerWarning> warnings;

public:
    explicit QueryEngineCompiler(QueryOptimizer& opt);
    ~QueryEngineCompiler() = default;

    void optimize_select_statement(SQLSelectStatement& stmt);
    std::string explain_select_plan(const SQLSelectStatement& stmt);
    std::vector<CompilerWarning> get_warnings() const { return warnings; }
    void clear_warnings() { warnings.clear(); }
};

} // namespace NexusRPC

#endif // NEXUS_RPC_QUERY_ENGINE_COMPILER_H
