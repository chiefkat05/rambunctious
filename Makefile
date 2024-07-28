all:
	g++ -c ./sources/main.cpp
	g++ main.o -o main -lsfml-graphics -lsfml-window -lsfml-system