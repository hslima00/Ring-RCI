/*------------------Projeto- Base-de-Dados em Anel com Cordas-------------------------------------*
*  header.h                                                                                       *
*                                                                                                 *
*  Created by Hugo Lima e Maria Rodrigues on 14/04/22.                                            *
*  Copyright © 2022 Hugo Lima e Maria Rodrigues. All rights reserved.                             *
*                                                                                                 *
*-------------------------------------------------------------------------------------------------*/

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


#define SOCKETERROR (-1)


typedef struct sockaddr_in SA_IN; /*Estrutura que lida com os endereços de rede. Usada com endereços IPV4*/
typedef struct sockaddr SA; 


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

/*Estrutura do tipo node que guarda o ID, IP e porta do próprio nó, seu sucessor e predecessor*/

typedef struct{
    node pred;
    node suc;
    node me; 
    node chord; 
}ring_s;

typedef struct
{
   int key_to_find; 
   char addr[INET_ADDRSTRLEN]; /* Guardar o endereço do nó que pede EFND*/
   int port;  
   bool mode; // Se há EFND mode= 1, se há FIND mode= 0 
}find_s;

