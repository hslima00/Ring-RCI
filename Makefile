################################################################################
# Makefile
# Descrição:
# forma simples e intuitiva de compilar o código, de fácil edição.
################################################################################
# Nomes de variaveis usadas ao longo do Makefile 
################################################################################

# variaveis do compilador
CC = gcc
MODE = -g
CFLAGS = -Wall -pedantic -Wextra

# Variaveis para limpeza de ficheiros inerentes a geracao do executavel 
C_TRASH = *.o *.so
OBJS_CTRASH = $(addprefix , $(C_TRASH))
SO_CTRASH = $(addprefix , $(C_TRASH))

# Nomes para os executaveis 
SERVER_NAME = ring


PROG_NAME = $(SERVER_NAME)  
# Nomes para as fontes e objetos 
SERVER_OBJS = ring_network.o new.o servers_and_client_creation.o
SERVER_SOURCES =ring_network.c new.c servers_and_client_creation.c header.h



################################################################################
################################################################################
.PHONY: default server SERVER
default: $(PROG_NAME)

server: SERVER
SERVER: $(SERVER_NAME)



################################################################################
# Compilacao e geracao de executaveis                               
################################################################################
$(SERVER_NAME):  $(SERVER_OBJS) 
	$(CC) $(MODE)  $(CFLAGS) -o $@  $(SERVER_OBJS) 


server.o: $(SERVER_SOURCES)
	$(CC) $(MODE) $(CFLAGS) -c $< -o $@ 



################################################################################
# limpeza de ficheiros inerentes a geracao do executavel  
################################################################################
.PHONY: cleanall
 
cleanall: clean cleanExe  

clean:
	rm -rf $(C_TRASH)  

cleanExe:
	rm -rf $(PROG_NAME) clientt clientu server clientm

clientt: 
	gcc tcp_client.c -o clientt
clientu:
	gcc udp_client.c -o clientu