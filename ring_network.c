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
        printf("ID %d\n", ring->me.ID);
        printf("IP %s\n", ring->me.IP);
        printf("PORT %s\n", ring->me.PORT);

        ring->suc.ID=ring->me.ID;
        ring->suc.PORT=ring->me.PORT;
        ring->suc.IP=ring->me.IP;
    }
    
    return 1;

}

char* verify_ip(char* ip_string){
    int init_size = strlen(ip_string);
	char delim[] = ".";
    printf("ip_string= %s\n", ip_string);
    char *ip_string_copy;
    ip_string_copy = (char*) malloc(strlen(ip_string+1)*sizeof(char));
    strcpy(ip_string_copy, ip_string);
    char *ptr = strtok(ip_string, delim);
    int point_counter=0;
    
    while(ptr != NULL)
	{
		//printf("'%s'\n", ptr);
        if(atoi(ptr)<0 || atoi(ptr)>255 || point_counter > 4){
            printf("IP not valid\n");
            return "error";
        }
        point_counter++;
        printf("point_counter= %d\n", point_counter);
		ptr = strtok(NULL, delim);
	}
    //printf("IP no verify %s\n", ip_string_copy);
    printf("Valid IP!\n");

    return ip_string_copy;
}

int split_string(char* command_string){
    int init_size = strlen(command_string);
	char delim[] = " ";
    printf("command_string= %s", command_string);
    char *ptr = strtok(command_string, delim);
    char *ip_copy;
    int arg_counter = 0;
    char *args[4];
    char *ret;
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
    ret = verify_ip(ip_copy);
    if(strcmp(ret,"error")==0){
        printf("The IP entered in the command was not valid.\n");
        return 1; 
    }

    for(int i = 0; i<arg_counter; i++){
        printf("arg[%d]=%s\n",i, args[i] );
    }
    //printf("Valid IP!\n");
    return 0;
}

int valid_arguments(int argc, char *argv[], ring_s *ring){
    //return 1 if failure, return 0 if valid
    int i=0;
    
    //argv[0]= i
    //argv[1]= ip
    //argv[2]=port
    char *ret;
    ret = (char*) malloc(strlen(argv[1]+1)*sizeof(char)); //return do verify ip;
    ret = verify_ip(argv[1]);
    if(argc<3){
        printf("You've Entered Too Few Arguments. Usage: ring i i.IP i.PORT\n");
        return(1);
    }else if(strcmp(ret,"error")==0){                                       //validar o IP

        printf("Each Octave of the IP must be between 0 and 255 for it to be valid!\n");
        return(1);
    }else if(atoi(argv[0]) < 0 || atoi(argv[0]) > 31){                  //validar i
        printf("O i tem de ser um numero entre 0 e 31.\n");
        return(1);
    }else 
    printf("ret :           %s\n", ret);
    ring->me.ID=atoi(argv[0]);
    strcpy(ring->me.PORT, argv[2]);
    strcpy(ring->me.IP, ret);    
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

int setup_server(short port, int backlog){
   int server_socket, client_socket, addr_size;
   SA_IN server_addr; 

   server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    //initialize the address struct 
    server_addr.sin_addr.s_addr= INADDR_ANY;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
     

    bind(server_socket, (SA*)&server_addr, sizeof(server_addr));
    listen(server_socket, backlog);
    return server_socket; 
}

void* handle_connection(int client_socket){
    char message[BUFSIZE]; 
    size_t message_bytes;  // size_t: unsigned integer
    int msgsize=0; 
  

    //read the client's message -- the name of the file to read 
    
    message_bytes=read(client_socket, message, sizeof(message)-1);
   
    message[strlen(message)-1] = 0; //null the terminate message and remove the \n

    printf("REQUEST: %s\n", message);
    write(client_socket, message, message_bytes);
    close(client_socket);

   
    return NULL; 

}


int accept_new_connection(int server_socket){
    int addr_size = sizeof(SA_IN);
    int client_socket; 
    SA_IN client_addr; 
    client_socket=accept(server_socket,(SA*)&client_addr,(socklen_t*)&addr_size);
    return client_socket; 

}

int check(int exp, const char *msg){
    if (exp == SOCKETERROR){
        perror(msg);
        exit(1);
    }
    return(exp); 
}

int main(int argc, char *argv[]){
    printf("Program started\nUsage: ring i i.IP i.PORT\n");
    //argv[1]= ring
    //argv[2]= i
    //argv[3]= ip
    //argv[4]=port
    //CENAS DO SELECT()                     //
    int fd_stdin, fd_stdout;                // variavel file descriptor                                   
    char buf[MAX_CHAR];                     // buffer que vai guardar os caracteres
    int ret /*return do read*/, sret /*return do select*/, ret2;     //                              //
    struct timeval timeout;
    ring_s ring;

    //fd and socket creation
    fd_set TCP_sockets, UDP_sockets, TCP_sockets_cpy, UDP_sockets_cpy;
    int server_socket = setup_server(SERVERPORT, SERVER_BACKLOG);
    FD_ZERO(&TCP_sockets); /*sets the table with 0's*/
    FD_SET(server_socket, &TCP_sockets); /* adding the server socket to the current set */

    
    initialize_ring_memory(&ring, argc-1, argv+1); //sets pred and suc memory to NULL (para n ter lixo) and mallocs memory for self node.
    

    if(valid_arguments(argc - 1, argv + 1, &ring))exit(-1); //se a porta ou ip não estiverem válidos então o programa fecha
    
    
    
   
    
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

        TCP_sockets_cpy =  TCP_sockets; 
        
        timeout.tv_sec=10;
        timeout.tv_usec=0;

        sret=select(5, &TCP_sockets_cpy, NULL,NULL, &timeout);
        


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
            }else if(strcmp(buf, "\n")==0){
                continue;
            }else
                {
                //separar string 
                split_string(buf); //isto vai ser recebido por sockets de clientes 
                // pentry: 
                // verificar se existe o nó "pred", e se não tiver 
                //

            }
            
            //agora que chamámos o select, ele muda a fd set e mete lá apenas as sockets que estão prontas para serem lidas 
            //vamos iterar por elas todas 
            for(int i = 0; i<FD_SETSIZE; i++){
                if(FD_ISSET(i, &TCP_sockets_cpy)){ //check if the socket is set 
                    printf("i dentro do for %d\n", i);
                    if(i == server_socket){
                        /*Se a socket do server está à espera de ser lida, é porque é um novo client*/
                        int client_socket = accept_new_connection(server_socket);
                        FD_SET(client_socket, &TCP_sockets_cpy); //meter o novo client na lista de sockets
                    }else{
                        handle_connection(i);
                        FD_CLR(i, &TCP_sockets_cpy);
                    }
                }
            } 

           


         
        }
    }
}