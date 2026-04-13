# DBMS Buffer Manager Simulation — Backend Makefile

CXX      := g++
CC       := gcc
CXXFLAGS := -Wall -Wextra -O2
CFLAGS   := -O2
LDFLAGS  := -lpthread

# C++ source files
SRCS := main.cpp \
        watcher/FileWatcher.cpp \
        database/SQLiteManager.cpp \
        buffer/LRUBuffer.cpp \
        buffer/MRUBuffer.cpp \
        buffer/ClockBuffer.cpp \
        buffer/PinnedBuffer.cpp

# SQLite amalgamation (compiled as C)
SQLITE_SRC := lib/sqlite3.c
SQLITE_OBJ := lib/sqlite3.o

# Object files
OBJS := $(SRCS:.cpp=.o)

# Output binary
TARGET := buffer_manager

# ── Targets ──────────────────────────────────────

.PHONY: all clean run dirs

all: dirs $(TARGET)

$(TARGET): $(OBJS) $(SQLITE_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile C++ sources
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile SQLite amalgamation as C
$(SQLITE_OBJ): $(SQLITE_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure input and output directories exist
dirs:
	@mkdir -p input output

run: all
	./$(TARGET)

clean:
	rm -f $(OBJS) $(SQLITE_OBJ) $(TARGET)
	rm -f output/metrics.txt
