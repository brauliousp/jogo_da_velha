#ifndef TIC_TAC_TOE_TICTACTOE_H
#define TIC_TAC_TOE_TICTACTOE_H

enum Player {
    PLAYER_X = -1, PLAYER_O = 1, PLAYER_NONE = 0
};

enum TicTacToeStatus {
    TTT_GAME_RUNNING, TTT_PLAYER_X_WON, TTT_PLAYER_O_WON, TTT_TIE
};

/** TicTacToe engine. */
struct TicTacToe {

    /** Creates an empty game. */
    TicTacToe();

    /** Makes a move (an int from 0..8). Returns false if the move is invalid. */
    bool move(int move, Player player);

    /** Prints the board to stdio. */
    void print() const;

    /** Returns the board as an 1D array. */
    const int * get_board_as_array() const;

    /** Return the current status of the game. */
    TicTacToeStatus status() const;

    /** Returns who should play next. */
    Player next_player() const;

    /** Return the # of valid moves made up to now. */
    int moves_made();

private:
    Player board[3][3];
    int _moves_made;
    TicTacToeStatus _status;
    Player _next_player;

    void update_game_status();
};

#endif //TIC_TAC_TOE_TICTACTOE_H
