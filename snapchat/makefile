CC = g++
FLAGS = `pkg-config --cflags --libs opencv`

all: snap procesA procesB procesC

snap: main.cpp
	$(CC) -o snap main.cpp $(FLAGS)

procesA: procesA.cpp
	$(CC) -o procesA procesA.cpp $(FLAGS)

procesB: procesB.cpp
	$(CC) -o procesB procesB.cpp $(FLAGS) -D _GNU_SOURCE

procesC: procesC.cpp
	$(CC) -o procesC procesC.cpp $(FLAGS) -pthread

clean:
	rm snap
	rm procesA
	rm procesB
	rm procesC
