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
#define PORT "8988"

int fd, errcode; 
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res; 
struct sockaddr_in addr; 
char buffer[128]; 

int main(){
    //TCP socket
    fd=socket (AF_INET, SOCK_STREAM, 0);
    printf("sucesso na criacao\n");
    if (fd==-1) exit(1); //error

    memset(&hints, 0, sizeof hints); 
    hints.ai_family=AF_INET; 
    hints.ai_socktype=SOCK_STREAM;
    //IPv4 //TCP socket

    errcode=getaddrinfo ("127.0.0.1", PORT, &hints,&res); 
    if (errcode !=0) /*error*/exit (1);
    
    n=connect(fd, res->ai_addr, res->ai_addrlen); 
    if(n==-1)/*error*/exit(1);

    n=write(fd, "Hello!\n", 7);
    if(n==-1)/*error*/exit(1);

    n=read (fd, buffer, 128);
    if(n==-1)/*error*/exit(1);

    write(1, "echo: ",6);
    write (1, buffer,n);
    printf("HERE");
    freeaddrinfo (res); 
    close(fd);
}
