#include "SQLiteManager.hpp"
#include "../utils/Logger.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>

SQLiteManager::SQLiteManager(const std::string &dbPath)
    : db_(nullptr), isOpen_(false) {
  int rc = sqlite3_open(dbPath.c_str(), &db_);
  if (rc != SQLITE_OK) {
    Logger::error("Failed to open database: " +
                  std::string(sqlite3_errmsg(db_)));
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

bool SQLiteManager::isOpen() const { return isOpen_; }

int SQLiteManager::executeQuery(const std::string &sql) {
  if (!isOpen_ || !db_) {
    Logger::error("Database is not open.");
    return 0;
  }

  int recordCount = 0;
  char *errMsg = nullptr;

  auto callback = [](void *data, int /*argc*/, char ** /*argv*/,
                     char ** /*colNames*/) -> int {
    int *count = static_cast<int *>(data);
    (*count)++;
    return 0;
  };

  int rc = sqlite3_exec(db_, sql.c_str(), callback, &recordCount, &errMsg);

  if (rc != SQLITE_OK) {
    Logger::error("SQL error: " + std::string(errMsg));
    sqlite3_free(errMsg);
    return 0;
  }

  Logger::info("Query executed. Records accessed: " +
               std::to_string(recordCount));
  return recordCount;
}

QueryResult SQLiteManager::executeQueryWithResults(const std::string &sql) {
  QueryResult result;

  if (!isOpen_ || !db_) {
    Logger::error("Database is not open.");
    return result;
  }

  struct CallbackData {
    QueryResult *result;
    bool columnsSet;
  };

  CallbackData cbData = {&result, false};
  char *errMsg = nullptr;

  auto callback = [](void *data, int argc, char **argv,
                     char **colNames) -> int {
    CallbackData *cb = static_cast<CallbackData *>(data);

    if (!cb->columnsSet) {
      for (int i = 0; i < argc; ++i) {
        cb->result->columns.push_back(colNames[i] ? colNames[i] : "");
      }
      cb->columnsSet = true;
    }

    std::vector<std::string> row;
    for (int i = 0; i < argc; ++i) {
      row.push_back(argv[i] ? argv[i] : "NULL");
    }
    cb->result->rows.push_back(std::move(row));

    return 0;
  };

  int rc = sqlite3_exec(db_, sql.c_str(), callback, &cbData, &errMsg);

  if (rc != SQLITE_OK) {
    Logger::error("SQL error: " + std::string(errMsg));
    sqlite3_free(errMsg);
    return result;
  }

  Logger::info("Query executed. Records accessed: " +
               std::to_string(result.recordCount()));
  return result;
}

void SQLiteManager::initPageOffsets(int pageSize) {
  if (!isOpen_ || !db_)
    return;

  tablePageOffsets_.clear();

  std::vector<std::string> tableNames;
  char *errMsg = nullptr;

  auto tableCallback = [](void *data, int argc, char **argv,
                          char ** /*colNames*/) -> int {
    auto *names = static_cast<std::vector<std::string> *>(data);
    if (argc > 0 && argv[0]) {
      names->push_back(argv[0]);
    }
    return 0;
  };

  int rc = sqlite3_exec(
      db_, "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;",
      tableCallback, &tableNames, &errMsg);

  if (rc != SQLITE_OK) {
    Logger::error("Failed to read tables: " + std::string(errMsg));
    sqlite3_free(errMsg);
    return;
  }

  int nextPageId = 0;

  for (const auto &table : tableNames) {
    int rowCount = 0;

    auto countCallback = [](void *data, int argc, char **argv,
                            char ** /*colNames*/) -> int {
      if (argc > 0 && argv[0]) {
        *static_cast<int *>(data) = std::atoi(argv[0]);
      }
      return 0;
    };

    std::string countSql = "SELECT COUNT(*) FROM " + table + ";";
    rc = sqlite3_exec(db_, countSql.c_str(), countCallback, &rowCount, &errMsg);

    if (rc != SQLITE_OK) {
      Logger::error("Failed to count rows in " + table + ": " +
                    std::string(errMsg));
      sqlite3_free(errMsg);
      continue;
    }

    int numPages =
        static_cast<int>(std::ceil(static_cast<double>(rowCount) / pageSize));
    if (numPages == 0)
      numPages = 1;

    tablePageOffsets_[table] = nextPageId;

    Logger::info("  Table '" + table + "': " + std::to_string(rowCount) +
                 " rows → pages [" + std::to_string(nextPageId) + " .. " +
                 std::to_string(nextPageId + numPages - 1) + "]");

    nextPageId += numPages;
  }

  Logger::info(
      "Page offset registry initialized. Total pages across all tables: " +
      std::to_string(nextPageId));
}

int SQLiteManager::getBasePageId(const std::string &tableName) const {

  std::string lower = tableName;
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  for (const auto &pair : tablePageOffsets_) {
    std::string key = pair.first;
    std::transform(key.begin(), key.end(), key.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (key == lower) {
      return pair.second;
    }
  }

  Logger::warn("Unknown table '" + tableName + "', using base page 0.");
  return 0;
}

std::string SQLiteManager::extractTableName(const std::string &sql) {

  std::string lower = sql;
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  std::string::size_type pos = lower.find("from");
  if (pos == std::string::npos) {
    return "";
  }

  pos += 4;
  while (pos < lower.size() && std::isspace(lower[pos])) {
    ++pos;
  }

  std::string::size_type start = pos;
  while (pos < lower.size() && !std::isspace(lower[pos]) && lower[pos] != ',' &&
         lower[pos] != ';' && lower[pos] != ')' && lower[pos] != '(') {
    ++pos;
  }

  return sql.substr(start, pos - start);
}

const std::unordered_map<std::string, int> &
SQLiteManager::getPageOffsets() const {
  return tablePageOffsets_;
}
