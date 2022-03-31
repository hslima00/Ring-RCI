#include "header.h"

int create_tcp_server(int listenfd, ring_s *ring){
                    // !TCP SERVER
    printf("inside\n");
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0))==-1){
        printf("Error: Listenfd\n");
        exit(0);
    }
    struct sockaddr_in mynodeaddr_tcp_s;
    printf("inside\n");
	bzero(&mynodeaddr_tcp_s, sizeof(mynodeaddr_tcp_s));
    printf("inside0\n");
    //! error here
	mynodeaddr_tcp_s.sin_family = AF_INET;
    printf("inside1\n");
	mynodeaddr_tcp_s.sin_addr.s_addr = htonl(INADDR_ANY);
	mynodeaddr_tcp_s.sin_port = htons(atoi(ring->me.PORT));
    printf("inside4\n");
    if((bind(listenfd, (struct sockaddr*)&mynodeaddr_tcp_s, sizeof(mynodeaddr_tcp_s)))==-1){
        printf("Error: Bind\n");
        exit(0);
    }
    listen(listenfd, 10);
    return listenfd;
}