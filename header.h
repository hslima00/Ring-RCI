#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <stdbool.h>
#define MAX_CHAR 20

#define SERVERPORT 8989
#define BUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA; 
//#define PORT "58001"

typedef struct{
    int ID; //
    char *IP;
    char *PORT;
    /*char IP_PRED[100];
    char IP_SUC[100]; */
}node; 

//estrutura de estruturas com nodes

typedef struct{
    node pred;
    node suc;
    node me; 
    node rope; 
}ring_s;