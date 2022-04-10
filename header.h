#define _CRT_SECURE_NO_DEPRECATE
#define _OPEN_SYS_ITOA_EXT
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
#define MAX_CHAR 100

#define SERVERPORT 8989
#define BUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA; 
//#define PORT "58001"

typedef struct{
    int udp_s;
    int tcp_s;
    int udp_c;
    int tcp_c; 
}sock_s;

typedef struct
{
  char ID[5];
  char IP[16];
  char PORT[10];
  char opt[10];
  char searched_key[5];
  char n_seq[5];
} command_s;


typedef struct{
    char ID[5];
    char IP[16];
    char PORT[10];
}node; 

//estrutura de estruturas com nodes

typedef struct{
    node pred;
    node suc;
    node me; 
    node chord; 
}ring_s;

typedef struct
{
   int key_to_find; 
   char addr[INET_ADDRSTRLEN]; //save the address of the guy that asks for the EFND
   int port;  
   bool mode; // if EFND 1, FIND 0 
}find_s;


void new(ring_s *ring);