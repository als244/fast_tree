# Define variables for compiler, flags, and source files
CXX = g++
CXXFLAGS = -Wall -Wextra -O3
SRCS = testSet.cpp

# Define the target executable
TARGET = testSet

# Default rule to build the executable
all: $(TARGET)

# Rule to build the executable from object files
testSet: testSet.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# Rule to compile object files from source files
testSet.o: testSet.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up the build directory
clean:
	rm -f $(TARGET) *.o