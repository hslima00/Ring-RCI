 
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAXLINE 1024
int main(int argc, char* argv[])
{
	int sockfd;
	char buffer[MAXLINE];
	char str[MAXLINE]; 	
	
	struct sockaddr_in servaddr;
	int PORT; 
	PORT = atoi(argv[1]);
	int n, len;
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket creation failed");
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(sockfd, (struct sockaddr*)&servaddr,
							sizeof(servaddr)) < 0) {
		printf("\n Error : Connect Failed \n");
	}

	for(;;){
		memset(buffer, 0, sizeof(buffer));
		bzero(str, sizeof(str));
		fgets(str,100, stdin);
		str[strlen(str)-1] = '\0';
		printf("lido do scanf %s\n", str);
		
		strcpy(buffer, str);
		write(sockfd, buffer, sizeof(buffer));
	}
		
	
}
