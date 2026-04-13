#include "SQLiteManager.hpp"
#include "../utils/Logger.hpp"
#include <sstream>

SQLiteManager::SQLiteManager(const std::string& dbPath) : db_(nullptr), isOpen_(false) {
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        Logger::error("Failed to open database: " + std::string(sqlite3_errmsg(db_)));
        sqlite3_close(db_);
        db_ = nullptr;
    } else {
        isOpen_ = true;
        Logger::info("Database opened: " + dbPath);
    }
}

SQLiteManager::~SQLiteManager() {
    if (db_) {
        sqlite3_close(db_);
        Logger::info("Database connection closed.");
    }
}

bool SQLiteManager::isOpen() const {
    return isOpen_;
}

int SQLiteManager::executeQuery(const std::string& sql) {
    if (!isOpen_ || !db_) {
        Logger::error("Database is not open.");
        return 0;
    }

    int recordCount = 0;
    char* errMsg = nullptr;

    // Callback: count each row returned
    auto callback = [](void* data, int /*argc*/, char** /*argv*/, char** /*colNames*/) -> int {
        int* count = static_cast<int*>(data);
        (*count)++;
        return 0;
    };

    int rc = sqlite3_exec(db_, sql.c_str(), callback, &recordCount, &errMsg);

    if (rc != SQLITE_OK) {
        Logger::error("SQL error: " + std::string(errMsg));
        sqlite3_free(errMsg);
        return 0;
    }

    Logger::info("Query executed. Records accessed: " + std::to_string(recordCount));
    return recordCount;
}
