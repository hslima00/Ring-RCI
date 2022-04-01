#include "header.h"


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
    if(ring->pred.IP!=NULL || ring->pred.PORT!=NULL){
        printf("\n----------PRED-----------\n");
        printf("ID:%s\n", ring->pred.ID);
        printf("IP:%s\n", ring->pred.IP);
        printf("PORT:%s\n", ring->pred.PORT);
    }
    if(ring->suc.IP!=NULL || ring->suc.PORT!=NULL){
        printf("\n----------SUC-----------\n");
        printf("ID:%s\n", ring->suc.ID);
        printf("IP:%s\n", ring->suc.IP);
        printf("PORT:%s\n", ring->suc.PORT);
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



void string_to_command (char *buf, command_s *command)
{

  char delim[] = " ";
  printf ("command_string= %s\n", buf);
  char *ptr = strtok (buf, delim);
  int arg_counter = 0;
  char *args[4];
  while (ptr != NULL)
    {
      printf ("'%s'\n", ptr);
      args[arg_counter] = (char *) malloc (strlen (ptr + 1) * sizeof (char));
      strcpy (args[arg_counter], ptr);
      arg_counter++;
      ptr = strtok (NULL, delim);
    }

  for (int i = 0; i < arg_counter; i++)
    {
      printf ("arg[%d]=%s\n", i, args[i]);
    }
  

    /* //! DELETE 
  memset (&command, " ", sizeof (command));
  command.opt   = (char *) malloc (strlen (args[0] + 1) * sizeof (char));
  command.IP    = (char *) malloc (strlen (args[2] + 1) * sizeof (char));
  command.PORT  = (char *) malloc (strlen (args[3] + 1) * sizeof (char));
  command.ID   = (char *) malloc (strlen (args[1] + 1) * sizeof (char));*/

  strcpy (command->opt, args[0]);
  strcpy (command->ID, args[1]);
  strcpy (command->IP, args[2]);
  strcpy (command->PORT, args[3]);

  free(args[3]);
  free(args[2]);
  free(args[1]);
  free(args[0]);
}

int main(int argc, char *argv[]){

    bool ring_created = false;                           
    char buf[MAX_CHAR];                     // buffer que vai guardar os caracteres
    int sret /*return do select*/;     //                              //
    
    ring_s ring;
    struct sockaddr_in clinodeaddr, mynodeaddr_tcp_s, mynodeaddr_tcp_c,
                                    mynodeaddr_udp_s, /*mynodeaddr_udp_c,*/ pred_tcp_s;
    socklen_t len;
    ssize_t n; 
    char* message = "Hello Client";

    int listenfd=0;/*socket listen do tcp server*/
    int connfd=0; /*socket tcp returned from connect*/
    int udpfd=0; /*socket udp */
    int tcp_c_fd=0;

    int maxfdp1; /*max file descriptors inside table*/
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

    maxfdp1= max(udpfd, listenfd) ;
    maxfdp1= max(STDIN_FILENO,maxfdp1)+1;	
    connfd = -1; 
    command_s command;

    memset(&command, 0, sizeof(command_s));

    char *fds; 
    FD_ZERO(&rset);
    int tcp_pred = 0; 
    /*   SELF 
        recebe PRED*/

    int tcp_suc = 0;
    /* recebe SELF do */
    

    for (;;) {
        FD_SET(STDIN_FILENO, &rset);
        if(listenfd) FD_SET(listenfd, &rset);
        if(udpfd) FD_SET(udpfd, &rset);
        if(connfd) FD_SET(connfd, &rset);
        if(ring.suc.socket.tcp_s) FD_SET(ring.suc.socket.tcp_s, &rset);
        rset_cpy = rset; 
        fds=(char*)&rset;

        
        printf("rset: %d\n",  (unsigned char)fds[0]);
    
        
        
        /*insert sockets in the fd set*/
        sret = select(maxfdp1, &rset_cpy, NULL, NULL, NULL);
        fds =(char*)&rset_cpy;
        printf("rset_cpy %d\n",  (unsigned char)fds[0]);
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
            maxfdp1 = max(connfd, maxfdp1)+1;
              
            //maxfdp1 = max(connfd, maxfdp1)+1;
            
            //n=write(connfd, "hello", n);
            //FD_SET(connfd, &rset);
      
            printf("O client conectou-se com a socketfd: %d\n", connfd);
                // ! fiz uma função que retorna o que é suposto. ver no "split.c"
                                                        // ! pfv verificar esta função. não dá para todos os comandos.  
            //close(connfd);
            //TODO: Handle the data read
            //TODO: criar função que separa o recebido
            //TODO: verificar se os commandos estão bem definidos.
            
            //printf("O comando recebido foi repartido em:\nOPT:%s\nKey:%s\nIP:%s\nPORT:%s\n", command.opt, command.ID, command.IP, command.PORT);
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
			sendto(udpfd, (const char*)message, sizeof(buf), 0,
				(struct sockaddr*)&clinodeaddr, sizeof(clinodeaddr));
		}

        if(connfd && (FD_ISSET(connfd, &rset_cpy))){
            
            
            printf("Recebi mensagem de um cliente que já estava conectado: %d\nMensagem:%s\n", connfd, buf);
            
            memset(buf,'\0',100);
            n=read(connfd,buf, sizeof(char)*100);
            if(n==-1){
                printf("Error in read\n");
            }
            buf[strlen(buf)-1]='\0';
            printf("BUFF = %s\n", buf);

            if(buf[0]=='S'){
                string_to_command(buf, &command);
                if(ring_created){
                    // verificar se o ring.suc.ID é o mesmo que vem na msg 
                }

                ring_created = true; 
                
                strcpy(ring.suc.ID,command.ID);
                strcpy(ring.suc.IP, command.IP);
                strcpy(ring.suc.PORT, command.PORT);
                ring.suc.socket.tcp_s = connfd;
                maxfdp1=max(ring.suc.socket.tcp_s, maxfdp1);
                connfd=0;   
                FD_CLR(connfd, &rset);
            }
            

        }
        

        if(ring.suc.socket.tcp_s && (FD_ISSET(ring.suc.socket.tcp_s, &rset_cpy))){
            printf("message from suc\n");
            FD_CLR(ring.suc.socket.tcp_s, &rset);
        }


        if(FD_ISSET(STDIN_FILENO, &rset_cpy)){
            bzero(buf, sizeof(buf));
            read(STDIN_FILENO,buf, sizeof(char)*100);
            buf[strlen(buf)-1]='\0';
            printf("User input %s\n", buf);
            /*HANDLE MESSAGES FROM USER INPUT*/
            //? o buf chega como uma string
            if((strcmp(buf, "n") ==0) && (ring_created == false)){
                ring_created = true;
                new(&ring);
            }else if(strcmp(buf, "s")==0){ //show
                show(&ring);
            }else if(buf[0]=='p'){
                printf("pentry\n");
                string_to_command(buf, &command);
                if((verify_ip(command.IP))==1){
                    printf("Given IP in pentry is not valid.\n");
                    continue;
                }
                pred_tcp_s.sin_family = AF_INET;
	            pred_tcp_s.sin_addr.s_addr = htonl(INADDR_ANY);
	            pred_tcp_s.sin_port = htons(atoi(command.PORT)); 
                connect(tcp_c_fd, (struct sockaddr*)&pred_tcp_s, sizeof(pred_tcp_s)); /*received by listen -> accept -> connfd*/
                
                //TODO: fazer uma função que passe command para string

                // "SELF" "+ring.me.ID+" "+ring.me.IP+" "+ring.me.PORT+"\n"
                sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
            
                
                write(tcp_c_fd, buf, sizeof(char)*100); /*sends SELF to connfd*/
            }else if(strcmp(buf, "\n")==0)continue;
        }
    }/*for*/
        
}/*main*/


//TODO: avaliar as portas 