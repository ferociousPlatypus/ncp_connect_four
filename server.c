#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>

#define PACKET_SIZE 2

struct thread_data{
    int onefd, twofd;   //two player fd
    struct sockaddr_in oneAddr, twoAddr;
    socklen_t oneLen, twoLen;
    int player;
};
typedef struct thread_data thread_data;

typedef struct sockaddr SA;

void sig_handler(int signum);
void error_exit(char* str);
int create_socket(int port);
// int connect_socket(SA *clientAddr, int clientLen);
int get_rand_num(void);
void *execute(void *thread_arg);

int def_socket;

int main(int argc, char **argv){
    signal(SIGINT, sig_handler);    // terminate program - maybe...

    if(argc != 2){
        printf("Error Enter Port Number!");
        exit(1);
    }
    int port;
    port = atoi(argv[1]);

    def_socket = create_socket(port);
    printf("Listening on Port: %i\n", port);

    while(1){
        thread_data *td = (thread_data*) malloc(sizeof(thread_data));
        
        // accept two connections before moving on to establish game
        td->oneLen = sizeof(struct sockaddr_in);
        td->onefd = accept(def_socket, (SA*) &(td->oneAddr), &(td->oneLen));
        td->twoLen = sizeof(struct sockaddr_in);
        td->twofd = accept(def_socket, (SA*) &(td->twoAddr), &(td->twoLen));

        // randomly determine player1
        if(get_rand_num() % 2 == 0)
            td->player = 1;
        else
            td->player = 2;
        
        int pid = fork();
        if(pid == 0){
            execute(td);
            return 0;
        }
    }

    return 0;
}

void *execute(void *thread_arg){
    printf("Process ID: %i\n", (int)getpid());

    // extract data from the struct passed to the thread
    struct thread_data *my_data = (struct thread_data *) thread_arg;
    int onefd, twofd, player;
    struct sockaddr_in oneAddr, twoAddr;
    socklen_t oneLen, twoLen;

    onefd = my_data->onefd;                 
    oneLen = my_data->oneLen;
    memcpy(&oneAddr, &my_data->oneAddr, sizeof(struct sockaddr_in));
    twofd = my_data->twofd;                 
    twoLen = my_data->twoLen;
    memcpy(&twoAddr, &my_data->twoAddr, sizeof(struct sockaddr_in));
    player = my_data->player;
 
    printf("Connected\n");
        char* buf = malloc(PACKET_SIZE * sizeof(char));

        if(player == 1){
            send(onefd,"p1", PACKET_SIZE, 0);
            send(twofd, "p2", PACKET_SIZE, 0);
        }
        else{
            send(onefd,"p2", PACKET_SIZE, 0);
            send(twofd, "p1", PACKET_SIZE, 0);
        }

    do{
        struct timeval time_out = {10, 0};          // TIMEOUT VALUE!!!
        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(onefd,&readset);
        FD_SET(twofd,&readset);
        if(select(128, &readset, NULL, NULL, &time_out) == 0){
            printf("Timeout\n");
            buf = "t\0";
            send(onefd, buf, PACKET_SIZE, 0);
            send(twofd, buf, PACKET_SIZE, 0);
            break;
        }
        if(player == 1){
            recv(onefd, buf, PACKET_SIZE, 0);
            send(twofd, buf, PACKET_SIZE, 0);
        }
        else{
            recv(twofd, buf, PACKET_SIZE, 0);
            send(onefd, buf, PACKET_SIZE, 0);
        }
        player = (player == 1)?2:1;
    }
    while(strcasecmp(&buf[0], "q") != 0);
    printf("gameover\n");

    close(onefd);
    close(twofd);
    return NULL;
}

int create_socket(int port){
    int fd = 0;
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error_exit("Error Creating Socket");

    int optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval , sizeof(int)) < 0){
        error_exit("Error setsockopt");
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(port);

    if( bind(fd, (SA*) &servaddr, sizeof(servaddr)) < 0){
        error_exit("Error Binding Socket");
    }
    
    if(listen(fd, 1024) < 0){
        error_exit("Error Listening on Socket");
    }

    return fd;
}

int get_rand_num(void){
	int num = 0;
	time_t t;
	srand((unsigned) time(&t));
	num = rand() % 65000;
	return num;
}

void sig_handler(int signum){
  printf("\nExiting Now...\n");
  close(def_socket);
  exit(0);
}

void error_exit(char* str){
    perror(str);
    exit(EXIT_FAILURE);
}
