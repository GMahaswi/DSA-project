# ===============================
# Makefile for Graph Routing Project
# ===============================

CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Include path for nlohmann JSON (if present inside Phase-1/nlohmann etc.)
INCLUDES = -IPhase-1/nlohmann -IPhase-2/nlohmann -IPhase-3/nlohmann


SRC_DIR3 = Phase-3
OBJ_DIR3 = Phase-3
TARGET3 = phase3

OBJS3 = $(OBJ_DIR3)/graph.o $(OBJ_DIR3)/DeliveryScheduler.o $(OBJ_DIR3)/queries.o $(OBJ_DIR3)/main.o

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
