#include<stdio.h> //printf
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>

#define PORT 8888   //The port on which to listen for incoming data
void error(char *msg)
{
    perror(msg);
    exit(1);
}
int main(void)
{
    printf("hi\n");
    struct sockaddr_in serv_addr, cli_addr;
    int sockfd, newsockfd, n;
    socklen_t cli_len;
    char buffer[512];
    char message[512];
    char sZhostName[255];
    gethostname(sZhostName,255);
    struct hostent *host_entry;
    host_entry = gethostbyname(sZhostName);


    //create a socket
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR OPENING SOCKET");

    // zero out the structure
    bzero((char *) &serv_addr, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //bind socket to port
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0)
            error("ERROR ON BINDING");

    //keep listening for data
    listen(sockfd,5);
    printf("Listening on ip %s and port %d\n", inet_ntoa (*(struct in_addr *)*host_entry->h_addr_list), ntohs(serv_addr.sin_port));
    while(1)
    {
        cli_len = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
        if (newsockfd < 0) error("ERROR ON ACCEPT");

        while(1)
        { 
            bzero(buffer,512);
            n = read(newsockfd,buffer,511);
            if(n < 0) error("ERROR READING FROM SOCKET");
            printf("Friend: %s\n" , buffer);

            //now reply
            printf("Please enter the message: ");
            bzero(buffer,512);
            fgets(buffer,511,stdin);
            n = write(newsockfd,buffer, strlen(buffer));
            if (n < 0) error("ERROR writing to socket");
        }
        close(newsockfd);
    }
    close(sockfd);
    return 0;
}