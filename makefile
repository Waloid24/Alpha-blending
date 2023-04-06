all:
	g++ -c Alpha-blend.cpp -o blend.o
	g++ blend.o -o blnd -lsfml-graphics -lsfml-window -lsfml-system

clear:
	rm -f *.o *.out
