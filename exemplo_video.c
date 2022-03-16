#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>

#define SERVERPORT 8989
#define BUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA; 

//prototypes
void * handle_connection(int);
int check(int exp, const char *msg);
int accept_new_connection(int server_socket);
int setup_server(short port, int backlog); 

int main(int argc , char **argv){
    int server_socket = setup_server(SERVERPORT, SERVER_BACKLOG);

    fd_set current_sockets, ready_sockets; /*declaring two file descriptor sets. select() is desctructive,
    it changes the set we pass in, so we need to copy our set if we dont want to lose it */

    //initialize current set (tabela de fd's)
    FD_ZERO(&current_sockets); /*sets the table with 0's*/
    FD_SET(server_socket, &current_sockets); /* adding the server socket to the current set */

    while (true){

        ready_sockets = current_sockets; //because select is destructive

        if(select(FD_SETSIZE, &ready_sockets/*vamos passar isto para dizer ao select
        que file descriptors é que ele tem que monitorizar, e por isto é que copiámos antes*/, NULL, NULL, NULL)<0){
            perror("select error");
            exit(EXIT_FAILURE);
        }

        //agora que chamámos o select, ele muda a fd set e mete lá apenas as sockets que estão prontas para serem lidas 
        //vamos iterar por elas todas 
        for(int i = 0; i<FD_SETSIZE; i++){
            if(FD_ISSET(i, &ready_sockets)){ //check if the socket is set 
                if(i == server_socket){
                    /*Se a socket do server está à espera de ser lida, é porque é um novo client*/
                    int client_socket = accept_new_connection(server_socket);
                    FD_SET(client_socket, &current_sockets); //meter o novo client na lista de sockets
                }else{
                    handle_connection(i);
                    FD_CLR(i, &current_sockets);
                }
            }
        } 

    } //while
return 0;
}

int setup_server(short port, int backlog){
   int server_socket, client_socket, addr_size;
   SA_IN server_addr; 

   check((server_socket = socket(AF_INET, SOCK_STREAM, 0)),
            "Failed to create socket\n");
    
    //initialize the address struct 
    server_addr.sin_addr.s_addr= INADDR_ANY;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port); 

    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)),
            "Bind failed!\n");
    check(listen(server_socket, backlog), 
            "Listen failed!");
    return server_socket; 
}

int accept_new_connection(int server_socket){
    int addr_size = sizeof(SA_IN);
    int client_socket; 
    SA_IN client_addr; 
    check(client_socket=
            accept(server_socket,
                    (SA*)&client_addr,
                    (socklen_t*)&addr_size),
                    "accept failed\n");
    return client_socket; 

}

int check(int exp, const char *msg){
    if (exp == SOCKETERROR){
        perror(msg);
        exit(1);
    }
    return(exp); 
}

void* handle_connection(int client_socket){
    char buffer[BUFSIZE]; 
    size_t bytes_read; 
    int msgsize=0; 
    char actualpath[PATH_MAX+1]; 

    //read the client's message -- the name of the file to read 
    while((bytes_read=read(client_socket, buffer+msgsize, sizeof(buffer)-msgsize-1))){
        msgsize += bytes_read; 
        if (msgsize > BUFSIZE-1 || buffer[msgsize-1] == '\n') break; 

    }

    check(bytes_read, "recv error");
    buffer[msgsize-1] = 0; //null the terminate message and remove the \n

    printf("REQUEST: %s\n", buffer);
    fflush(stdout); 

    //validity check
    if(realpath(buffer, actualpath)==NULL){
        printf("ERROR(bad path): %s\n", buffer);
        close(client_socket);
        return NULL; 
    }

    FILE *fp = fopen(actualpath, "r"); 
    if(fp == NULL){
        printf("ERROR(open): %s\n", buffer );
        close(client_socket); 
        return NULL; 
    }

    //read file contents and send them to client 
    //note this is a fine example program, but rather insecure
    //a real program would probably limit the client to a certain files 
    while((bytes_read=fread(buffer,1,BUFSIZE, fp))>0){
        //printf("sending %zu bytes\n", bytes_read);
        write(client_socket, buffer, bytes_read);
    }

    close(client_socket);
    fclose(fp); 
    printf("closing connection \n");
    return NULL; 

}