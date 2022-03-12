#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#define MAX_CHAR 20


int main(int argc, char *argv[]){
    printf("Program started\n");
    //CENAS DO SELECT()                     //
    int fd_stdin, fd_stdout;                                 // variavel file descriptor                                   
    char buf[MAX_CHAR];                     // buffer que vai guardar os caracteres
    int ret /*return do read*/, sret /*return do select*/, ret2;     //  
    fd_stdin=0;
    fd_stdout=1;                                 //
    fd_set readfds;                         //
    fd_set writefds;
    struct timeval timeout;                 //

    while(1){
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(fd_stdin, &readfds);
        FD_SET(fd_stdout, &writefds);

        timeout.tv_sec=5;
        timeout.tv_usec=0;

        sret=select(8, &readfds, NULL,NULL,&timeout);

        if(sret==-1){

            printf("erro no select\n");

        }else if(sret==0){

            printf("timeout, o return do select() é %d\n", sret);

        }else{

            printf("O return do select() é=%d\n", sret);
            memset((void*)buf, 0, MAX_CHAR);

            //estamos a meter cenas no stdin
            ret = read(fd_stdin, (void*)buf,MAX_CHAR);

            //estamos a meter cenas no stdout
            ret2 = write(fd_stdout, (void*)buf, MAX_CHAR);


            if(ret != -1 && ret2 != -1){
                printf("buf= %s\n", buf);
            }else{
                printf("Erro no read ou write\n");
            } 
        }
    }
}