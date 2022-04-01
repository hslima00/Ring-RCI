#include "header.h"

void new(ring_s *ring) { //cria o anel com um nó com s=self 
    //TODO: Não precisa de meter servers à mistura
    //char buf[MAX_CHAR];
    //bzero(buf, sizeof(buf));
    //buf[strlen(buf)-1]='\0'
    printf("Executa a função new()\n");
    strcpy(ring->pred.ID,ring->me.ID);
    strcpy(ring->pred.PORT,ring->me.PORT);
    strcpy(ring->pred.IP,ring->me.IP);
    printf("ID %s\n", ring->me.ID);
    printf("IP %s\n", ring->me.IP);
    printf("PORT %s\n", ring->me.PORT);

    strcpy(ring->suc.ID,ring->me.ID);
    strcpy(ring->suc.PORT,ring->me.PORT);
    strcpy(ring->suc.IP,ring->me.IP);
}

        

    
    
   

