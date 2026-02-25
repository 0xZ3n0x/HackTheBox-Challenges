# Compiler settings
CXX      := g++
CXXFLAGS := -O3 -Wall -Wextra -std=c++17
LDFLAGS  := -lpthread

# Target executable name
TARGET   := solver

# Source and Object files
SRCS     := main.cpp
OBJS     := $(SRCS:.cpp=.o)

# Default IP and Port (can be overridden via command line)
IP       ?= 127.0.0.1
PORT     ?= 1337

# Default rule: Compile the target
all: $(TARGET)

# Linking the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compiling source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean the build directory
clean:
	rm -f $(OBJS) $(TARGET)

# Rule to compile and run the solver
run: all
	./$(TARGET) $(IP) $(PORT)

.PHONY: all clean run
