#ifndef SQLITE_MANAGER_HPP
#define SQLITE_MANAGER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "../lib/sqlite3.h"

// Holds the full result of a query: column names + row data
struct QueryResult {
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;
    int recordCount() const { return static_cast<int>(rows.size()); }
};

// Manages SQLite database connection and query execution
class SQLiteManager {
public:
    explicit SQLiteManager(const std::string& dbPath);
    ~SQLiteManager();

    // Execute a SQL query and return the number of records accessed
    int executeQuery(const std::string& sql);

    // Execute a SQL query and return full results (columns + rows)
    QueryResult executeQueryWithResults(const std::string& sql);

    // Check if the database connection is valid
    bool isOpen() const;

    // Initialize page offset registry — assigns unique page ranges per table.
    // Must be called after database is opened. Uses pageSize to compute page counts.
    void initPageOffsets(int pageSize);

    // Get the base page ID for a given table name.
    // Returns 0 if the table is unknown.
    int getBasePageId(const std::string& tableName) const;

    // Extract the primary table name from a SQL query string.
    // Handles SELECT ... FROM <table> ..., JOINs use the first table.
    static std::string extractTableName(const std::string& sql);

    // Get the full page offset map (for logging)
    const std::unordered_map<std::string, int>& getPageOffsets() const;

private:
    sqlite3* db_;
    bool isOpen_;

    // Maps table name → base page ID (globally unique, contiguous)
    std::unordered_map<std::string, int> tablePageOffsets_;
};

#endif // SQLITE_MANAGER_HPP
