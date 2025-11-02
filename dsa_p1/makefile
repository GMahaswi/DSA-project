# ==========================
# Makefile for RoutingSystem
# Phase-1 Build Configuration
# ==========================

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# Directories
PHASE1_DIR = Phase-1

# Files
PHASE1_SRC = $(wildcard $(PHASE1_DIR)/*.cpp)
PHASE1_OBJ = $(PHASE1_SRC:.cpp=.o)
TARGET1 = phase1

# ==========================
# Default target
# ==========================
all: $(TARGET1)

# Build phase1 executable
$(TARGET1): $(PHASE1_OBJ)
	$(CXX) $(CXXFLAGS) $(PHASE1_OBJ) -o $(TARGET1)

# Compile .cpp → .o
$(PHASE1_DIR)/%.o: $(PHASE1_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run with example JSONs
run: $(TARGET1)
	./$(TARGET1) graph.json queries.json

# Clean build files
clean:
	rm -f $(PHASE1_OBJ) $(TARGET1)

.PHONY: all clean run
