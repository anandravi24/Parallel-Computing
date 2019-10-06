all: p1 p2

p1 : p1.o
	g++ p1.o -o p1 -lpthread

p1.o : problem1.cpp
	g++ -std=c++11 -c problem1.cpp -lpthread -o p1.o

p2 : p2.o
	g++ p2.o -o p2 -lpthread

p2.o : problem2_better.cpp
	g++ -std=c++11 -c problem2_better.cpp -lpthread -o p2.o

clean:
	rm *.o p2 p1


