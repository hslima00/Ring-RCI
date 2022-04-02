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
    printf("My ID:%s\nMy IP:%s\nMy PORT:%s\n",ring->me.ID,ring->me.IP,ring->me.PORT);
    
    printf("\n----------PRED-----------\n");
    printf("ID:%s\n", ring->pred.ID);
    printf("IP:%s\n", ring->pred.IP);
    printf("PORT:%s\n", ring->pred.PORT);
    
    
    printf("\n----------SUC-----------\n");
    printf("ID:%s\n", ring->suc.ID);
    printf("IP:%s\n", ring->suc.IP);
    printf("PORT:%s\n", ring->suc.PORT);
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

void redefine_mask_size(int *mask_size,int listenfd,int udpfd,int connfd,int tcp_c_fd,int tcp_s_fd){
    int temp = *mask_size;
    max(listenfd,temp);
    max(udpfd,temp);
    max(connfd,temp);
    max(tcp_s_fd,temp);
    max(tcp_c_fd,temp);
    *mask_size = temp;
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

int main(int argc, char *argv[]){

    bool ring_created = false;                           
    char buf[MAX_CHAR];                     // buffer que vai guardar os caracteres
    int sret /*return do select*/;     //                              //
    
    ring_s ring;
    struct sockaddr_in clinodeaddr, mynodeaddr_tcp_s, mynodeaddr_tcp_c,
                                    mynodeaddr_udp_s, /*mynodeaddr_udp_c,*/ pred_tcp_s, suc_tcp_s;
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

                    // !TCP SERVER
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0))==-1){
        printf("Error: Listenfd\n");
        exit(0);
    }
	bzero(&mynodeaddr_tcp_s, sizeof(mynodeaddr_tcp_s));
	mynodeaddr_tcp_s.sin_family = AF_INET;
	mynodeaddr_tcp_s.sin_addr.s_addr = htonl(INADDR_ANY);
	mynodeaddr_tcp_s.sin_port = htons(atoi(ring.me.PORT));
    if((bind(listenfd, (struct sockaddr*)&mynodeaddr_tcp_s, sizeof(mynodeaddr_tcp_s)))==-1){
        printf("Error: Bind\n");
        exit(0);
    }
    listen(listenfd, 10);

                    // !UDP SERVER
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    mynodeaddr_udp_s.sin_family = AF_INET;
	mynodeaddr_udp_s.sin_addr.s_addr = htonl(INADDR_ANY);
	mynodeaddr_udp_s.sin_port = htons(atoi(ring.me.PORT));
    bind(udpfd, (struct sockaddr*)&mynodeaddr_udp_s, sizeof(mynodeaddr_udp_s));

                    //! TCP Client
    if ((tcp_c_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket creation failed");
		exit(0);
	}
	memset(&mynodeaddr_tcp_c, 0, sizeof(mynodeaddr_tcp_c));

	// Filling server information
	mynodeaddr_tcp_c.sin_family = AF_INET;
	mynodeaddr_tcp_c.sin_port = htons(atoi(ring.me.PORT));
	mynodeaddr_tcp_c.sin_addr.s_addr = inet_addr(ring.me.IP);

    mask_size= max(udpfd, listenfd) ;
    mask_size= max(STDIN_FILENO,mask_size)+1;	 
    command_s command;

    memset(&command, 0, sizeof(command_s));

    //char *fds; 
    FD_ZERO(&rset);
   
    

    for (;;) {
        FD_SET(STDIN_FILENO, &rset);
        if(listenfd) FD_SET(listenfd, &rset);
        if(udpfd) FD_SET(udpfd, &rset);
        if(connfd) FD_SET(connfd, &rset);
        if(tcp_s_fd) FD_SET(tcp_s_fd, &rset);
        if(tcp_c_fd) FD_SET(tcp_c_fd, &rset); 
        rset_cpy = rset; 
        //fds=(char*)&rset;
       
        redefine_mask_size(&mask_size, listenfd, udpfd, connfd, tcp_c_fd, tcp_s_fd);
        
        //printf("rset: %d\n",  (unsigned char)fds[0]); 
        
        /*insert sockets in the fd set*/
        sret = select(mask_size, &rset_cpy, NULL, NULL, NULL);
        //fds =(char*)&rset_cpy;
        //printf("rset_cpy %d\n",  (unsigned char)fds[0]);
        //FD_SET(connfd, &rset);
        
        if(sret<=0){
            printf("erro no select\n");
            exit(0);
        }

        //*if tcp socket is readable then handle it by accepting the connection
        if (listenfd && (FD_ISSET(listenfd, &rset_cpy))) {
           //! clear 
           FD_CLR(listenfd, &rset);
            len = sizeof(struct sockaddr_in);
            if(connfd != 0)close(connfd);
            connfd = accept(listenfd, (struct sockaddr*)&clinodeaddr, &len);
            mask_size = max(connfd, mask_size)+1;
          
            printf("O client conectou-se com a socketfd: %d\n", connfd);
        }
                    
        // if udp socket is readable receive the message.
		if (udpfd && (FD_ISSET(udpfd, &rset_cpy))) {

			len = sizeof(clinodeaddr);
			bzero(buf, sizeof(buf));
			n = recvfrom(udpfd, buf, sizeof(buf), 0,
						(struct sockaddr*)&clinodeaddr, &len);
			printf("Message from UDP client (shortcut): %s\n",buf);
            //puts(buf);
            /*   MESSAGE HANDLER PARA O SHORCUT */

            /*a mensaguem recebida por udp é o buf*/
			sendto(udpfd, "hello!", sizeof(buf), 0,
				(struct sockaddr*)&clinodeaddr, sizeof(clinodeaddr));
		}

        if (tcp_c_fd && (FD_ISSET(tcp_c_fd, &rset_cpy))){
            memset(buf,'\0',sizeof(char)*100);
            n=read(connfd,buf, sizeof(char)*100);
            if(n==-1){
                printf("Error in read\n");
                continue;
            }

            //* vou receber o meu novo pred nesta mensagem
            string_to_command(buf, &command);
            if(ring_created){
                //* atualiza o seu pred 
                strcpy(ring.pred.ID,command.ID);
                strcpy(ring.pred.IP, command.IP);
                strcpy(ring.pred.PORT, command.PORT);
                //*fechar a socket com o meu antigo pred, i 
                if(tcp_c_fd != 0)close(tcp_c_fd);
                if((tcp_c_fd = socket(AF_INET, SOCK_STREAM, 0))==-1){
                    perror("socket creation failed");
                    exit(0);
                }
                //* criar nova socket para mandar o SELF ao nó j
                suc_tcp_s.sin_family = AF_INET;
                if (inet_pton(AF_INET, ring.pred.IP, &suc_tcp_s.sin_addr) < 1) {
                    printf("adress not valid: \n");
                    continue;
  	            }
	            suc_tcp_s.sin_port = htons(atoi(ring.pred.PORT)); 
                connect(tcp_c_fd, (struct sockaddr*)&suc_tcp_s, sizeof(suc_tcp_s));
                sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
                write(tcp_c_fd, buf, sizeof(char)*100);
            }


        }

        if(connfd && (FD_ISSET(connfd, &rset_cpy))){
            memset(buf,'\0',sizeof(char)*100);
            n=read(connfd,buf, sizeof(char)*100);
            printf("CONNFD\n");
            if(n==-1){
                printf("Error in read\n");
            }
            buf[strlen(buf)-1]='\0';
            printf("BUFF = %s\n", buf);

            if(buf[0]=='S'){
                printf("este gajo mandou-me um SELF\n");
            
                string_to_command(buf, &command);
                if(ring_created){
                    //TODO:
                    //se eu tiver num anel então tenho que mandar uma mensagem ao meu sucessor
                    // "PRED j j.IP j.PORT\n" sendo j o anel que está a tentar entrar
                    //* 1- atualizar o sucessor para j
                    strcpy(ring.suc.ID,command.ID);
                    strcpy(ring.suc.IP, command.IP);
                    strcpy(ring.suc.PORT, command.PORT);
                    //* 2- informa o antigo sucessor sobre o nó j e abre o anel
                    memset(buf,'\0', sizeof(char)*100);
                    sprintf(buf, "%s %s %s %s\n", "PRED",command.ID,command.IP,command.PORT);         
                    write(tcp_s_fd, buf, sizeof(char)*100);
                    //* 3- abre o anel
                    if(tcp_s_fd!=0)close(tcp_s_fd);
                    tcp_s_fd = connfd;

                }else{ //o anel não está criado.
                    ring_created = true; 
                    //neste caso o nó entrante é tanto meu sucessor como meu predecessor 
                    strcpy(ring.suc.ID,command.ID);
                    strcpy(ring.suc.IP, command.IP);
                    strcpy(ring.suc.PORT, command.PORT);
                    strcpy(ring.pred.ID,command.ID);
                    strcpy(ring.pred.IP, command.IP);
                    strcpy(ring.pred.PORT, command.PORT);
                    tcp_s_fd = connfd;
                    
                }
                mask_size=max(tcp_s_fd, mask_size);
                connfd=0;   
                FD_CLR(connfd, &rset);
            }
        }
        
        if(tcp_s_fd && (FD_ISSET(tcp_s_fd, &rset_cpy))){
            
            printf("message from suc\n");
            bzero(buf, sizeof(char)*100);
            read(tcp_s_fd,buf,sizeof(char)*100);
            printf("buff = %s\n, buf[0]= %d\n", buf, *buf);
            string_to_command(buf, &command);
            printf("command.ID (in tcp_s_fd) = %s\n", command.ID);
            if(*buf=='s' && strcmp(command.ID, ring.suc.ID)){
                bzero(buf, sizeof(char)*100);
                printf("My suc tried to pentry with me again!\n");
                strcpy(buf,"I'm already your pred!\n");
                write(tcp_s_fd,buf,sizeof(char)*100);
            }
            FD_CLR(tcp_s_fd, &rset);

        }

        if(FD_ISSET(STDIN_FILENO, &rset_cpy)){
            bzero(buf, sizeof(char)*100);
            read(STDIN_FILENO,buf, sizeof(char)*100); //reads the stdin
            buf[strlen(buf)-1]='\0'; //removes \n from buffer
            /*HANDLE MESSAGES FROM USER INPUT*/
            if((strcmp(buf, "n") ==0) && (ring_created == false)){
                ring_created = true;
                new(&ring);
            }else if(strcmp(buf, "s")==0){ //show
                show(&ring);
            }else if(buf[0]=='p'){
                if((string_to_command(buf, &command))==1){
                    printf("Given IP in pentry is not valid.\n");
                    continue;
                }
                pred_tcp_s.sin_family = AF_INET;
                if (inet_pton(AF_INET, command.IP, &pred_tcp_s.sin_addr) < 1) {
                    //inet_pton - convert IPv4 and IPv6 addresses from text to binary form 
                    // returns 0 if given adress isn't valid 
                    printf("adress not valid: \n");
                    continue;
  	            }
	            pred_tcp_s.sin_port = htons(atoi(command.PORT)); 
                connect(tcp_c_fd, (struct sockaddr*)&pred_tcp_s, sizeof(pred_tcp_s)); /*received by listen -> accept -> connfd*/
                
                sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
            
                write(tcp_c_fd, buf, sizeof(char)*100); /*sends SELF to connfd*/
            }else if(strcmp(buf, "\n")==0)continue;
        }
    }/*for*/
        
}/*main*/