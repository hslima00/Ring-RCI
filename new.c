#include "header.h"

int new(ring_s *ring){ //cria o anel com um nó com s=self 
    //TODO: Não precisa de meter servers à mistura
    //char buf[MAX_CHAR];
    //bzero(buf, sizeof(buf));
    if(ring->me.ID==ring->pred.ID){
        printf("Ring already created\n");
        return 0;
    }else{
        
        //connect(tcp_c_fd, (struct sockaddr*)&mynodeaddr_tcp_s,
							//sizeof(mynodeaddr_tcp_s));
        //strcpy(buf, "connecting to self\n");
        //write(tcp_c_fd, buf, sizeof(buf));
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