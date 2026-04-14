#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "../buffer/BufferManager.hpp"
#include "../database/SQLiteManager.hpp"
#include "../models/Metrics.hpp"

namespace Helpers {

inline std::vector<int> generatePageSequence(int recordCount, int pageSize,
                                             int basePageId = 0) {
  if (recordCount <= 0 || pageSize <= 0)
    return {};

  int totalPages =
      static_cast<int>(std::ceil(static_cast<double>(recordCount) / pageSize));

  std::vector<int> sequence;
  sequence.reserve(totalPages);
  for (int i = 0; i < totalPages; ++i) {
    sequence.push_back(basePageId + i);
  }
  return sequence;
}

inline void ensureDirectory(const std::string &path) {
  struct stat st;
  if (stat(path.c_str(), &st) != 0) {
    mkdir(path.c_str(), 0755);
  }
}

inline void dumpMetricsToFile(const std::vector<BufferManager *> &managers,
                              const std::string &filepath) {

  std::string dir = filepath.substr(0, filepath.find_last_of('/'));
  if (!dir.empty()) {
    ensureDirectory(dir);
  }

  std::ofstream ofs(filepath, std::ios::out | std::ios::trunc);
  if (!ofs.is_open()) {
    return;
  }

  std::time_t now = std::time(nullptr);
  std::tm *lt = std::localtime(&now);
  char timeBuf[64];
  std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", lt);

  ofs << "========================================\n";
  ofs << "  BUFFER MANAGER METRICS REPORT\n";
  ofs << "  Generated on: " << timeBuf << "\n";
  ofs << "========================================\n\n";

  for (size_t i = 0; i < managers.size(); ++i) {
    Metrics m = managers[i]->getMetrics();

    ofs << "Strategy: " << m.strategyName << "\n";
    ofs << "  Total Requests : " << m.totalRequests << "\n";
    ofs << "  Buffer Hits    : " << m.bufferHits << "\n";
    ofs << "  Buffer Misses  : " << m.bufferMisses << "\n";
    ofs << "  Disk Reads     : " << m.diskReads << "\n";
    ofs << std::fixed << std::setprecision(4);
    ofs << "  Hit Ratio      : " << m.hitRatio() << "\n";
    ofs << "  Miss Ratio     : " << m.missRatio() << "\n";
    ofs << "  Evictions      : " << m.evictions << "\n";

    if (i + 1 < managers.size()) {
      ofs << "\n----------------------------------------\n\n";
    }
  }

  ofs << "\n========================================\n";
  ofs.close();
}

inline void appendQueryResultToFile(const std::string &sql,
                                    const QueryResult &result,
                                    const std::string &filepath,
                                    bool isFirstWrite) {

  std::string dir = filepath.substr(0, filepath.find_last_of('/'));
  if (!dir.empty()) {
    ensureDirectory(dir);
  }

  auto mode = std::ios::out;
  if (isFirstWrite) {
    mode |= std::ios::trunc;
  } else {
    mode |= std::ios::app;
  }

  std::ofstream ofs(filepath, mode);
  if (!ofs.is_open())
    return;

  std::time_t now = std::time(nullptr);
  std::tm *lt = std::localtime(&now);
  char timeBuf[64];
  std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", lt);

  ofs << "========================================\n";
  ofs << "  Query executed at: " << timeBuf << "\n";
  ofs << "  SQL: " << sql << "\n";
  ofs << "  Records: " << result.recordCount() << "\n";
  ofs << "========================================\n\n";

  if (result.rows.empty()) {
    ofs << "(No results)\n\n";
    ofs.close();
    return;
  }

  int numCols = static_cast<int>(result.columns.size());
  std::vector<size_t> widths(numCols, 0);

  for (int c = 0; c < numCols; ++c) {
    widths[c] = result.columns[c].size();
  }
  for (const auto &row : result.rows) {
    for (int c = 0; c < numCols && c < static_cast<int>(row.size()); ++c) {
      widths[c] = std::max(widths[c], row[c].size());
    }
  }

  for (int c = 0; c < numCols; ++c) {
    ofs << std::left << std::setw(static_cast<int>(widths[c]) + 2)
        << result.columns[c];
  }
  ofs << "\n";

  for (int c = 0; c < numCols; ++c) {
    ofs << std::string(widths[c], '-') << "  ";
  }
  ofs << "\n";

  for (const auto &row : result.rows) {
    for (int c = 0; c < numCols && c < static_cast<int>(row.size()); ++c) {
      ofs << std::left << std::setw(static_cast<int>(widths[c]) + 2) << row[c];
    }
    ofs << "\n";
  }

  ofs << "\n";
  ofs.close();
}

} // namespace Helpers

#endif
