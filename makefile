# Compiler and Flags
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# File Names
TARGET = yora
SRC = yora.cpp raw.cpp

# Build Rule (The default)
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean Rule (Deletes the executable)
clean:
	rm -f $(TARGET)
