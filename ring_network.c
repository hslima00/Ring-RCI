#include "header.h"

int new(ring_s *ring){ //cria o anel com um nó com s=self 
    if(ring->me.ID==ring->pred.ID){
        printf("Ring already created\n");
        return 0;
    }else{
        printf("Executa a função new()\n");
        ring->pred.ID=ring->me.ID;
        ring->pred.PORT=ring->me.PORT;
        ring->pred.IP=ring->me.IP;

        ring->suc.ID=ring->me.ID;
        ring->suc.PORT=ring->me.PORT;
        ring->suc.IP=ring->me.IP;
    }
    
    return 1;

}

int verify_ip(char* ip_string){
    int init_size = strlen(ip_string);
	char delim[] = ".";
    printf("ip_string= %s\n", ip_string);
    char *ptr = strtok(ip_string, delim);
    int point_counter=0;
    while(ptr != NULL)
	{
		//printf("'%s'\n", ptr);
        if(atoi(ptr)<0 || atoi(ptr)>255 || point_counter > 4){
            printf("IP not valid\n");
            return 1;
        }
        point_counter++;
        printf("point_counter= %d\n", point_counter);
		ptr = strtok(NULL, delim);
	}
    printf("Valid IP!\n");
    return 0;
}

int split_string(char* command_string){
    int init_size = strlen(command_string);
	char delim[] = " ";
    printf("command_string= %s", command_string);
    char *ptr = strtok(command_string, delim);
    char *ip_copy;
    int arg_counter = 0;
    char *args[4];
    while(ptr != NULL)
	{
		printf("'%s'\n", ptr);
        args[arg_counter] = (char*) malloc(strlen(ptr+1)*sizeof(char));
        strcpy(args[arg_counter],ptr);
        arg_counter++;
		ptr = strtok(NULL, delim);
	}
    for(int i = 0; i<arg_counter; i++){
        printf("arg[%d]=%s\n",i, args[i] );
    }
    ip_copy = (char*) malloc(strlen(args[2]+1)*sizeof(char));
    strcpy(ip_copy, args[2]);
    if(verify_ip(ip_copy)==1){
        printf("The IP entered in the command was not valid.\n");
        return 1; 
    }
     for(int i = 0; i<arg_counter; i++){
        printf("arg[%d]=%s\n",i, args[i] );
    }
    //printf("Valid IP!\n");
    return 0;
}

int valid_arguments(int argc, char *argv[]){
    //return 1 if failure, return 0 if valid
    int i=0;
    
    //argv[0]= i
    //argv[1]= ip
    //argv[2]=port

    if(argc<3){
        printf("You've Entered Too Few Arguments. Usage: ring i i.IP i.PORT\n");
        return(1);
    }else if(verify_ip(argv[1])){                                       //validar o IP
        printf("Each Octave of the IP must be between 0 and 255 for it to be valid!\n");
        return(1);
    }else if(atoi(argv[0]) < 0 || atoi(argv[0]) > 31){                  //validar i
        printf("O i tem de ser um numero entre 0 e 31.\n");
        return(1);
    }else 
    
    //validar o IP
    

    return(0);
}

void show(ring_s *ring){
    printf("My ID:%d\nMy IP:%s\nMy PORT:%s\n",ring->me.ID,ring->me.IP,ring->me.PORT);
    if(ring->pred.IP!=NULL || ring->pred.PORT!=NULL){
        printf("\n----------PRED-----------\n");
        printf("ID:%d\n", ring->pred.ID);
        printf("IP:%s\n", ring->pred.IP);
        printf("PORT:%s\n", ring->pred.PORT);
    }
    if(ring->suc.IP!=NULL || ring->suc.PORT!=NULL){
        printf("\n----------SUC-----------\n");
        printf("ID:%d\n", ring->suc.ID);
        printf("IP:%s\n", ring->suc.IP);
        printf("PORT:%s\n", ring->suc.PORT);
    }
}

void initialize_ring_memory(ring_s *ring,int argc, char *argv[]){
    for(int i=0; i<5; i++){
        printf("arg[%d]=%s\n", i, argv[i]);
    }
    ring->me.ID= atoi(argv[0]);
    ring->me.IP= (char*) malloc(strlen(argv[1]+1)*sizeof(char));
    ring->me.PORT= (char*) malloc(strlen(argv[2]+1)*sizeof(char));
    
    ring->pred.ID=-1;
    ring->suc.ID=-1;
   
    ring->pred.IP=      NULL;
    ring->pred.PORT=    NULL;
    ring->suc .IP=      NULL;
    ring->suc .PORT=    NULL;
 
}


int main(int argc, char *argv[]){
    printf("Program started\nUsage: ring i i.IP i.PORT\n");
    //CENAS DO SELECT()                     //
    int fd_stdin, fd_stdout;                // variavel file descriptor                                   
    char buf[MAX_CHAR];                     // buffer que vai guardar os caracteres
    int ret /*return do read*/, sret /*return do select*/, ret2;     //  
    fd_stdin=0;
    fd_stdout=1;                            //
    fd_set readfds;                         //
    fd_set writefds;
    struct timeval timeout;
             
    ring_s ring;

    //argv[1]= ring
    //argv[2]= i
    //argv[3]= ip
    //argv[4]=port
    initialize_ring_memory(&ring, argc-1, argv+1); //sets pred and suc memory to NULL (para n ter lixo) and mallocs memory for self node.
    

    if(valid_arguments(argc - 1, argv + 1))exit(-1); //se a porta ou ip não estiverem válidos então o programa fecha
    
    
    
   
    
    //new(&ring);

    int fd, errcode, newfd; 
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res; 
    struct sockaddr_in addr;
    memset(&hints, 0 ,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    
    
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
            //Se passarem 10 segundos dá timeout
            printf("timeout, o return do select() é %d\n", sret);
            //check for messages

        }else{
            memset((void*)buf, 0, MAX_CHAR);
            fgets(buf, 20, stdin);
            printf("buf=%s\n", buf);
            ret = strcmp(buf, "n\n");
            printf("ret=%d\n",ret);
            if(strcmp(buf, "n\n")==0){
                new(&ring);
            }else if(strcmp(buf, "e\n")==0 || strcmp(buf, "s\n")==0 || strcmp(buf, "l\n")==0 || strcmp(buf, "ex\n")==0){
                printf("e,s,l pressed\n");
                if(strcmp(buf, "s\n")==0){ //show
                    show(&ring);
                } 
            }else{
                //separar string 
                split_string(buf); //isto vai ser recebido por sockets de clientes 
                // pentry: 
                // verificar se existe o nó "pred", e se não tiver 
                //

            }
            
           


         
        }
    }
}