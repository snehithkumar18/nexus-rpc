#include "../src/database.h"
#include "../src/logger.h"
#include "../src/wal.h"
#include "../src/lock_manager.h"
#include "../src/sql_parser.h"
#include "../src/query_planner.h"
#include <cstdint>
#include <cstddef>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <algorithm>
#include <cstdio>

namespace NexusRPC {

class FuzzDBShell {
private:
    std::unique_ptr<Database> db;
    std::unique_ptr<LogManager> log_manager;
    std::unique_ptr<LockManager> lock_manager;
    std::unique_ptr<QueryPlanner> planner;
    bool is_open = false;
    uint32_t active_tx_id = 0;
    bool in_transaction = false;
    std::string current_db_path;

    std::vector<std::string> tokenize(const std::string& line) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream iss(line);
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

    void execute_sql_select(const SQLSelectStatement& stmt) {
        if (!planner) return;
        auto executor = planner->plan_query(stmt);
        if (!executor) return;

        executor->init();
        Document doc;
        RecordID rid;
        while (executor->next(doc, rid)) {
            // Do nothing, just iterate
        }
        executor->close();
    }

    void execute_sql_insert(const SQLInsertStatement& stmt) {
        Document doc;
        for (size_t i = 0; i < stmt.fields.size(); ++i) {
            if (i < stmt.values.size()) {
                doc.set_field(stmt.fields[i], stmt.values[i]);
            }
        }

        if (in_transaction && log_manager) {
            log_manager->append_record(active_tx_id, LogRecordType::INSERT, 0, 0, {}, doc.serialize());
        }

        db->insert(stmt.key, doc);
    }

    void handle_sql(const std::string& sql) {
        SQLLexer lexer(sql);
        std::vector<Token> tokens = lexer.tokenize();
        if (tokens.empty() || tokens[0].type == TokenType::END_OF_FILE) return;

        SQLParser parser(tokens);
        auto stmt = parser.parse();
        if (!stmt) return;

        if (stmt->type == StatementType::SELECT) {
            execute_sql_select(*static_cast<SQLSelectStatement*>(stmt.get()));
        } else if (stmt->type == StatementType::INSERT) {
            execute_sql_insert(*static_cast<SQLInsertStatement*>(stmt.get()));
        }
    }

    void handle_tx(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) return;
        std::string cmd = tokens[1];
        if (cmd == "begin") {
            if (in_transaction) return;
            in_transaction = true;
            active_tx_id++;
            if (log_manager) {
                log_manager->append_record(active_tx_id, LogRecordType::BEGIN);
            }
        } else if (cmd == "commit") {
            if (!in_transaction) return;
            if (log_manager) {
                log_manager->append_record(active_tx_id, LogRecordType::COMMIT);
                log_manager->flush();
            }
            if (lock_manager) {
                lock_manager->release_all(active_tx_id);
            }
            in_transaction = false;
        } else if (cmd == "abort") {
            if (!in_transaction) return;
            if (log_manager) {
                log_manager->append_record(active_tx_id, LogRecordType::ABORT);
                log_manager->flush();
            }
            if (lock_manager) {
                lock_manager->release_all(active_tx_id);
            }
            in_transaction = false;
        }
    }

public:
    FuzzDBShell() = default;
    ~FuzzDBShell() { close_db(); }

    void open_db(const std::string& path) {
        db = std::make_unique<Database>();
        log_manager = std::make_unique<LogManager>(path + ".wal");
        lock_manager = std::make_unique<LockManager>();
        
        DBErrorCode res = db->open(path);
        if (res == DBErrorCode::SUCCESS) {
            is_open = true;
            current_db_path = path;
            planner = std::make_unique<QueryPlanner>(*db->get_disk_manager(), *db->get_cache_manager(), *db->get_index());
        }
    }

    void close_db() {
        if (is_open) {
            db->close();
            planner.reset();
            db.reset();
            log_manager.reset();
            lock_manager.reset();
            is_open = false;
        }
    }

    void execute_line(const std::string& line) {
        std::vector<std::string> tokens = tokenize(line);
        if (tokens.empty()) return;

        std::string cmd = tokens[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });

        if (cmd == "open") {
            if (tokens.size() >= 2) {
                open_db(tokens[1]);
            }
        } else if (cmd == "tx") {
            handle_tx(tokens);
        } else if (cmd == "close") {
            close_db();
        } else {
            if (is_open) {
                handle_sql(line);
            }
        }
    }
};

} // namespace NexusRPC

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size == 0) return 0;
    
    std::string input(reinterpret_cast<const char*>(data), size);
    std::stringstream ss(input);
    std::string line;
    
    NexusRPC::FuzzDBShell shell;
    shell.open_db("fuzz_test_db");
    
    while (std::getline(ss, line, '\n')) {
        if (!line.empty()) {
            shell.execute_line(line);
        }
    }
    
    shell.close_db();
    std::remove("fuzz_test_db");
    std::remove("fuzz_test_db.wal");
    return 0;
}
