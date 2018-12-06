cmake_minimum_required(VERSION 3.10)

project(tic-tac-toe)

find_package(Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -std=c++0x -Werror")

add_executable(main main.cpp
        neural_net tictactoe genetic)
add_executable(play_the_game play_the_game.cpp
        tictactoe neural_net genetic)

target_link_libraries(main pthread)
