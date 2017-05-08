#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>
#include <signal.h>

#ifndef CONNECT4_H
#define CONNECT4_H

#include "connect4.h"

#endif /* CONNECT4_H */

int clientfd = 0;

#define BUFSIZE 1024
typedef struct sockaddr SA;

// Game variables
game test;
int winner = 0;
int playerMove =1;
int col;
char charCol;
int playernumber = -1;
char buf[BUFSIZE];

void sig_handler(int signum){
	printf("\nExiting Now\n");
	close(clientfd);
	exit(0);
}

void error(char *msg) {
    perror(msg);
    exit(0);
}

void mimicServer(char* msg,int fd){
	int n;
    /* send the message line to the server */
    n = write(fd, msg, strlen(msg));
    if (n < 0) 
      error("ERROR writing to socket");
    
    return;
}
/***
 *	Client fd , handles connecting the client to the server ( Hostname, Port)
 ***/

int open_clientfd(char *hostname, int port) 
{
    int fd;
    struct hostent *hp;
    struct sockaddr_in serveraddr;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	return -1; /* check errno for cause of error */

    /* Fill in the server's IP address and port */
    if ((hp = gethostbyname(hostname)) == NULL)
	return -2; /* check h_errno for cause of error */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)hp->h_addr_list[0], 
	  (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
    serveraddr.sin_port = htons(port);

    /* Establish a connection with the server */
    if (connect(fd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
	return -1;
    return fd;
}
/***
 *	Initializes the Game,board, and prints it.
 ***/
void initiate_game(){
	new_game(&test);
	print_board(&test);
}
/***
 *	Pre processing . if ( "t" or "q" doesnt enter this function ).
 *	Return of "drop_piece" is either 0,1,2,-1. 
 * 	if( -1 therefore invalid ,and return ).
 * 	if( 0 now winner so just continues normally ) 
 * 	if( 1 , 2 declares and winner and ends current game ).
 ***/
int gameFrame(){	
    col  = charCol - '0';
    if( (winner = drop_piece(&test, col, playerMove)) != -1 )
        playerMove = (playerMove == 1)?2:1;
    else{
        printf("Invalid Move!\n");
        return -1;
    }
        
	print_board(&test);
	return 0;
}

int main(int argc,char** argv)
{
	
    int n, timeout = 0;
    /* Check arguments */
    if (argc != 3) {
	fprintf(stderr, "Usage: %s <host name> <port number>\n", argv[0]);
	exit(0);
    }
    //assigning port to 2nd argument 
    int port = atoi(argv[2]);

	do{
		int clientfd = open_clientfd(argv[1],port);
		signal(SIGINT, sig_handler);
		if(clientfd == -1)
		{
		    char msg[BUFSIZE];
		    sprintf(msg,"error creating a connection . connecting \n");
		    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
		    exit(0);
		}
		if(clientfd == -2)
		{
		    printf("Invalid hostname\n");
		    exit(0);
		}
		
		initiate_game();
		//mimicServer("p2",clientfd);
		
		// Player assignment Loop. Waits for server to assign playernumber.
		do{
			bzero(buf, BUFSIZE);
			n = read(clientfd, buf, BUFSIZE);
			if (n < 0) {
				error("ERROR reading from socket");
			}
			sleep(1);
		}while( n == 0);
		//printf("Echo from server: %s\n\n PlayerNumber : %c\n\n", buf,buf[1]);
	

		// set value of current player to 1
		if(strncmp(buf,"p1",2) == 0)
		{
			playernumber =1;
		}
		// set value of current player to 2
		else if(strncmp(buf,"p2",2) == 0)
		{
			playernumber = 2;
		}
		printf("You're PlayerNumber : %i \n", playernumber);
		
		// Game Handling Loop if Player number == 1
		if(playernumber == 1)
		{
			do{
			// Reads player input ( 0,6,t,q are valid anything else is not ). 
			// keeps requesting player input till input is valid.
			printf("Please enter player move: ");
			bzero(buf, BUFSIZE);
			fgets(buf, BUFSIZE, stdin);
			// Player Enters a q to quit.
			if(buf[0] == 'q'){
				printf("User Quit\n");
				winner = 3 - playernumber;
				break;
			}

			charCol = buf[0];
			if(gameFrame() == -1) continue;

			//Relays player move to server if valid.
			n = write(clientfd, buf, strlen(buf));
			if (n < 0) 
			  error("ERROR writing to socket");
				
			// if value of winner > 0 , A winner is found.
	 		if(winner > 0) break;
			  
			// Get 2nd player movment.  
			// by reading the buffer from the server.
			bzero(buf, BUFSIZE);
			n = read(clientfd, buf, BUFSIZE);
			if (n < 0) 
			  error("ERROR reading from socket");
			// Recieved a timeout from server.
			if(strcasecmp(&buf[0], "t") == 0){
				printf("Timeout!\n");
				timeout = 1;
				break;								// handle timeout better
			}
			// Other player quit.
			if(buf[0] == 'q'){
					printf("other user quit!\n");
					winner = playernumber;
					break;								// handle timeout better
			}

			printf("\nPlayer Number %c move \n",buf[0]);
			charCol = buf[0];
			gameFrame();		
			}while(winner <= 0);
			
		}
		
		// Game Handling Loop if Player number == 2
		if ( playernumber == 2)
		{
			do{
				bzero(buf, BUFSIZE);
				n = read(clientfd, buf, BUFSIZE);
				if (n < 0) 
				  error("ERROR reading from socket");
				// Recieved a timeout from server.
				if(strcasecmp(&buf[0], "t") == 0){
					printf("Timeout!\n");
					timeout = 1;
					break;								// handle timeout better
				}
				// Other player quit.
				if(buf[0] == 'q'){
					printf("other user quit!\n");
					winner = playernumber;
					break;								// handle timeout better
				}


				//printf("Echo from server: %s\n", buf);
				printf("\nPlayer Number %c move \n",buf[0]);
				charCol = buf[0];
				gameFrame();
				// if value of winner > 0 , A winner is found.
				if(winner > 0) break;

				do{
					printf("Please enter player move: ");
					bzero(buf, BUFSIZE);
					fgets(buf, BUFSIZE, stdin);
					if(buf[0] == 'q'){
						winner = 3 - playernumber;
						printf("User Quit\n");
						break;
					}
					charCol = buf[0];
			
				}while(gameFrame() == -1);
				if(buf[0] == 'q'){
					break;
				}
				/* send the message line to the server */
				n = write(clientfd, buf, strlen(buf));
				if (n < 0) 
				  error("ERROR writing to socket");
				//printf("\n\n Sent to Server : %c \n\n",buf[0]);
			
			}while(winner <= 0);
		
		}
		// recieved a timeout from server)
		if(timeout == 0){
				printf("Winner: Player %i\n", winner);
				if(winner == playernumber)
					printf("You Win");
				else
					printf("You Lose");
		}
	
		write(clientfd,"q", 2);
		close(clientfd);
		// Retry loop ?
		printf("\n\nEnter \'Y\' or \'y\' to start another match\nor any other key to exit\n\n");
		bzero(buf, BUFSIZE);
		fgets(buf, BUFSIZE, stdin);
	}while(buf[0] == 'Y' || buf[0] == 'y');

    return 0;
}
