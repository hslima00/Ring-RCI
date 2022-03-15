#include "header.h"

int new(ring_s *ring){ //cria o anel com um nó com s=self 
    
    ring->pred.ID=ring->me.ID;
    ring->pred.PORT=ring->me.PORT;
    ring->pred.IP=ring->me.IP;

    ring->suc.ID=ring->me.ID;
    ring->suc.PORT=ring->me.PORT;
    ring->suc.IP=ring->me.IP;


}

int create_TCP_Server(int fd, struct addrinfo *hints, struct addrinfo **res, ring_s *ring, struct sockaddr_in *addr, socklen_t *addrlen){
    int errcode;
    ssize_t n;
    fd = socket(AF_INET, SOCK_STREAM, 0); //TCP
    if(fd==-1) /*error*/ {
        printf("Erro ao ligar socket TCP Server\n");
        exit(1);
    }

    errcode = getaddrinfo("127.0.0.1", ring->me.PORT, hints, res);
    if(errcode != 0){
        
        exit(1);
    }else printf("Socket ligada\n");
    

    n=bind(fd, res->ai_addr, res->ai_addrlen);
    if(n==-1){
        exit(1);
    }else printf("Erro no bind\n");

    if(listen(fd,5)==-1)exit(1);

    while(1){
        addrlen = sizeof(addr);
        /*if((newfd=accept(fd, (struct sockaddr*)&addr, &addrlen))==-1 exit(1);
        //exit(1);*/

        if((fd=accept(fd, (struct sockaddr*)&addr,
                        &addrlen))==-1)
                    /*error*/exit(1);

        n=read(fd,/*buffer*/NULL,128);
        if(n==-1)exit(1);
        write(1, "received: ", 10);
        write(1, /*buffer*/NULL, n);
        n=write(fd, /*buffer*/NULL, n);
        if(n==-1)exit(1); 

        close(fd);
    }

}



int main(int argc, char *argv[]){
    printf("Program started\n");
    //CENAS DO SELECT()                     //
    int fd_stdin, fd_stdout;                // variavel file descriptor                                   
    char buf[MAX_CHAR];                     // buffer que vai guardar os caracteres
    int ret /*return do read*/, sret /*return do select*/, ret2;     //  
    fd_stdin=0;
    fd_stdout=1;                                 //
    fd_set readfds;                         //
    fd_set writefds;
    struct timeval timeout;
             
    
    if(atoi(argv[1]) < 0 || atoi(argv[1]) > 31){
        printf("O i tem de ser um numero entre 0 e 31.\n");
        exit(-1);
    }

    
    ring_s ring;
    ring.me.IP= (char*) malloc(strlen(argv[2]+1)*sizeof(char));
    ring.me.PORT= (char*) malloc(strlen(argv[3]+1)*sizeof(char));
    memcpy(ring.me.IP, argv[2], sizeof(argv[2])+1);
    printf("o meu ip é %s\n", ring.me.IP);
    //passar argv[3] para inteiro
    //ring.me.PORT = atoi(argv[3]);
    memcpy(ring.me.PORT, argv[3], sizeof(argv[3])+1);
    printf("o minha porta é %s\n", ring.me.PORT);
    
    new(&ring);

    int fd, errcode, newfd; 
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res; 
    struct sockaddr_in addr;
    memset(&hints, 0 ,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    printf("antes do TCP server");
    
    create_TCP_Server(fd, &hints, &res, &ring, &addr, &addrlen);
    //create_TCP_Client();

    while(1){
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(fd_stdin, &readfds);
        FD_SET(fd_stdout, &writefds);

        timeout.tv_sec=10;
        timeout.tv_usec=0;

        sret=select(8, &readfds, NULL,NULL,&timeout);
        


        if(sret==-1){

            printf("erro no select\n");

        }else if(sret==0){

            printf("timeout, o return do select() é %d\n", sret);
            //check for messages

        }else{
            //Se passarem 10 segundos dá timeout
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