# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Directories
SRC_DIR = src
INC_DIR = include

# Include paths
INCLUDES = -I$(INC_DIR)

# Project files
TARGET = server
SRCS = $(SRC_DIR)/server.cpp $(SRC_DIR)/main.cpp
OBJS = $(SRCS:.cpp=.o)

# Default rule to build the executable
all: $(TARGET)

# 1. Links the object files into the final executable
# 2. Immediately removes the object files so your folders stay clean
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
	rm -f $(OBJS)

# Rule to compile .cpp files into .o object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/server.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Rule to clean up the executable (since the .o files are already gone)
clean:
	rm -f $(TARGET)

.PHONY: all clean