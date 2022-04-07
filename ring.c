#include "header.h"

//! notes on search a key 
// array que guarda um searched_key number associado à posição da array(mete a -1 quando receber a resposta)
// ou seja primeira busca iniciada no meu no há de ser na array[0]=seached_key
// segunda array[1]=searched_key segunda busca etc etc 
// quando chega um rsp com o meu key vou ver no array[n(que vem na mensagem)] qual era o k que eu tinha procurado e onde está 
//! guardar o address de buscas por UDP [isto é EFNDS por alguém fora do ring](pelo menos 5 adresses )
// posso reservar as primeiras 5 posições do array para guardar esses (usando um k que vai de 0 a 4 se estiver em quatro e não
//tiver o searched_key do de nunhum k a -1 não inicia busca (nem responde com ACK) ) 
//! ^altamente improvável
// sendo assim o n irá de 5 a 99 FND'S fazer memset a -1 quando saí do ring  e quando começa o programa !
// a array vai guardar char's por isso é tudo feito com strcpy e strcmp! º
//k e n vão ser static_int e levam reset quando chegam ao máximo
//(caso a posição 0 esteja vazia senão tentamos encontrar uma vazia(strcmp(array[n],'-1') incrementando)
//returns 1 if not valid, 0 if valid

bool verify_ip(char* ip_string){ 

	char delim[] = ".";
    char *string_to_free,*ip_string_copy;
    //ip_string_copy = (char*) malloc((strlen(ip_string)+1)*sizeof(char));
    
    string_to_free=ip_string_copy=strdup(ip_string);
    char *ptr = strtok(ip_string_copy, delim);
    int point_counter=0;
    
    while(ptr != NULL)
	{
        if(atoi(ptr)<0 || atoi(ptr)>255 || point_counter > 4){
            printf("IP not valid\n");
            return 1;
        }
        point_counter++;
		ptr = strtok(NULL, delim);
	}

    free(string_to_free);
    return 0;
    
}

int distance(char* ID1, char* ID2){
    int l = 0;
    int k = 0;
    int N = 32;
    k = atoi(ID1);
    l = atoi(ID2);
    int resto;
    resto = (l-k)%N;
    if(resto<0){
        resto = resto+32;
    }
    printf("distancia entre %s e %s = %d\n", ID1, ID2 ,resto);

    return resto; 
}


//close and sets to 0 all sockets
void close_all_sockets(int listenfd,int udpfd,int connfd,int tcp_s_fd,int tcp_c_fd){
    if(listenfd != 0)close(listenfd);
    if(udpfd != 0)close(udpfd);
    if(connfd != 0)close(connfd);
    if(tcp_s_fd != 0)close(tcp_s_fd);
    if(tcp_c_fd != 0)close(tcp_c_fd);
}

int valid_arguments(int argc, char *argv[], ring_s *ring){
    bool ret;
    ret = verify_ip(argv[1]);
    if(argc<3){
        printf("You've Entered Too Few Arguments. Usage: ring i i.IP i.PORT\n");
        return(1);
    }else if(ret==1){                                       //validar o IP
        printf("Each Octave of the IP must be between 0 and 255 for it to be valid!\n");
        return(1);
    }else if(atoi(argv[0]) < 0 || atoi(argv[0]) > 31){                  //validar i
        printf("O i tem de ser um numero entre 0 e 31.\n");
        return(1);
    }else 
    //printf("ret :           %s\n", ret);
   
    strcpy(ring->me.ID,argv[0]);
    strcpy(ring->me.PORT, argv[2]);
    strcpy(ring->me.IP, argv[1]);    
    //validar o IP
    

    return(0);
}

void show(ring_s *ring, int alone, bool ring_created){
    static int print = 0;
    system("clear");
    printf("%30s\n\n","RCI-Anel");
    printf("|%6s\t|%10s\t|%10s\t|%10s\n","NÓ", "CHAVE", "IP", "PORTA");
    printf("-----------------------------------------------------\n");
    printf("|%5s\t|%9s\t|%13s\t|%10s\n","ME",ring->me.ID, ring->me.IP, ring->me.PORT);
    if(atoi(ring->pred.ID)!=0){
        printf("|%5s\t|%9s\t|%13s\t|%10s\n","PRED",ring->pred.ID, ring->pred.IP, ring->pred.PORT);
        printf("|%5s\t|%9s\t|%13s\t|%10s\n","SUC",ring->suc.ID, ring->suc.IP, ring->suc.PORT);
    }
    
    if(ring_created && (alone==0) && (print!=0)){
        printf("\nI'm inside a ring but alone.\n");
    }else if(ring_created && (alone!=0) && (print!=0)){
        printf("\nI'm inside a ring.\n");
    }else if(print!=0){
        printf("\nNot inside a ring.\n");
    }
    
    if(print==0){
        printf("Commands:\n");
        printf("pentry - entry a ring knowing future predecessor position\n");
        printf("mais comandos etc etc\n");
        printf("Enter a command or press any key followed by ENTER to continue the program.\n");
        print++;
    }
    
} 


void initialize_ring_memory(ring_s *ring){ 
    memset(&ring->pred,0,sizeof(node));
    memset(&ring->suc,0,sizeof(node));
}

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}

void  redefine_mask_size(fd_set *rset, int *mask_size, int listenfd,int udpfd,int connfd,int tcp_s_fd,int tcp_c_fd, bool leave){
    *mask_size = 0;
    
    if(listenfd){
        FD_SET(listenfd, rset);
        *mask_size = max(*mask_size, listenfd);
    } 
    if(udpfd){
        FD_SET(udpfd, rset);
        *mask_size = max(*mask_size, udpfd);
    } 
    if(connfd){
        FD_SET(connfd, rset);
        *mask_size = max(*mask_size, connfd);
    }
    if(tcp_c_fd){
        FD_SET(tcp_c_fd, rset);
        *mask_size = max(*mask_size, tcp_s_fd); 
    } 
}

char* split_find(char *buf){
    char delim[] = " ";
    char *ptr = strtok (buf, delim);
    char *temp;
    int arg_counter = 0;
    char *args[3];
    while (ptr != NULL)
    {
      args[arg_counter] = (char *) malloc ((strlen(ptr)+1) * sizeof (char));
      strcpy (args[arg_counter], ptr);
      
      ptr = strtok (NULL, delim);
      printf("arg[%d]=%s\n", arg_counter, args[arg_counter]);
      arg_counter++;

    }

    return temp;
}

bool string_to_command (char *buf, command_s *command)
{

  char delim[] = " ";

  char *ptr = strtok (buf, delim);
  int arg_counter = 0;
  char *args[4];
  while (ptr != NULL)
    {
      args[arg_counter] = (char *) malloc ((strlen(ptr)+1) * sizeof (char));
      strcpy (args[arg_counter], ptr);
      arg_counter++;
      ptr = strtok (NULL, delim);
    }
 

  strcpy (command->opt, args[0]);
  strcpy (command->ID, args[1]);
  strcpy (command->IP, args[2]);
  strcpy (command->PORT, args[3]);

  free(args[3]);
  free(args[2]);
  free(args[1]);
  free(args[0]);

  if(verify_ip(command->IP)==1){
      return 1;
  }

  return 0; 
}

void user_input(char *buf){
    size_t len =1000;
    getline(&buf, &len, stdin);
    buf[strlen(buf)-1]='\0';
}

void create_tcp_client(int *tcp_c_fd, char* IP, char* PORT, struct sockaddr_in *pred_tcp_s ){ 
    
    if ((*tcp_c_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket creation failed");
		exit(0);
	}

	memset(pred_tcp_s, 0, sizeof(*pred_tcp_s));
    pred_tcp_s->sin_family = AF_INET;
	pred_tcp_s->sin_port = htons(atoi(PORT));
	pred_tcp_s->sin_addr.s_addr = inet_addr(IP);
	// Filling server information
	
    if(connect(*tcp_c_fd, (struct sockaddr*) pred_tcp_s, sizeof(*pred_tcp_s)) != 0 ){
        printf(" Erro connect 'create tcp client' \n    ");
    }
    
}

void create_tcp_server(int *listenfd, char* PORT, struct sockaddr_in *tcp_s ){
    if((*listenfd = socket(AF_INET, SOCK_STREAM, 0))==-1){
        printf("Error: Listenfd\n");
        exit(0);
    }
    if (setsockopt(*listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        perror("setsockopt(SO_REUSEADDR) failed");
    }
    
	bzero(tcp_s, sizeof(*tcp_s));
	tcp_s->sin_family = AF_INET;
	tcp_s->sin_addr.s_addr = htonl(INADDR_ANY);
	tcp_s->sin_port = htons(atoi(PORT));
    if((bind(*listenfd, (struct sockaddr*)tcp_s, sizeof(*tcp_s)))==-1){
        printf("Error in TCP Server creation: Bind\n");
        exit(0);
    }
    if((listen(*listenfd, 10)==-1)){
        printf("Listen error: %s\n", strerror(errno));
    }
}

void create_udp_server(int *udpfd, char* PORT, struct sockaddr_in *udp_s){
    *udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    udp_s->sin_family = AF_INET;
	udp_s->sin_addr.s_addr = htonl(INADDR_ANY);
	udp_s->sin_port = htons(atoi(PORT));
    bind(*udpfd, (struct sockaddr*)udp_s, sizeof(*udp_s));
}

int main(int argc, char *argv[]){
    size_t size; 
    bool ring_created = false; 
    bool print_mask = false; 
    bool leave = false;      
    char key_to_f[10]; //find
    char trash[5]; //find
    int n_sec=0; //numero de sequencia para o find
    char n_sec_str[5]; //find
    char buf[MAX_CHAR];                     // buffer que vai guardar os caracteres
    int sret /*return do select*/=0;     //                              //
    ring_s ring;
    struct sockaddr_in clinodeaddr, mynodeaddr_tcp_s,
                                    mynodeaddr_udp_s,  pred_tcp_s;
    socklen_t len;
    ssize_t n; 
    int listenfd=0;/*socket listen do tcp server*/
    int connfd=0; /*socket tcp returned from connect*/
    int udpfd=0; /*socket udp */
    int tcp_c_fd=0; 
    int tcp_s_fd=0;
    int mask_size; /*max file descriptors inside table*/
    fd_set rset, rset_cpy;
    
    initialize_ring_memory(&ring); //sets pred and suc memory to NULL (para n ter lixo) and mallocs memory for self node.

    if(valid_arguments(argc - 1, argv + 1, &ring)){
        //se a porta ou ip não estiverem válidos então o programa fecha
        printf("Usage: ./ring i i.IP i.PORT\n");
        exit(-1);
    } 

    // *create TCP listening socket and UDP SERVERS
    create_tcp_server(&listenfd, ring.me.PORT, &mynodeaddr_tcp_s);
    //create_udp_server(&udpfd, ring.me.PORT);

    create_udp_server(&udpfd, ring.me.PORT, &mynodeaddr_udp_s);
    

	 
    command_s command;

    memset(&command, 0, sizeof(command_s));

    char *fds; 
   
    int alone_in_a_ring=0; //is 0 if alone in a ring
    FD_ZERO(&rset);

    show(&ring, alone_in_a_ring, ring_created);
	
    for (;;) {
        
        if(leave == false){
            FD_SET(0, &rset);
            redefine_mask_size(&rset, &mask_size, listenfd, udpfd, connfd, tcp_s_fd, tcp_c_fd, leave);
        }else if(leave == true){
            leave = false;
            listenfd=0;
            udpfd=0;
            connfd=0;
            tcp_c_fd=0;
            tcp_s_fd=0;
            create_tcp_server(&listenfd, ring.me.PORT, &mynodeaddr_tcp_s);
            create_udp_server(&udpfd, ring.me.PORT, &mynodeaddr_udp_s);
            FD_ZERO(&rset);
            FD_SET(0, &rset);
        }
        
        rset_cpy = rset; 
        
        memset(buf,0,sizeof(char)*100);
        
        alone_in_a_ring = strcmp(ring.me.PORT, ring.pred.PORT);
         
        
        if(print_mask){
            fds=(char*)&rset;
            printf("rset: %d\n",  (unsigned char)fds[0]);
        } 

        if((sret =select(mask_size+1, &rset_cpy, NULL, NULL, NULL))==-1){
            printf("Error in select: %s\n", strerror(errno));
            exit(0);
        }

        if(print_mask){
            fds =(char*)&rset_cpy;
            printf("rset_cpy %d\n",  (unsigned char)fds[0]);
        }
        
        FD_SET(connfd, &rset);
        
        
        if(FD_ISSET(0, &rset_cpy)){
            FD_CLR(0,&rset);
            user_input(buf);           
            //read(0,buf, sizeof(char)*100); //reads the stdin
            
            //removes \n from buffer
            /*HANDLE MESSAGES FROM USER INPUT*/
            if((strcmp(buf, "n") ==0) && (ring_created == false)){
                ring_created = true;
                new(&ring);
            }else if((strcmp(buf, "n") ==0) && (ring_created == true)){
                printf("Ring already created\n");
            }else if(strcmp(buf, "s")==0){ //show
                show(&ring, alone_in_a_ring, ring_created);
            }else if(buf[0]=='p'){

                if((string_to_command(buf, &command))==1){
                    printf("Given IP in pentry is not valid.\n");
                    continue;
                }
                // cria a socket do client (que vai mandar o "PRED")
                create_tcp_client(&tcp_c_fd, command.IP, command.PORT, &pred_tcp_s);
                strcpy(ring.pred.ID,command.ID);
                strcpy(ring.pred.IP, command.IP);
                strcpy(ring.pred.PORT, command.PORT);
                // mando o SELF do tcp_c_fd para pred_tcp_s
                sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
                write(tcp_c_fd, buf, sizeof(char)*100); /*sends SELF to connfd*/
                leave=false;
            }
            else if(strcmp(buf, "l")==0){
                ring_created=0;
                leave=true;
                //vou mandar um PRED i i.IP i.port\n ao meu sucessor 
                //sendo i o meu predecessor
                sprintf(buf, "%s %s %s %s\n", "PRED",ring.pred.ID,ring.pred.IP,ring.pred.PORT);
                write(tcp_s_fd, buf, strlen(buf));
                //close all sockets
                close_all_sockets(listenfd, udpfd, connfd, tcp_s_fd, tcp_c_fd);
                memset(&ring.pred, 0, sizeof(node));
                memset(&ring.suc, 0, sizeof(node));
                printf("Leave\n");
                
            }else if(strcmp(buf, "d")==0){
                printf("distance\n");
                distance(ring.me.ID, ring.pred.ID);
            }else if(*buf=='f'){
                
                sscanf(buf, "%s %s", trash, key_to_f);
                if(key_to_f == ring.me.ID){
                    printf("It's me you dumbass\n");
                    continue;
                }else if(distance(ring.me.ID,key_to_f)>distance(ring.suc.ID, key_to_f)){//passar para o suc
                    //TODO: check distance between chord if i have one
                    //* se não tiver chord delego a procura ao suc 
                    memset(buf, 0, sizeof(buf));
                    n_sec = 3*atoi(ring.me.ID);
                    //sprintf(n_sec_str, "%d", n_sec);
                    sprintf(buf, "%s %s %d %s %s %s\n", "FND", key_to_f, n_sec, ring.me.ID, ring.me.IP, ring.me.PORT);
                    printf("BUF = %s\n", buf);
                    write(tcp_s_fd, buf, strlen(buf));
                }else{ // o objeto é meu
                    printf("O objeto é meu\n");
                }
                
                
            }
            else continue;
            
            
        }
        //*if tcp socket is readable then handle it by accepting the connection
        else if (listenfd && (FD_ISSET(listenfd, &rset_cpy))) {
            FD_CLR(listenfd, &rset); 
            len = sizeof(struct sockaddr_in);
            //if(connfd != 0)close(connfd);
            connfd = accept(listenfd, (struct sockaddr*)&clinodeaddr, &len);
            printf("O client conectou-se com a socketfd: %d\n", connfd);
        }
                    
        // if udp socket is readable receive the message.
		else if (udpfd && (FD_ISSET(udpfd, &rset_cpy))) {

			len = sizeof(clinodeaddr);
			
			n = recvfrom(udpfd, buf, sizeof(buf), 0,
						(struct sockaddr*)&clinodeaddr, &len);
			printf("Message from UDP client (shortcut): %s\n",buf);
           
			sendto(udpfd, "hello!", sizeof(buf), 0,
				(struct sockaddr*)&clinodeaddr, sizeof(clinodeaddr));
		}

        else if(connfd && (FD_ISSET(connfd, &rset_cpy))){
            FD_CLR(connfd, &rset);           
            if((n=read(connfd,buf, sizeof(char)*100))!=0){
                printf("CONNFD\n");
                if(n==-1){
                    printf("Error in read\n");
                }
                buf[strlen(buf)-1]='\0';
                printf("BUFF = %s\n", buf);

                if(buf[0]=='S'){
                    printf("Recebi um SELF\n");
                    string_to_command(buf, &command);
                    // condition1 = (((atoi(command.ID)>atoi(ring.me.ID))&&(atoi(command.ID)<atoi(ring.suc.ID))) || (alone_in_a_ring==0));  //se estiver sozinho e o ID do nó entrante for maior, ENTRA
                    //bool condition2 = !ring_created;
                    //if(condition1 || condition2){
                        if(ring_created){ // se o anel estiver criado
                            if(alone_in_a_ring != 0){ //verifica se não está sozinho no anel
                                sprintf(buf, "%s %s %s %s\n", "PRED",command.ID,command.IP,command.PORT);         
                                write(tcp_s_fd, buf, strlen(buf));  
                                close(tcp_s_fd);
                            
                            }else{ //está sozinho no anel 
                                //é o meu pred e suc ao mm tempo
                                
                                strcpy(ring.pred.ID,command.ID);
                                strcpy(ring.pred.IP, command.IP);
                                strcpy(ring.pred.PORT, command.PORT);
                                //manda o self ao gajo que se conectou
                                create_tcp_client(&tcp_c_fd, command.IP, command.PORT, &pred_tcp_s);
                                sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
                                write(tcp_c_fd, buf, strlen(buf));
                                
                            }
                        }else{ //o anel não está criado.
                            ring_created = true;
                            //strcpy(ring.pred.ID,command.ID);
                            //strcpy(ring.pred.IP, command.IP);
                            //strcpy(ring.pred.PORT, command.PORT);
                        }
                        strcpy(ring.suc.ID,command.ID);
                        strcpy(ring.suc.IP, command.IP);
                        strcpy(ring.suc.PORT, command.PORT);
                    //}else{ 
                        //condição das chaves 
                    //    close(connfd);
                    //    connfd=0;
                    //}
                
                //if(tcp_s_fd!=0)close(tcp_s_fd);
                tcp_s_fd = connfd;
                //close(connfd);
                connfd=0;
                }
                     
            }else{
                close(connfd);
                connfd=0;
                printf("EOF\n");
            }    
            
        }
        
        else if (tcp_c_fd && (FD_ISSET(tcp_c_fd, &rset_cpy))){
           FD_CLR(tcp_c_fd,&rset);
            if((n=read(tcp_c_fd,buf, sizeof(char)*100))!=0){
                if(n==-1){
                    printf("Error in read\n");
                }
                printf("\nEntrei tcp_c_fd\n");
                //* vou receber o meu novo pred nesta mensagem
                string_to_command(buf, &command);
                if(*buf=='P'){
                    printf("Recebi um PRED\n");

                    if(ring_created){
                        
                            //* atualiza o seu pred 
                            strcpy(ring.pred.ID,command.ID);
                            strcpy(ring.pred.IP, command.IP);
                            strcpy(ring.pred.PORT, command.PORT);
                            close(tcp_c_fd);
                            tcp_c_fd=0;
                            int temp_fd; 
                            create_tcp_client(&temp_fd, command.IP, command.PORT, &pred_tcp_s);
                            tcp_c_fd=temp_fd;
                            sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
                            write(tcp_c_fd, buf, strlen(buf));
                    } 
                }
                if(buf[0]=='F'){
                    //fflush(stdin);
                    printf("RECEBI UM FIND\n");
                }
            }else{
                close(tcp_c_fd);
                tcp_c_fd=0;
            }
        }


        
    }/*for*/
        
}/*main*/