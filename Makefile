CXX        = g++
CXXFLAGS   = -O3 -mavx2 -msse3
SOURCES    = Main.cpp Alpha-blend.cpp
SFML	   = -lsfml-graphics -lsfml-window -lsfml-system
OBJECTS    = $(SOURCES:.cpp=.o)
EXECUTABLE = blnd

.PHONY: compile
compile: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(SFML)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY: run
run:
	./blnd ./Imgs/Table.png ./Imgs/Cat.png 300 200

.PHONY: clear
clear:
	rm -rf $(OBJECTS) $(EXECUTABLE)

clrObj:
	rm -rf $(OBJECTS)
