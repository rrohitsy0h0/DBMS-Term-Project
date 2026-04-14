#ifndef SQLITE_MANAGER_HPP
#define SQLITE_MANAGER_HPP

#include "../lib/sqlite3.h"
#include <string>
#include <unordered_map>
#include <vector>

struct QueryResult {
  std::vector<std::string> columns;
  std::vector<std::vector<std::string>> rows;
  int recordCount() const { return static_cast<int>(rows.size()); }
};

class SQLiteManager {
public:
  explicit SQLiteManager(const std::string &dbPath);
  ~SQLiteManager();

  int executeQuery(const std::string &sql);

  QueryResult executeQueryWithResults(const std::string &sql);

  bool isOpen() const;

  void initPageOffsets(int pageSize);

  int getBasePageId(const std::string &tableName) const;

  static std::string extractTableName(const std::string &sql);

  const std::unordered_map<std::string, int> &getPageOffsets() const;

private:
  sqlite3 *db_;
  bool isOpen_;

  std::unordered_map<std::string, int> tablePageOffsets_;
};

#endif
