# Compiler and Flags
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# File Names
TARGET = yora
SRC = src/yora.cpp src/editor.cpp src/keymap.cpp src/path.cpp

# Build Rule (The default)
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean Rule (Deletes the executable)
clean:
	rm -f $(TARGET)
# Install the file to PATH
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/$(TARGET)

# Uninstall: Removes the file
uninstall:
	rm -f /usr/local/bin/$(TARGET)
