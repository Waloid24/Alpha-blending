CXX        = g++
CXXFLAGS   = -mavx2 -msse3
SOURCES    = Main.cpp Alpha-blend.cpp
SFML	   = -lsfml-graphics -lsfml-window -lsfml-system
OBJECTS    = $(SOURCES:.cpp=.o)
EXECUTABLE = blnd

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(SFML)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY: clear
clear:
	rm -rf $(OBJECTS) $(EXECUTABLE)

clrObj:
	rm -rf $(OBJECTS)
