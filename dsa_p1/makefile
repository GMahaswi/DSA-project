# ===============================
# Makefile for Graph Routing Project
# ===============================

CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Include path for nlohmann JSON (if present inside Phase-1/nlohmann etc.)
INCLUDES = -IPhase-1/nlohmann -IPhase-2/nlohmann -IPhase-3/nlohmann

# ===============================
# Phase 1
# ===============================
SRC_DIR1 = Phase-1
OBJ_DIR1 = Phase-1
TARGET1 = phase1

OBJS1 = $(OBJ_DIR1)/graph.o $(OBJ_DIR1)/knn.o $(OBJ_DIR1)/queries.o $(OBJ_DIR1)/main.o

$(TARGET1): $(OBJS1)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET1) $(OBJS1)

$(OBJ_DIR1)/%.o: $(SRC_DIR1)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ===============================
# Phase 2
# ===============================
SRC_DIR2 = Phase-2
OBJ_DIR2 = Phase-2
TARGET2 = phase2

OBJS2 = $(OBJ_DIR2)/graph.o $(OBJ_DIR2)/queries.o $(OBJ_DIR2)/main.o

$(TARGET2): $(OBJS2)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET2) $(OBJS2)

$(OBJ_DIR2)/%.o: $(SRC_DIR2)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ===============================
# Phase 3
# ===============================
SRC_DIR3 = Phase-3
OBJ_DIR3 = Phase-3
TARGET3 = phase3

OBJS3 = $(OBJ_DIR3)/graph.o $(OBJ_DIR3)/queries.o $(OBJ_DIR3)/main.o

$(TARGET3): $(OBJS3)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET3) $(OBJS3)

$(OBJ_DIR3)/%.o: $(SRC_DIR3)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ===============================
# Utility Targets
# ===============================

all: $(TARGET1) $(TARGET2) $(TARGET3)

clean:
	rm -f $(OBJ_DIR1)/*.o $(OBJ_DIR2)/*.o $(OBJ_DIR3)/*.o $(TARGET1) $(TARGET2) $(TARGET3)

run_phase1:
	./$(TARGET1) graph.json queries.json output.json

run_phase2:
	./$(TARGET2) graph.json queries.json output.json

run_phase3:
	./$(TARGET3) graph.json queries.json output.json

.PHONY: all clean run_phase1 run_phase2 run_phase3
