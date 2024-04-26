# Compiler and linker configurations
CXX = g++
CXXFLAGS = -std=c++11 -I/usr/include/SDL2 -Wall -Wextra -std=c++17
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lSDL2 -ldl -lSDL2_ttf

# Project files
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = MyApp

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
