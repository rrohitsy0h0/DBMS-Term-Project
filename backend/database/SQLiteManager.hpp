#ifndef SQLITE_MANAGER_HPP
#define SQLITE_MANAGER_HPP

#include <string>
#include <vector>
#include "../lib/sqlite3.h"

// Manages SQLite database connection and query execution
class SQLiteManager {
public:
    explicit SQLiteManager(const std::string& dbPath);
    ~SQLiteManager();

    // Execute a SQL query and return the number of records accessed
    int executeQuery(const std::string& sql);

    // Check if the database connection is valid
    bool isOpen() const;

private:
    sqlite3* db_;
    bool isOpen_;
};

#endif // SQLITE_MANAGER_HPP
