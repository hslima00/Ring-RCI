/*------------------Projeto- Base-de-Dados em Anel com Cordas-------------------------------------*
*  ring.c                                                                                         *
*                                                                                                 *
*  Created by Hugo Lima e Maria Rodrigues on 14/04/22.                                            *
*  Copyright © 2022 Hugo Lima e Maria Rodrigues. All rights reserved.                             *
*                                                                                                 *
*  Warning: For chord use the command c                                                           *
            For echord use the command e                                                          *
            For exit use the command x                                                            *                                    *
*-------------------------------------------------------------------------------------------------*/

#include "header.h"

/*----------------------------------------------------------------------------------------------------
 Verificação da validade do endereço IP recebido quando o programa é invocado
------------------------------------------------------------------------------------------------------*/
bool verify_ip(char* ip_string){ 

	char delim[] = ".";
    char *string_to_free,*ip_string_copy;
    
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

/*----------------------------------------------------------------------------------------------------
Função cria um anel com apenas um nó com a informação da invocação do programa
------------------------------------------------------------------------------------------------------*/

void new(ring_s *ring) { 
   
    printf("Executa a função new()\n");
    strcpy(ring->pred.ID,   ring->me.ID);
    strcpy(ring->pred.PORT, ring->me.PORT);
    strcpy(ring->pred.IP,   ring->me.IP);
    printf("ID %s\n",   ring->me.ID);
    printf("IP %s\n",   ring->me.IP);
    printf("PORT %s\n", ring->me.PORT);

    strcpy(ring->suc.ID,ring->me.ID);
    strcpy(ring->suc.PORT,ring->me.PORT);
    strcpy(ring->suc.IP,ring->me.IP);
}

/*----------------------------------------------------------------------------------------------------
Cálculo da distância entre 2 ID's
------------------------------------------------------------------------------------------------------*/
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

    return resto; 
}


/*----------------------------------------------------------------------------------------------------
Função que efetua o fecho das sockets 
------------------------------------------------------------------------------------------------------*/
void close_all_sockets(int listenfd,int udpfd,int connfd,int tcp_s_fd,int tcp_c_fd){
    if(listenfd != 0)close(listenfd);
    if(udpfd != 0)close(udpfd);
    if(connfd != 0)close(connfd);
    if(tcp_s_fd != 0)close(tcp_s_fd);
    if(tcp_c_fd != 0)close(tcp_c_fd);
}
/*----------------------------------------------------------------------------------------------------
Validação dos argumentos na invocação do programa: ./ring ID IP Porta
------------------------------------------------------------------------------------------------------*/
int valid_arguments(int argc, char *argv[], ring_s *ring){
    bool ret;
    ret = verify_ip(argv[1]);
    if(argc<3){
        printf("You've Entered Too Few Arguments. Usage: ring i i.IP i.PORT\n");
        return(1);
    }else if(ret==1){   
        /*Validação do IP*/                                  
        printf("Each Octave of the IP must be between 0 and 255 for it to be valid!\n");
        return(1);
    }else if(atoi(argv[0]) < 0 || atoi(argv[0]) > 31){  
        /*Validação do ID*/                
        printf("O i tem de ser um numero entre 0 e 31.\n");
        return(1);
    }else 
   
    strcpy(ring->me.ID,argv[0]);
    strcpy(ring->me.PORT, argv[2]);
    strcpy(ring->me.IP, argv[1]);    

    return(0);
}

/*------------------------------------------------------------------------------------------------------------
Função imprime no terminal o estado do nó, do seu predecessor e sucessor ( ID, IP, Porto e Chord quando existe)
--------------------------------------------------------------------------------------------------------------*/
void show(ring_s *ring, int alone, bool ring_created){
    static int print = 0;
    system("clear");
    printf("%30s\n\n","RCI-Anel");
    printf("|%6s\t|%10s\t|%10s\t|%10s\n","NÓ", "CHAVE", "IP", "PORTA");
    printf("-----------------------------------------------------\n");
    printf("|%5s\t|%9s\t|%13s\t|%10s\n","ME",ring->me.ID, ring->me.IP, ring->me.PORT);
    if(ring_created){
        printf("|%5s\t|%9s\t|%13s\t|%10s\n","PRED",ring->pred.ID, ring->pred.IP, ring->pred.PORT);
        printf("|%5s\t|%9s\t|%13s\t|%10s\n","SUC",ring->suc.ID, ring->suc.IP, ring->suc.PORT);
    }
    
    if(atoi(ring->chord.PORT)!=0){
        printf("|%5s\t|%9s\t|%13s\t|%10s\n","CHORD",ring->chord.ID, ring->chord.IP, ring->chord.PORT);
    }
    
    if(ring_created && (alone==0) && (print!=0)){
        printf("\nI'm inside a ring but alone.\n");
    }else if(ring_created && (alone!=0) && (print!=0)){
        printf("\nI'm inside a ring.\n");
    }else if(print!=0){
        printf("\nNot inside a ring.\n");
    }
    
    if(print==0){ /*Informação inicial útil para o usuário*/
        printf("Commands:\n");
        printf("new    - establishment of a ring containing one node\n");
        printf("bentry - entrance of a node without knowing its position\n");
        printf("pentry - entrance of a node knowing future predecessor position\n");
        printf("chord  - creation of a chord to node i\n");
        printf("echord - eliminate a chord\n");
        printf("show   - shows the key, ID and IP of the node\n");
        printf("find   - search the key k throughout the ring\n");
        printf("leave  - exit of the node out of the ring\n");
        printf("exit   - close application \n");
        printf("For chord use the command c\n");
        printf("For echord use the command e\n");
         printf("For exit use the command x\n");
        printf("Enter a command or press any key followed by ENTER to continue the program.\n");
        print++;
    }
    
} 

/*----------------------------------------------------------------------------------------------------
Inicialização da estrutura do tipo ring_s 
------------------------------------------------------------------------------------------------------*/
void initialize_ring_memory(ring_s *ring){ 
    memset(&ring->pred,0,sizeof(node));
    memset(&ring->suc,0,sizeof(node));
    memset(&ring->chord,0,sizeof(node));
}

/*----------------------------------------------------------------------------------------------------
Determinação do máximo entre 2 valores
------------------------------------------------------------------------------------------------------*/
int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}

/*----------------------------------------------------------------------------------------------------
Função que calcula o tamanho da máscara à medida que são adicionadas sockets à mesma 
------------------------------------------------------------------------------------------------------*/
void  redefine_mask_size(fd_set *rset, int *mask_size, int listenfd,int udp_s_fd, int udp_c_fd,int connfd,int tcp_c_fd){
    *mask_size = 0;
    
    if(listenfd){
        FD_SET(listenfd, rset);
        *mask_size = max(*mask_size, listenfd);
    } 
    if(udp_s_fd){
        FD_SET(udp_s_fd, rset);
        *mask_size = max(*mask_size, udp_s_fd);
    }
    if(udp_c_fd){
        FD_SET(udp_c_fd, rset);
        *mask_size = max(*mask_size, udp_c_fd);
    }
    if(connfd){
        FD_SET(connfd, rset);
        *mask_size = max(*mask_size, connfd);
    }
    if(tcp_c_fd){
        FD_SET(tcp_c_fd, rset);
        *mask_size = max(*mask_size, tcp_c_fd); 
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
    size_t length =1000;
    getline(&buf, &length, stdin);
    buf[strlen(buf)-1]='\0';
}

/*---------------------------------------------------------------------------------------------------------------------
Criação do cliente TCP
---------------------------------------------------------------------------------------------------------------------*/
void create_tcp_client(int *tcp_c_fd, char* PORT, struct sockaddr_in *pred_tcp_s ){ 
    
    /*Criação e verificação da socket*/
    if ((*tcp_c_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket creation failed");
		exit(0);
	}

    
	memset(pred_tcp_s, 0, sizeof(*pred_tcp_s));
    pred_tcp_s->sin_family = AF_INET;
	pred_tcp_s->sin_port = htons(atoi(PORT));
	
	
    /* Conecta a socket do cliente à do servidor */
    if(connect(*tcp_c_fd, (struct sockaddr*) pred_tcp_s, sizeof(*pred_tcp_s)) != 0 ){
        printf(" Erro connect 'create tcp client' \n    ");
    }
    
}

/*---------------------------------------------------------------------------------------------------------------------
Criação do servidor TCP
---------------------------------------------------------------------------------------------------------------------*/
void create_tcp_server(int *listenfd, char* PORT, struct sockaddr_in *tcp_s ){
    
    /*Criação e verificação da socket*/
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

/*---------------------------------------------------------------------------------------------------------------------
Criação do servidor UDP
---------------------------------------------------------------------------------------------------------------------*/
void create_udp_server(int *udpfd_s, char* PORT, struct sockaddr_in *udp_s){
    struct timeval tv;
    tv.tv_sec=0;
    tv.tv_usec=10000;
    if((*udpfd_s = socket(AF_INET, SOCK_DGRAM, 0))==-1){
        printf("Error: creating UDP server\n");
        exit(0);
    }
    if (setsockopt(*udpfd_s, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){ /*Para evitar o erro no bind*/
        perror("setsockopt(SO_REUSEADDR) failed");
    }
    setsockopt(*udpfd_s, SOL_SOCKET,SO_RCVTIMEO, (const char*)&tv, sizeof(tv)); /*Timer para o UDP */
    memset(udp_s, 0 ,sizeof(*udp_s));
    udp_s->sin_family = AF_INET;
	udp_s->sin_addr.s_addr = htonl(INADDR_ANY);
	udp_s->sin_port = htons(atoi(PORT));
    bind(*udpfd_s, (struct sockaddr*)udp_s, sizeof(*udp_s));
    printf("UDP Server binded\n");
}
/*---------------------------------------------------------------------------------------------------------------------
Criação do cliente UDP
---------------------------------------------------------------------------------------------------------------------*/
void create_udp_client(int *udpfd_c,char *IP, char* PORT, struct sockaddr_in *udp_c){
    *udpfd_c = socket(AF_INET, SOCK_DGRAM, 0);
    struct timeval tv;
    tv.tv_sec=0;
    tv.tv_usec=5000;
    setsockopt(*udpfd_c, SOL_SOCKET,SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
   
    udp_c->sin_family = AF_INET;
    udp_c->sin_addr.s_addr = inet_addr(IP);
    udp_c->sin_port = htons(atoi(PORT));
    if(connect(*udpfd_c, (struct sockaddr *)udp_c, sizeof(*udp_c)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    } 
}

/*---------------------------------------------------------------------------------------------------------------------
Função retorna true se a o "my_ID" se encontra mais próximo da "key_to_f" e false caso contrário
Utilizada na rotina do find e bentry
---------------------------------------------------------------------------------------------------------------------*/

bool check_if_key_is_mine(char *my_ID, char *suc_ID, char *key_to_f){
    
    if(distance(my_ID,key_to_f)>distance(suc_ID, key_to_f)){
        return false;
    }else return true; 
}

/*---------------------------------------------------------------------------------------------------------------------
Função cria um número de sequência para cada procura
Coloca na posição correspondenete a esse número o valor da chave k
---------------------------------------------------------------------------------------------------------------------*/
int get_sequence(find_s *store_finds,int key_to_find){
    static int n_seq=0;
    do{
        if(n_seq ==99) n_seq=-1;
        n_seq ++;
    }while (store_finds[n_seq].key_to_find!=-1);
    store_finds[n_seq].key_to_find=key_to_find;
    store_finds[n_seq].mode = false;
    return n_seq;
}

int main(int argc, char *argv[]){
    
    bool ring_created = false; 
    bool leave = false;      
    char key_to_f[10]; // Vetor utilizado no find
    char trash[5]; // Vetor utilizado no find
    int n_seq=0; // Número de sequência para o find
    
    char buf[MAX_CHAR]; /*Buffer que guarda as mensagens que serão enviadas entre nós*/
    int sret /*return do select*/=0;    
    ring_s ring;
    struct sockaddr_in clinodeaddr, mynodeaddr_tcp_s,
                                    udp_s_addr,  pred_tcp_s, udp_c_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    ssize_t n; 
    int listenfd=0; /*socket listen do tcp server*/
    int connfd  =0; /*socket tcp returned from connect*/
    int udp_c_fd=0; /*socket udp client */
    int udp_s_fd=0; /*socket udp server */
    int tcp_c_fd=0; /*socket tcp client */
    int tcp_s_fd=0; /*socket tcp server */
    int mask_size; /*Número máximo de file descriptors na tabela rset*/
    command_s command; /*Armazenda strings separadas recebidas no buf*/
    fd_set rset, rset_cpy;
    find_s store_finds[100];
    memset(store_finds, -1, sizeof(find_s)*100);
    initialize_ring_memory(&ring); 

    if(valid_arguments(argc - 1, argv + 1, &ring)){
         /* Se a porta ou IP não forem válidos o programa termina*/
        printf("Usage: ./ring i i.IP i.PORT\n");
        exit(0);
    } 

    /*Criação das sockets para listening TCP e os servidores UDP*/
    create_tcp_server(&listenfd, ring.me.PORT, &mynodeaddr_tcp_s);
    create_udp_server(&udp_s_fd, ring.me.PORT, &udp_s_addr);
    
    memset(&command, 0, sizeof(command_s)); /*Inicialização */

    /*Variáveis utilizadas para imprimir a máscara */
    bool print_mask = false; 
    char *fds; 
   
    int alone_in_a_ring=0; /*Indica se o nó está sozinho no anel- 0: sozinho; 1: acompanhado*/

    FD_ZERO(&rset);

    show(&ring, alone_in_a_ring, ring_created);
	
    for (;;) {
        
        if(leave == false){
            FD_SET(0, &rset);
            redefine_mask_size(&rset, &mask_size, listenfd, udp_s_fd, udp_c_fd, connfd,tcp_c_fd);
        }else if(leave == true){
            leave = false;
            listenfd=0;
            udp_c_fd=0;
            connfd=0;
            tcp_c_fd=0;
            tcp_s_fd=0;
            create_tcp_server(&listenfd, ring.me.PORT, &mynodeaddr_tcp_s);
            create_udp_server(&udp_s_fd, ring.me.PORT, &udp_s_addr);
            FD_ZERO(&rset); /*Retiro da máscara a informação das sockets associadas aos nós que sairam do anel*/
            FD_SET(0, &rset);
        }
        
        rset_cpy = rset; 
        
        memset(buf,0,sizeof(char)*100); /*Inicialização do buffer*/
        
        alone_in_a_ring = strcmp(ring.me.PORT, ring.pred.PORT); /*Se o predecessor de um nó for ele próprio isto indica que está sozinho no anel*/
         
        
        if(print_mask){ /*Código utilizado para ver o conteúdo da máscara*/
            fds=(char*)&rset;
            printf("rset: %d\n",  (unsigned char)fds[0]);
        } 

        if((sret =select(mask_size+1, &rset_cpy, NULL, NULL, NULL))==-1){ /*Validação das condições do select*/
            printf("Error in select: %s\n", strerror(errno));
            exit(0);
        }

        if(print_mask){ /*Código utilizado para ver o conteúdo da máscara após o select*/
            fds =(char*)&rset_cpy;
            printf("rset_cpy %d\n",  (unsigned char)fds[0]);
        }
        
        FD_SET(connfd, &rset); /*Colocar a socket connfd na máscara*/
          
        if(FD_ISSET(0, &rset_cpy)){ /*Tratamento dos inputs do utilizador*/
            FD_CLR(0,&rset);
            user_input(buf);           
            if((strcmp(buf, "n") ==0) && (ring_created == false)){ /*new */
                ring_created = true;
                new(&ring);
            }else if((strcmp(buf, "n") ==0) && (ring_created == true)){
                printf("Ring already created\n");
            }else if(strcmp(buf, "s")==0){ /* show */
                show(&ring, alone_in_a_ring, ring_created);
            }else if(buf[0]=='p'){  /*pentry: p i i.IP i.port*/

                if((string_to_command(buf, &command))==1){  /*Separação dos argumentos do pentry*/
                    printf("Given IP in pentry is not valid.\n");
                    continue;
                }

                /*Criação da socket do client (que irá enviar o "PRED")*/
                create_tcp_client(&tcp_c_fd, command.PORT, &pred_tcp_s);
                strcpy(ring.pred.ID,command.ID); /*Preenchimento das informações do predecessor do novo nó com a informação dada no input do pentry*/
                strcpy(ring.pred.IP, command.IP);
                strcpy(ring.pred.PORT, command.PORT);

                /* Envio da mensagem "SELF "do nó j para o nó i (do tcp_c_fd para pred_tcp_s=*/
                sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
                write(tcp_c_fd, buf, strlen(buf));  /* Envio do SELF que será identificado na connfd*/
                leave=false; /*Variável a true apenas quando se faz leave*/

            }else if(strcmp(buf, "l")==0){ /*leave */
                ring_created=0;
                leave=true;
                /* Envio de um "PRED" i i.IP i.port\n ao meu sucessor
                sendo i o meu predecessor */
                sprintf(buf, "%s %s %s %s\n", "PRED",ring.pred.ID,ring.pred.IP,ring.pred.PORT);
                write(tcp_s_fd, buf, strlen(buf));
                 /*Fecho de todas as sockets do nó que deu leave*/
                close_all_sockets(listenfd, udp_c_fd, connfd, tcp_s_fd, tcp_c_fd);
                memset(&ring.pred, 0, sizeof(node));
                memset(&ring.suc, 0, sizeof(node));
                printf("Leave\n");
                
            }else if(buf[0]=='f'){

                //TODO: Check if command is good "trash"
                sscanf(buf, "%s %s", trash, key_to_f);

                if(key_to_f == ring.me.ID){
                    printf("I'm searching for myself\n");
                    continue;
                /* Verificar distância entre o suc e k e entre o próprio nó e k
                A procura será delegada ao nó mais próximo de k */
                }else if(/*not mine!*/!check_if_key_is_mine(ring.me.ID, ring.suc.ID, key_to_f) ){
                    /*Delegar procura ao suc*/
                    if(/*TENHO CHORD*/udp_c_fd!=0){
                        /* Verificar se k está mais próximo do atalho ou do sucessor */
                        if(/*not mine*/!check_if_key_is_mine(ring.suc.ID, ring.chord.ID, key_to_f)){
                            /*k mais próximo do atalho*/
                            memset(buf, 0, sizeof(buf));
                            n_seq = get_sequence(store_finds,atoi(key_to_f));
                            /* Envio de FND key_to_f n_seq ID IP Porta ( do nó gerador da procura)*/
                            /* Envio por UDP */
                            sprintf(buf, "%s %s %d %s %s %s\n", "FND", key_to_f, n_seq, ring.me.ID, ring.me.IP, ring.me.PORT);
                            printf("BUF = %s\n", buf);
                            send(udp_c_fd, buf, strlen(buf),0); 
                            recv(udp_c_fd, buf, 4,0);
                            if(strcmp(buf,"ACK")!=0){
                                printf("Missing ACK when sending FIND to chord(1)\n");
                                strcat(buf,"\n\0");
                                write(tcp_s_fd,buf,strlen(buf)); /* Envio da mensagem por tcp para o sucessor quando ACK é perdido */
                            }
                        }
                    }else{
                        /* Se não tiver chord delego a procura ao suc */
                        memset(buf, 0, sizeof(buf));
                        n_seq = get_sequence(store_finds,atoi(key_to_f));
                        sprintf(buf, "%s %s %d %s %s %s\n", "FND", key_to_f, n_seq, ring.me.ID, ring.me.IP, ring.me.PORT);
                        printf("BUF = %s\n", buf);
                        write(tcp_s_fd, buf, strlen(buf));  
                    }
                    
                }else{ /* o objeto é meu */
                    printf("O objeto é meu\n");
                }
                
                
            }else if (buf[0]=='c') /*chord i.ID i.IP i.Porta*/
            {
                /* Criação do atalho para o nó i por UDP */
                sscanf(buf, "%s %s %s %s", command.opt, command.ID, command.IP, command.PORT);
                create_udp_client(&udp_c_fd, command.IP, command.PORT, &udp_c_addr);
                strcpy(ring.chord.ID,   command.ID); 
                strcpy(ring.chord.IP,   command.IP);
                strcpy(ring.chord.PORT, command.PORT);
                   
            }else if(buf[0]=='b'){ /* bentry j j.IP j.PORT 
                                    sendo j o nó que queremos que entre no anel
                                    cuja posição no mesmo não é conhecida */
                
                sscanf(buf, "%s %s %s %s", command.opt, command.ID, command.IP, command.PORT);
                create_udp_client(&udp_c_fd, command.IP, command.PORT, &udp_c_addr);
                sprintf(buf, "%s %s", "EFND", ring.me.ID);
                printf("BUFF B:%s\n", buf);
                send(udp_c_fd, buf, strlen(buf),0); /*Envio ACK */
                recv(udp_c_fd, buf, 4,0);
                if(strcmp(buf,"ACK")!=0){
                    printf("Missing ACK in bentry, retry\n");
                    strcat(buf,"\n\0");
                    write(tcp_s_fd,buf,strlen(buf));
                    printf("BUFF MISSING: %s\n", buf);
                    close(udp_c_fd); /*Admitimos que este ACK nunca irá chegar */
                    udp_c_fd=0;
                }

            }
            else if (buf[0]=='x') /* exit */
            {
                exit(0);
            }
            else if (buf[0]=='e') /* echord*/
            {
                if (udp_c_fd!=0) /*Eliminação do atalho para um nó caso o mesmo exista*/
                {
                    memset(&ring.chord,0,sizeof(node));
                    close(udp_c_fd);
                    udp_c_fd=0;
                }
            }
            else continue;
            
            
        }
       
        else if(listenfd && (FD_ISSET(listenfd, &rset_cpy))){
            FD_CLR(listenfd, &rset); 
            len = sizeof(struct sockaddr_in);
            connfd = accept(listenfd, (struct sockaddr*)&clinodeaddr, &len);
            printf("O client conectou-se com a socketfd: %d\n", connfd);
        }
                    
        /* Se a socket UDP tem informação para ser lida é recebida a mensagem*/
		else if(udp_c_fd && (FD_ISSET(udp_c_fd, &rset_cpy))){
            //* Recebe o EPRED
            printf("RECEBI O EPRED\n");

            FD_CLR(udp_c_fd, &rset);
            memset(buf,0,sizeof(buf));
            
            recvfrom(udp_c_fd,buf,sizeof(buf) ,0,(struct sockaddr * ) &udp_s_addr, &len);
            sendto(udp_c_fd, "ACK", 4, 0,(struct sockaddr*)&udp_c_addr,len);
            close(udp_c_fd);
            udp_c_fd=0;
            //* EPRED pred pred.IP pred.port
            sscanf(buf, "%s %s %s %s", command.opt, command.ID, command.IP, command.PORT);
            create_tcp_client(&tcp_c_fd, command.PORT, &pred_tcp_s);
            strcpy(ring.pred.ID,command.ID);
            strcpy(ring.pred.IP, command.IP);
            strcpy(ring.pred.PORT, command.PORT);
            /* É enviado o SELF do tcp_c_fd para pred_tcp_s */
            sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
            write(tcp_c_fd, buf, strlen(buf)); /*Envio do "SELF" para a connfd*/
            leave=false;
			
		}

        else if(udp_s_fd && (FD_ISSET(udp_s_fd, &rset_cpy))){

            recvfrom(udp_s_fd, buf, sizeof(buf),0, (struct sockaddr*)&udp_s_addr,&len);
            sendto(udp_s_fd, "ACK", 4,0, (struct sockaddr*)&udp_s_addr,len);
            if(buf[0]=='F'){
                /*Lógica semelhante ao envio destas mensagens TCP 
                Mas estas são enviadas por UDP*/
                sscanf(buf, "%s %s %s %s %s %s", command.opt, command.searched_key, command.n_seq, command.ID, command.IP, command.PORT);
                printf("Recebi um find por UDP\n");
                if(/*MINE*/check_if_key_is_mine(ring.me.ID, ring.suc.ID, command.searched_key)){
                    
                memset(buf, 0, sizeof(char)*100);
                sprintf(buf, "%s %s %s %s %s %s", "RSP", command.ID, command.n_seq, ring.me.ID, ring.me.IP, ring.me.PORT);
                }
                
                if((ring.chord.ID!=0)&&/*chord is closer to key*/(!check_if_key_is_mine(ring.suc.ID, ring.chord.ID, command.searched_key))){
                    send(udp_c_fd, buf, strlen(buf),0);
                    recv(udp_c_fd, buf, 4,0);
                    if(strcmp(buf,"ACK")!=0){
                        printf("Missing ACK when sending FIND to chord(udp_s_fd FIND)\n");
                        strcat(buf,"\n\0");
                        write(tcp_s_fd,buf,strlen(buf));
                    }
                    
                }else{
                    
                    write(tcp_s_fd, buf, strlen(buf)); 
                }
                
               
                
            }else if(buf[0]=='E'){
                printf("External Find!\n");
                sscanf(buf, "%s %s", command.opt, command.searched_key);
                if(command.searched_key == ring.me.ID){
                    printf("It's me I'm looking for\n");
                    continue;
                }else if(/*not mine!*/!check_if_key_is_mine(ring.me.ID, ring.suc.ID, command.searched_key) ){
                    
                    if((/*TENHO CHORD*/udp_c_fd!=0)&&(!check_if_key_is_mine(ring.suc.ID, ring.chord.ID, command.searched_key))){
                            
                            memset(buf, 0, sizeof(buf));
                            n_seq = get_sequence(store_finds, atoi(key_to_f));
                            store_finds[n_seq].mode = 1; //* É um EFND que está presente nesta posição da estrutura
                            inet_ntop(AF_INET, &udp_s_addr.sin_addr, store_finds[n_seq].addr, INET_ADDRSTRLEN);
                            store_finds[n_seq].port=ntohs(udp_s_addr.sin_port);
                            sprintf(buf, "%s %s %d %s %s %s\n", "FND", command.searched_key, n_seq, ring.me.ID, ring.me.IP, ring.me.PORT);
                            printf("BUF = %s\n", buf);
                            send(udp_c_fd, buf, strlen(buf),0);
                            recv(udp_c_fd, buf, 4,0);
                            if(strcmp(buf,"ACK")!=0){
                                printf("Missing ACK when sending FIND to chord(udp_s_fd EFIND)\n");
                                strcat(buf,"\n\0");
                                write(tcp_s_fd,buf,strlen(buf));
                            }
                        
                        
                    }else{
                        //* Se não existir chord a procura é delegada ao suc 
                        memset(buf, 0, sizeof(buf));
                        n_seq = get_sequence(store_finds,atoi(command.searched_key));
                        store_finds[n_seq].mode = 1; //* dizer que é um EFND que está nesta posição da estrutura
                        inet_ntop(AF_INET, &udp_s_addr.sin_addr, store_finds[n_seq].addr, INET_ADDRSTRLEN);
                        store_finds[n_seq].port=(int)ntohs(udp_s_addr.sin_port);
                        sprintf(buf, "%s %s %d %s %s %s\n", "FND", command.searched_key, n_seq, ring.me.ID, ring.me.IP, ring.me.PORT);
                        printf("BUF = %s\n", buf);
                        write(tcp_s_fd, buf, strlen(buf));  
                    }
                    
                }else{ // o objeto é meu
                    sprintf(buf, "%s %s %s %s", "EPRED", ring.me.ID, ring.me.IP, ring.me.PORT);
                
                    if(sendto(udp_s_fd,buf, strlen(buf),0, (const struct sockaddr * ) &udp_s_addr, len)!=-1){
                        recvfrom(udp_s_fd, buf, 4,0,(struct sockaddr * ) &udp_s_addr, &len);

                        if(strcmp(buf,"ACK")!=0){
                            printf("Missing ACK when sending EPRED to chord(2)\n");
                        }
                    }else printf("Error on sendto\n");
                }

            }
            else if (buf[0]=='R'){
                printf("RECEBI UM RSP\n");
                sscanf(buf, "%s %s %s %s %s %s", command.opt, command.searched_key, command.n_seq, command.ID, command.IP, command.PORT);
                if(atoi(command.searched_key)==atoi(ring.me.ID)){
                    if(/*EFND*/store_finds[atoi(command.n_seq)].mode){
                        //printf("RECEBI UM EFND\n");
                        //* Mandar para o nó que mandou o ENFD
                        //* EPRED pred pred.IP pred.PORT
                        sprintf(buf, "%s %s %s %s", "EPRED", command.ID, command.IP, command.PORT);               
                        inet_pton(AF_INET, store_finds[atoi(command.n_seq)].addr, &udp_s_addr.sin_addr);
                       
                        store_finds[atoi(command.n_seq)].key_to_find=-1;
                        udp_s_addr.sin_port = htons(store_finds[atoi(command.n_seq)].port);
                        if(sendto(udp_s_fd,buf, strlen(buf),0, (const struct sockaddr * ) &udp_s_addr, len)!=-1){
                            recvfrom(udp_s_fd, buf, 4,0,(struct sockaddr * ) &udp_s_addr, &len);
                            if(strcmp(buf,"ACK")!=0){
                                printf("Missing ACK when sending EPDRED to chord(3)\n");
                                strcat(buf,"\n\0");
                            }
                        }else printf("Error on sendto\n");
                        
                    }else{ /*Find*/
                        
                        printf("Chave %d: nó %s (%s:%s)\n",store_finds[atoi(command.n_seq)].key_to_find, command.ID, command.IP, command.PORT);
                        store_finds[atoi(command.n_seq)].key_to_find=-1;
                    }
                }else{
                        if((udp_c_fd!=0) &&/*chord is closer to key*/(!check_if_key_is_mine(ring.suc.ID, ring.chord.ID,command.searched_key))){
                        send(udp_c_fd, buf, strlen(buf),0);
                        recv(udp_c_fd, buf, 4,0);
                        if(strcmp(buf,"ACK")!=0){
                            printf("Missing ACK when sending FIND to chord(4)\n");
                            strcat(buf,"\n\0");
                            write(tcp_s_fd,buf,strlen(buf));
                        }
                    }    
                    else write(tcp_s_fd, buf, strlen(buf));
                }
            }
            
            
        }

        else if(connfd && (FD_ISSET(connfd, &rset_cpy))){
            FD_CLR(connfd, &rset);           
            if((n=read(connfd,buf, sizeof(char)*100))!=0    ){
                printf("CONNFD\n");
                if(n==-1){
                    printf("Error in read\n");
                }
                buf[strlen(buf)-1]='\0';
                printf("BUFF = %s\n", buf);

                if(buf[0]=='S'){
                    printf("Recebi um SELF\n");
                    string_to_command(buf, &command);
                   
                        if(ring_created){ /*Anel está criado*/
                            if(alone_in_a_ring != 0){  /*Receção do SELF por parte do nó i que tem um sucessor u diferente do próprio */
                            /* Se o nó não está sozinho no anel é enviado um PRED ao sucessor u do nó i */
                                sprintf(buf, "%s %s %s %s\n", "PRED",command.ID,command.IP,command.PORT);         
                                write(tcp_s_fd, buf, strlen(buf));  
                                close(tcp_s_fd); /*Fecho da ligação entre i e u (s(i))*/
                            
                            }else{ 
                                /* Caso o nó esteja sozinho no anel*/
                               /* O pred e suc  do nó é o mesmo e não é notificado o próprio nó*/
                              /* O nó j atualiza o seu predecessor para i  */
                                
                                strcpy(ring.pred.ID,command.ID);
                                strcpy(ring.pred.IP, command.IP);
                                strcpy(ring.pred.PORT, command.PORT);
                                /*Pentry:Criação de um novo cliente tcp para enviar o "SELF" do nó i para o nó j*/  
                                /*ou Leave: Criação de um novo cliente tcp para enviar o "SELF" do nó s(j) para p(j)*/
        
                                create_tcp_client(&tcp_c_fd,command.PORT, &pred_tcp_s);
                                sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
                                printf("BUFF PENTRY: %s\n", buf);
                                write(tcp_c_fd, buf, strlen(buf));
                                
                            }
                        }else{ /* Se o anel não está criado este é criado*/
                            ring_created = true;
                        
                        }
                         /*Cada um dos nós atualiza o seu sucessor*/
                        strcpy(ring.suc.ID,command.ID);
                        strcpy(ring.suc.IP, command.IP);
                        strcpy(ring.suc.PORT, command.PORT);
                  
                tcp_s_fd = connfd; /*Impede a desconecção com o nó ao que estava ligado para saber informação do antigo
                    , tcp_s_fd liga ao nó novo e connfd liga ao anterior*/
                connfd=0; /*Interrompo a ligação com o nó a que estava ligado para se ligar ao novo nó */
                }
                     
            }else{
                /*Caso não haja mais bytes para ler é fechada a socket connfd*/

                close(connfd);
                connfd=0;
                printf("EOF\n");
            }    
            
        }
        
        else if(tcp_c_fd && (FD_ISSET(tcp_c_fd, &rset_cpy))){
           FD_CLR(tcp_c_fd,&rset);
            if((n=read(tcp_c_fd,buf, sizeof(char)*100))!=0){
                if(n==-1){
                    printf("Error in read\n");
                }

                //*Receção do novo pred nesta mensagem
                
                if(buf[0]=='P'){ /*Receção do "PRED" */
                    string_to_command(buf, &command);
                    printf("Recebi um PRED\n");

                    if(ring_created){
                        
                            /*u (s(i)) atualiza o seu pred para j*/
                            strcpy(ring.pred.ID,command.ID);
                            strcpy(ring.pred.IP, command.IP);
                            strcpy(ring.pred.PORT, command.PORT);
                            close(tcp_c_fd); /*u fecha anel com j */
                            tcp_c_fd=0;
                            int temp_fd; 
                            /*Criação de um novo cliente tcp para envio do "SELF"*/ 
                            create_tcp_client(&temp_fd,command.PORT, &pred_tcp_s);
                            tcp_c_fd=temp_fd;
                            sprintf(buf, "%s %s %s %s\n", "SELF",ring.me.ID,ring.me.IP,ring.me.PORT);
                            write(tcp_c_fd, buf, strlen(buf));
                            /*Em caso de leave:
                            Após este self as sockets são fechadas saindo o nó j do anel pois leave=true*/
                    } 
                }
                else if(buf[0]=='F'){
                    /* FND k n_seq j.ID j.IP j.Porta\n ( j-> originador da procura) */
                    printf("RECEBI UM FND\n");
                    sscanf(buf, "%s %s %s %s %s %s", command.opt, command.searched_key, command.n_seq, command.ID, command.IP, command.PORT);
                    if(/*MINE*/check_if_key_is_mine(ring.me.ID, ring.suc.ID, command.searched_key)){
                        /*Chave mais próxima de mim do que do meu suc*/
                        /* É enviado: RSP j n_seq i.ID i.IP i.Porta\n (i-> originador da resposta) 
                        para o nó encarregado de responder ao nó j */
                        memset(buf, 0, sizeof(char)*100);
                        sprintf(buf, "%s %s %s %s %s %s\n", "RSP", command.ID, command.n_seq, ring.me.ID, ring.me.IP, ring.me.PORT);
                        strcpy(command.searched_key,command.ID);
                    }
                    if((udp_c_fd!=0)&&/*atalho mais perto da chave*/(!check_if_key_is_mine(ring.suc.ID, ring.chord.ID,command.searched_key))){
                        send(udp_c_fd, buf, strlen(buf),0); /* Envio de um ACK por UDP */
                        recv(udp_c_fd, buf, 4,0);
                        if(strcmp(buf,"ACK")!=0){
                            printf("Missing ACK when sending FIND to chord(4)\n");
                            strcat(buf,"\n\0");
                            write(tcp_s_fd,buf,strlen(buf));/* Envio da mensagem por tcp para o sucessor quando ACK é perdido */
                        }
                    }else write(tcp_s_fd, buf, strlen(buf)); /*Envio da mensagem FND para o sucessor */
                    
                }
                else if (buf[0]=='R'){
                    printf("RECEBI UM RSP\n");
                    sscanf(buf, "%s %s %s %s %s %s", command.opt, command.searched_key, command.n_seq, command.ID, command.IP, command.PORT);
                    if(atoi(command.searched_key)==atoi(ring.me.ID)){ /* Se k = ID do ring invocado  */
                        if(/*EFND*/store_finds[atoi(command.n_seq)].mode){ 
                            //printf("RECEBI UM EFND\n")
                            /*Rotina bentry*/
                            /* É enviado um EPRED pred pred.IP pred.PORT para o nó que enviou EFND por UDP */
                            sprintf(buf, "%s %s %s %s", "EPRED", command.ID, command.IP, command.PORT);
                            
                            inet_pton(AF_INET, store_finds[atoi(command.n_seq)].addr, &udp_s_addr.sin_addr);
                            store_finds[atoi(command.n_seq)].key_to_find=-1;
                            /*Passagem do Porto de string para byte network para indicar à máquina o endereço para que irá enviar as infr*/
                            udp_s_addr.sin_port = htons(store_finds[atoi(command.n_seq)].port);
                            if(sendto(udp_s_fd,buf, strlen(buf),0, (const struct sockaddr * ) &udp_s_addr, len)!=-1){
                                recvfrom(udp_s_fd, buf, 4,0,(struct sockaddr * ) &udp_s_addr, &len);
                                if(strcmp(buf,"ACK")!=0){
                                    printf("Missing ACK when sending EPRED to chord(3)\n");
                                    strcat(buf,"\n\0");
                                }
                            }else printf("Error on sendto\n");
                            
                        }else{ /*Mensagem final do Find*/
                            
                            printf("Chave %d: nó %s (%s:%s)\n",store_finds[atoi(command.n_seq)].key_to_find, command.ID, command.IP, command.PORT);
                            store_finds[atoi(command.n_seq)].key_to_find=-1;
                        }
                    }else{
                         if((udp_c_fd!=0) &&/*chord is closer to key*/(!check_if_key_is_mine(ring.suc.ID, ring.chord.ID,command.searched_key))){
                            send(udp_c_fd, buf, strlen(buf),0); /*Envio de ACK*/
                            recv(udp_c_fd, buf, 4,0);
                            if(strcmp(buf,"ACK")!=0){
                                printf("Missing ACK when sending FIND to chord(4)\n");
                                strcat(buf,"\n\0");
                                write(tcp_s_fd,buf,strlen(buf)); /* Envio da mensagem por tcp para o sucessor quando ACK é perdido */
                            }
                        }    
                        else write(tcp_s_fd, buf, strlen(buf));
                    }
                }
            }else{
                close(tcp_c_fd);
                tcp_c_fd=0;
            }
        }


        
    }/*for*/
        
}/*main*/