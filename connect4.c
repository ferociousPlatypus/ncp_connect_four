#include "connect4.h"

// initialize new game board to blank spaces
void new_game(game *b){
    int i, j;
    for(i = 0; i < ROW; i++){
        for(j = 0; j < COL; j++){
            b->board[i][j] = ' ';
        }
    }
    return;
}

void print_board(game *b){
    int r, c, i;

    // label columns
    for(i = 0; i < COL; i++){
        printf("  %i ", i);
    }
    printf("\n");

    // print game board
    for(r = ROW - 1; r >= 0; r--){
        //print divider
        for(i = 0; i < COL*4+1; i++){
            printf("-");
        }
        printf("\n");

        for(c = 0; c < COL; c++){
            printf("| %c ", b->board[r][c]);
        }
        printf("|\n");
    }

    for(i = 0; i < COL*4+1; i++){
         printf("-");
    }
    printf("\n");
    return;
}

int check_horizontal(game *b, int row, int c){  // check left & right
    int left = 0;
    int right = 0;
    int temp = c;
    // check left
    while( (temp-1 >= 0) && (b->board[row][temp] == b->board[row][temp-1]) ){
        left++;
        temp -= 1;
    }
    temp = c;
    //check right
    while( (temp+1 < COL) && (b->board[row][temp] == b->board[row][temp+1]) ){
        right++;
        temp += 1;
    }

    if(left + right == 3)
        return (b->board[row][c] == PLAYER_ONE)?1:2;
    else
        return 0;
}

int check_vertical(game *b, int row, int col){
    int top = 0;
    int bottom = 0;
    int temp = row;
    // check bottom
    while( (temp-1 >= 0) && (b->board[temp][col] == b->board[temp-1][col]) ){
        bottom++;
        temp -= 1;
    }
    temp = row;
    //check top
    while( (temp+1 < ROW) && (b->board[temp][col] == b->board[temp+1][col]) ){
        top++;
        temp += 1;
    }

    if(top + bottom == 3)
        return (b->board[row][col] == PLAYER_ONE)?1:2;
    else
        return 0;
}

int check_diag_down(game *b, int row, int col){
    int left = 0;
    int right = 0;
    int tempRow = row;
    int tempCol = col;
    while( (tempCol - 1 >= 0) && (tempRow + 1 < ROW) &&  (b->board[tempRow][tempCol] == b->board[tempRow+1][tempCol-1]) ){
        left++;
        tempCol -= 1;
        tempRow += 1;
    }
    tempRow = row;
    tempCol = col;
    while( (tempCol + 1 < COL) && (tempRow - 1 >= 0) &&  (b->board[tempRow][tempCol] == b->board[tempRow-1][tempCol+1]) ){
        right++;
        tempCol += 1;
        tempRow -= 1;
    }
    if(left + right == 3)
        return (b->board[row][col] == PLAYER_ONE)?1:2;
    else
        return 0;
}

int check_diag_up(game *b, int row, int col){
    int left = 0;
    int right = 0;
    int tempRow = row;
    int tempCol = col;
    while( (tempCol - 1 >= 0) && (tempRow - 1 >= 0) &&  (b->board[tempRow][tempCol] == b->board[tempRow-1][tempCol-1]) ){
        left++;
        tempCol -= 1;
        tempRow -= 1;
    }
    tempRow = row;
    tempCol = col;
    while( (tempCol + 1 < COL) && (tempRow + 1 < ROW) &&  (b->board[tempRow][tempCol] == b->board[tempRow+1][tempCol+1]) ){
        right++;
        tempCol += 1;
        tempRow += 1;
    }
    if(left + right == 3)
        return (b->board[row][col] == PLAYER_ONE)?1:2;
    else
        return 0;
}

/*
return:
    0 - no winner
    1 - player 1 wins
    2 - player 2 wins
*/
int check_win(game *b, int r, int c){
    return check_horizontal(b, r, c) + check_vertical(b, r, c) + check_diag_down(b, r, c) + check_diag_up(b, r, c);
}

// return -1 if invalid move
int drop_piece(game *b, int col, int player){
    char piece = (player==1)?PLAYER_ONE:PLAYER_TWO;
    int r;
    if(col < 0 || col >= COL)
        return -1;
    for(r = 0; r < ROW; r++){
        if(b->board[r][col] == ' '){
            b->board[r][col] = piece;
            return check_win(b, r, col);
        }
    }
    return -1;
}
