#include <cstdio>
#include <cstdlib>

#include "tictactoe.h"

TicTacToe::TicTacToe() {
    _moves_made = 0;
    _status = TTT_GAME_RUNNING;
    _next_player = PLAYER_X;

    for (auto &lines : board) {
        for (auto &cell : lines) {
            cell = PLAYER_NONE;
        }
    }
}

bool TicTacToe::move(int move, Player player) {

    if (_status != TTT_GAME_RUNNING) {
        return false;
    }

    if (_next_player != player) {
        return false;
    }

    int move_x = move / 3;
    int move_y = move % 3;

    // move outside bounds or over occupied cell
    if (
            move_x < 0 or move_x > 3 or
            move_y < 0 or move_y > 3 or
            board[ move_x ][ move_y ] != PLAYER_NONE)
        return false;
    
    board[ move_x ][ move_y ] = player;

    _moves_made++;
    _next_player = (Player)-(int)player;
    update_game_status();

    return true;
}

void TicTacToe::update_game_status() {
    Player winner = PLAYER_NONE;

    int sum_d1 = 0;    /* sum of the main diagonal */
    int sum_d2 = 0;    /* sum of the secondary diagonal */
    
    for (int i = 0; i < 3; i++) {
        int sum_row = 0;
        int sum_col = 0;
        for (int j = 0; j < 3; j++) {
            sum_row += board[i][j];
            sum_col += board[j][i];
            if (i == j) {
                sum_d1 += board[i][j];
                sum_d2 += board[i][2 - j];
            }
        }
        if (sum_row == 3 * PLAYER_X || sum_col == 3 * PLAYER_X) {
            winner = PLAYER_X;
            break;
        }
        if (sum_row == 3 * PLAYER_O or sum_col == 3 * PLAYER_O) {
            winner = PLAYER_O;
            break;
        }
    }

    if (sum_d1 == 3 * PLAYER_X or sum_d2 == 3 * PLAYER_X)
        winner = PLAYER_X;
    if (sum_d1 == 3 * PLAYER_O or sum_d2 == 3 * PLAYER_O)
        winner = PLAYER_O;

    if (winner == PLAYER_NONE and _moves_made == 9) {
        _status = TTT_TIE;
    } else if (winner == PLAYER_X) {
        _status = TTT_PLAYER_X_WON;
    } else if (winner == PLAYER_O) {
        _status = TTT_PLAYER_O_WON;
    }

}

void TicTacToe::print() const {
    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            if (j % 2 != 0) {
                printf("|");
            } else if (i % 2 != 0) {
                printf("--");
            } else {
                char printed;
                switch (board[i/2][j/2]) {
                    case PLAYER_X:
                        printed = 'x';
                        break;
                    case PLAYER_O:
                        printed = 'o';
                        break;
                    default:
                        printed = ' ';
                }
                printf("%c ", printed);
            }
        }
        printf("\n");
    }
}

const int * TicTacToe::get_board_as_array() const {
    return reinterpret_cast<const int *>(board);
}

TicTacToeStatus TicTacToe::status() const {
    return _status;
}

Player TicTacToe::next_player() const {
    return _next_player;
}

int TicTacToe::moves_made() {
    return _moves_made;
}
