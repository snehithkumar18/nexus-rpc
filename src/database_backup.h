#ifndef NEXUS_RPC_DATABASE_BACKUP_H
#define NEXUS_RPC_DATABASE_BACKUP_H

#include "database.h"
#include <string>

namespace NexusRPC {

class DatabaseBackup {
private:
    Database* db;

public:
    explicit DatabaseBackup(Database* target_db);
    ~DatabaseBackup() = default;

    // Performs online backup of the database pages to a target file
    DBErrorCode create_backup(const std::string& backup_filepath);

    // Restores the database pages from a target file
    DBErrorCode restore_backup(const std::string& backup_filepath);
};

} // namespace NexusRPC

#endif // NEXUS_RPC_DATABASE_BACKUP_H
