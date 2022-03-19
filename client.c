#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#define PORT "8989"

void main(){
    int fd, errcode; 
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res; 
    struct sockaddr_in addr;
    char buffer[500];

    fd=socket(AF_INET, SOCK_DGRAM, 0);
    if(fd==-1) /*error*/ exit(1);
    
    memset(&hints, 0 ,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errcode = getaddrinfo("127.0.0.1", PORT, &hints, &res);

    if(errcode != 0) exit(1);

    n=sendto(fd, "Minho>Guadiana!\n", 16,0, res->ai_addr, res->ai_addrlen);
    if(n==-1)exit(1);

    //printf("mandei uma cena\n");

    addrlen = sizeof(addr);
    n=recvfrom(fd, buffer, 500, 0, (struct sockaddr*) &addr, &addrlen);
    if(n==-1)exit(1);

    write(1, "echo: ", 6);
    write(1, buffer, n);

    freeaddrinfo(res);
    close(fd); 


}