#include "connect4.h"

int main(int argc, char** argv){

    game test;
    new_game(&test);

    int winner = 0;
    int playerMove = 1;
    int col;
    char charCol;
    do{
        print_board(&test);

        printf("Player %i: ", playerMove);
        charCol = getchar();
        getchar(); // handles newline char eloquently
        col  = charCol - '0';
        if( (winner = drop_piece(&test, col, playerMove)) != -1 )
            playerMove = (playerMove == 1)?2:1;
        else
            printf("Invalid Move!\n");
    }
    while(winner <= 0);
    
    print_board(&test);
    printf("Winner: Player %i\n", winner);

    return 0;
}