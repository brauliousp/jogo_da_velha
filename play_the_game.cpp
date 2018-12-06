#include <cstdlib>
#include <cstdio>
#include <algorithm>

#include "tictactoe.h"
#include "genetic.h"

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

void game() {

    TicTacToe ttt;

    int move = 0;

    ttt.print();
    while (ttt.status() == TTT_GAME_RUNNING) {
        scanf("%d", &move);
        while (not ttt.move(move, ttt.next_player())) {
            puts("Invalid move.");
            scanf("%d", &move);
        }
        system(CLEAR_SCREEN);
        ttt.print();
    }

    switch (ttt.status()) {
        case TTT_PLAYER_X_WON:
            puts("X won");
            break;
        case TTT_PLAYER_O_WON:
            puts("O won");
            break;
        case TTT_TIE:
            puts("It was a tie");
            break;
        default:
            break;
    }
}

void game_against_best() {

    Indiv npc("best.txt");

    while (true) {

        TicTacToe game;

        puts("Play against the best:\n\n");

        while (game.status() == TTT_GAME_RUNNING) {
            if (game.next_player() == PLAYER_X) {
                float input[9], output[9];
                for (int i = 0; i < 9; i++) {
                    input[i] = (float) game.get_board_as_array()[i];
                }
                npc.nn.evaluate(input, output);
                auto move = (int) (std::max_element(output, output + 9) - output);

                if (not game.move(move, PLAYER_X)) {
                    printf("Invalid move: %d\n", move);
                    break;
                }
            } else {
                int move = 0;

                printf("> ");
                scanf("%d", &move);
                if (move < 0)
                    return;
                while (not game.move(move, PLAYER_O)) {
                    puts("Invalid move.");
                    printf("> ");
                    scanf("%d", &move);
                }
                system(CLEAR_SCREEN);
            }
            game.print();
        }

        switch (game.status()) {
            case TTT_PLAYER_X_WON:
                puts("You lose");
                break;
            case TTT_PLAYER_O_WON:
                puts("You win");
                break;
            case TTT_TIE:
                puts("Tie");
                break;
            default:
                ;
        }
    }
}


int main() {
    game_against_best();
}
