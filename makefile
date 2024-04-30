# Compiler and linker configurations
CXX = g++
CXXFLAGS = -std=c++11 -I/usr/include/SDL2 -Wall -Wextra -std=c++17
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lSDL2 -ldl -lSDL2_ttf -lSDL2_gfx

# Source and header files
CPP_SOURCES = $(shell find src -name '*.cpp')
HEADERS = $(shell find incl -name '*.h')

# Object files and directories
BUILD_DIR = build
DIST_DIR = dist

OBJECTS = $(CPP_SOURCES:src/%.cpp=$(BUILD_DIR)/%.o)
EXECUTABLE = $(DIST_DIR)/MyApp

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(DIST_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile source files to object files
$(BUILD_DIR)/%.o: src/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
