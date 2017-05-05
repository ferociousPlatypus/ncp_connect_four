#ifndef _CONN_FOUR_
#define _CONN_FOUR_

#include <stdio.h>

#define ROW 6
#define COL 7

#define PLAYER_ONE 'x'
#define PLAYER_TWO 'o'

// player 1 = x
// player 2 = o

struct game{
    char board[ROW][COL];
};
typedef struct game game;

void new_game(game *b);
void print_board(game *b);

/*
return:
    -1 - invalid move
    0 - no winning move
    1 - player 1 made winning move
    2 - player 2 made winning move
*/
int drop_piece(game *b, int col, int player);

#endif