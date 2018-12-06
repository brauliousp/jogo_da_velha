CC = g++
CXXFLAGS = -Wall -Wextra -std=c++0x -Ofast -Werror -Wno-unused-result
LDLIBS = -lpthread

all: main play_the_game

main: main.o tictactoe.o neural_net.o genetic.o

play_the_game: play_the_game.o tictactoe.o neural_net.o genetic.o

tictactoe.o: tictactoe.cpp tictactoe.h
neural_net.o: neural_net.cpp neural_net.h
genetic.o: genetic.cpp genetic.h

clean:
	rm -f main play_the_game *.o
