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
RING_NAME = ring
TCP_CLIENT_NAME = tcpclient
UDP_CLIENT_NAME= udpclient
PROG_NAME = $(RING_NAME) 
# Nomes para as fontes e objetos 

RING_OBJS = ring.o new.o
RING_SOURCES = ring.c new.o

TCP_CLIENT_SOURCES = A_tcp_client.c

UDP_CLIENT_SOURCES = A_UDPClient.c

################################################################################
################################################################################
.PHONY: default  tcpclient udpclient udpc tcpc
default: $(PROG_NAME)
udpc: udpclient
tcpc: tcpclient
udpclient:$(UDP_CLIENT_NAME)
tcpclient:$(TCP_CLIENT_NAME)

################################################################################
# Compilacao e geracao de executaveis                               
################################################################################ 

$(RING_NAME):  $(RING_OBJS)
	$(CC) $(MODE) -L. $(CFLAGS) -o $@ $(RING_OBJS) 


ring.o: $(RING_SOURCES)
	$(CC) $(MODE) $(CFLAGS) -c $< -o $@ 

$(TCP_CLIENT_NAME):  $(TCP_CLIENT_SOURCES)
	$(CC) $(MODE)  $(CFLAGS) A_tcp_client.c -o tcpclient 

$(UDP_CLIENT_NAME):  $(UDP_CLIENT_SOURCES)
	$(CC) $(MODE)  $(CFLAGS) A_UDPClient.c -o udpclient  



################################################################################
# limpeza de ficheiros inerentes a geracao do executavel  
################################################################################
.PHONY: clean cleanall
 
cleanall: clean cleanExe  

clean:
	rm -rf $(C_TRASH)  

cleanExe:
	rm -rf $(PROG_NAME)

