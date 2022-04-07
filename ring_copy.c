#include "header.h"

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

void show(ring_s *ring){
    system("clear");
    printf("\n-----------ME------------\n");
    printf("| ID:%10s\t|\n", ring->me.ID);
    printf("| IP:%10s\t|\n", ring->me.IP);
    printf("| PORT:%10s\t|\n", ring->me.PORT);
    printf("%10s\t|%10s\t|\t%10s\t%10s\n","NÓ", "CHAVE", "IP", "PORTA");
    printf("__________________________________________________________________\n");
    printf("%9s\t|%9s\t|\t%13s\t%10s\n","ME",ring->me.ID, ring->me.IP, ring->me.PORT);
    if(ring->pred.PORT != 0){
        printf("\n----------PRED-----------\n");
        printf("| ID:\t\t%s\t|\n", ring->pred.ID);
        printf("| IP:\t\t%s\t|\n", ring->pred.IP);
        printf("| PORT:\t\t%s\t|\n", ring->pred.PORT);
        
    }
    if(ring->suc.PORT != 0){
        printf("\n----------SUC-----------\n");
        printf("| ID:\t\t%s\t|\n", ring->suc.ID);
        printf("| IP:\t\t%s\t|\n", ring->suc.IP);
        printf("| PORT:\t\t%s\t|\n", ring->suc.PORT);
    }
    printf("-----------------------------\n");
    
    
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

void  redefine_mask_size(fd_set *rset, int *mask_size, int listenfd,int udpfd,int connfd,int tcp_s_fd,int tcp_c_fd){
    *mask_size = 0;
    FD_SET(0, rset);
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

int main(int argc, char *argv[]){
    size_t size; 
    bool ring_created = false;                           
    char buf[MAX_CHAR];                     // buffer que vai guardar os caracteres
    int sret /*return do select*/=0;     //                              //
    
    ring_s ring;
    struct sockaddr_in clinodeaddr, mynodeaddr_tcp_s,
                                    mynodeaddr_udp_s, /*mynodeaddr_udp_c,*/ pred_tcp_s;
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
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    mynodeaddr_udp_s.sin_family = AF_INET;
	mynodeaddr_udp_s.sin_addr.s_addr = htonl(INADDR_ANY);
	mynodeaddr_udp_s.sin_port = htons(atoi(ring.me.PORT));
    bind(udpfd, (struct sockaddr*)&mynodeaddr_udp_s, sizeof(mynodeaddr_udp_s));

	 
    command_s command;

    memset(&command, 0, sizeof(command_s));

    char *fds; 
   
   //
    int alone_in_a_ring; //is 0 if alone in a ring
    FD_ZERO(&rset);
	
    for (;;) {
        
        redefine_mask_size(&rset, &mask_size, listenfd, udpfd, connfd, tcp_s_fd, tcp_c_fd);
        
        
        rset_cpy = rset; 
        fds=(char*)&rset;
        memset(buf,0,sizeof(char)*100);
        //redefine_mask_size(&mask_size, listenfd, udpfd, connfd, tcp_c_fd, tcp_s_fd);
        alone_in_a_ring = strcmp(ring.me.PORT, ring.pred.PORT);
        //printf("rset: %d\n",  (unsigned char)fds[0]); 
        
        /*insert sockets in the fd set*/
        /*sret =*/ select(mask_size+1, &rset_cpy, NULL, NULL, NULL);
        
        fds =(char*)&rset_cpy;
        //printf("rset_cpy %d\n",  (unsigned char)fds[0]);
        //FD_SET(connfd, &rset);
        
       /* if(sret<=0){
            printf("Oh dear, something went wrong with select()! %s\n", strerror(errno));
            exit(0);
        }*/
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
                show(&ring);
            }else if(buf[0]=='p'){

                if((string_to_command(buf, &command))==1){
                    printf("Given IP in pentry is not valid.\n");
                    continue;
                }

                //strcpy(ring.pred.ID, command.ID);
                //strcpy(ring.pred.IP, command.IP);
                //strcpy(ring.pred.PORT, command.PORT);
                // cria a socket do client (que vai mandar o "PRED")
                create_tcp_client(&tcp_c_fd, command.IP, command.PORT, &pred_tcp_s);
                // mando o SELF do tcp_c_fd para pred_tcp_s
                sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
                write(tcp_c_fd, buf, sizeof(char)*100); /*sends SELF to connfd*/
            }
            else if(strcmp(buf, "l")==0){
                //vou mandar um PRED i i.IP i.port\n ao meu sucessor 
                //sendo i o meu predecessor
                sprintf(buf, "%s %s %s %s\n", "PRED",ring.pred.ID,ring.pred.IP,ring.pred.PORT);
                write(tcp_s_fd, buf, strlen(buf));
                //close all sockets
                close_all_sockets(listenfd, udpfd, connfd, tcp_s_fd, tcp_c_fd);
                memset(&ring.pred, 0, sizeof(node));
                memset(&ring.suc, 0, sizeof(node));
                printf("Leave\n");
                
            }
            else if(strcmp(buf, "ex")){
                //exit
            }
            else if(strcmp(buf, "\n")==0)continue;
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
                    //bool condition1 = (((atoi(command.ID)>atoi(ring.me.ID))&&(atoi(command.ID)<atoi(ring.suc.ID))) || (alone_in_a_ring==0));  //se estiver sozinho e o ID do nó entrante for maior, ENTRA
                    //bool condition2 = !ring_created;
                    
                        if(ring_created){ // se o anel estiver criado
                            if(alone_in_a_ring != 0){ //verifica se não está sozinho no anel
                                printf("HERE2\n");
                                sprintf(buf, "%s %s %s %s\n", "PRED",command.ID,command.IP,command.PORT);         
                                write(tcp_s_fd, buf, strlen(buf));  
                                close(tcp_s_fd);
                            
                            }else{ //está sozinho no anel 
                                //é o meu pred e suc ao mm tempo
                                printf("HERE1\n");
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
                            strcpy(ring.pred.ID,command.ID);
                            strcpy(ring.pred.IP, command.IP);
                            strcpy(ring.pred.PORT, command.PORT);
                            strcpy(ring.suc.ID,command.ID);
                            strcpy(ring.suc.IP, command.IP);
                            strcpy(ring.suc.PORT, command.PORT);
                        }
                        strcpy(ring.suc.ID,command.ID);
                        strcpy(ring.suc.IP, command.IP);
                        strcpy(ring.suc.PORT, command.PORT);
                    
                        //close(connfd);
                        //connfd=0;
                    
                        }
                //if(tcp_s_fd!=0)close(tcp_s_fd);
                tcp_s_fd = connfd;
                connfd=0;
                
                
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
            }else{
                close(tcp_c_fd);
                tcp_c_fd=0;
            }
        }


        
    }/*for*/
        
}/*main*/