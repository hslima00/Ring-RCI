#include "header.h"

int new(ring_s *ring){ //cria o anel com um nó com s=self 
    
    ring->pred.ID=ring->me.ID;
    ring->pred.PORT=ring->me.PORT;
    ring->pred.IP=ring->me.IP;

    ring->suc.ID=ring->me.ID;
    ring->suc.PORT=ring->me.PORT;
    ring->suc.IP=ring->me.IP;


}

int verify_ip(char* ip_string){
    int init_size = strlen(ip_string);
	char delim[] = ".";
    char *ptr = strtok(ip_string, delim);
    //printf("ip_string= %s", ip_string);
    while(ptr != NULL)
	{
		//printf("'%s'\n", ptr);
        if(atoi(ptr)<0 || atoi(ptr)>255){
            printf("IP not valid\n");
            return 1;
        }
		ptr = strtok(NULL, delim);
	}
    printf("Valid IP!\n");
    return 0;
}

int valid_arguments(int argc, char *argv[]){
    //return 1 if failure, return 0 if valid
    int i=0;
    //argv[0]= ring
    //argv[1]= i
    //argv[2]= ip
    //argv[3]=port

    if(argc<4){
        printf("You've Entered Too Few Arguments. Usage: ring i i.IP i.PORT\n");
        return(1);
    }else if(!(strcmp(argv[0],"ring\0")|| strcmp(argv[0],"RING\0"))){   //validar o primeiro argv "ring"
        printf("Usage: ring i i.IP i.PORT\n");
        return(1);
    }else if(verify_ip(argv[2])){                                       //validar o IP
        printf("Each Octave of the IP must be between 0 and 255 for it to be valid!\n");
        return(1);
    }else if(atoi(argv[1]) < 0 || atoi(argv[1]) > 31){                  //validar i
        printf("O i tem de ser um numero entre 0 e 31.\n");
        return(1);
    }else 
    
    //validar o IP
    

    return(0);
}

int main(int argc, char *argv[]){
    printf("Program started\nUsage: ring i i.IP i.PORT\n");
    //CENAS DO SELECT()                     //
    int fd_stdin, fd_stdout;                // variavel file descriptor                                   
    char buf[MAX_CHAR];                     // buffer que vai guardar os caracteres
    int ret /*return do read*/, sret /*return do select*/, ret2;     //  
    fd_stdin=0;
    fd_stdout=1;                                 //
    fd_set readfds;                         //
    fd_set writefds;
    struct timeval timeout;
             
    ring_s ring;

    //argv[1]= ring
    //argv[2]= i
    //argv[3]= ip
    //argv[4]=port

    

    if(valid_arguments(argc - 1, argv + 1))exit(-1); //se a porta ou ip não estiverem válidos então o programa fecha
    
    ring.me.IP= (char*) malloc(strlen(argv[2]+1)*sizeof(char));
    ring.me.PORT= (char*) malloc(strlen(argv[3]+1)*sizeof(char));
    
    memcpy(ring.me.IP, argv[3], sizeof(argv[3])+1);
    memcpy(ring.me.PORT, argv[4], sizeof(argv[4])+1);
    
    //new(&ring);

    int fd, errcode, newfd; 
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res; 
    struct sockaddr_in addr;
    memset(&hints, 0 ,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    printf("antes do TCP server");
    
    //create_TCP_Server(fd, &hints, &res, &ring, &addr, &addrlen);
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