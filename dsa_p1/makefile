CXX = g++
CXXFLAGS = -std=c++17 -IPhase-1/nlohmann

SRC_DIR = Phase-1
OBJ_DIR = Phase-1
TARGET = phase1

OBJS = $(OBJ_DIR)/graph.o $(OBJ_DIR)/queries.o $(OBJ_DIR)/main.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
