CFLAGS := -Wall
LDFLAGS := -lncurses

all: cliente

cliente: rede.o cliente.o mensagem.o verificacao.o conversao.o pilha.o
	gcc -o cliente rede.o cliente.o mensagem.o verificacao.o conversao.o pilha.o $(LDFLAGS)

rede.o: rede.h rede.c
	gcc -c rede.c $(CFLAGS)

pilha.o: pilha.h pilha.c
	gcc -c pilha.c $(CFLAGS)

mensagem.o: mensagem.h mensagem.c
	gcc -c mensagem.c $(CFLAGS)

verificacao.o: verificacao.h verificacao.c
	gcc -c verificacao.c $(CFLAGS)

conversao.o: conversao.h conversao.c
	gcc -c conversao.c $(CFLAGS)

cliente.o: cliente.c
	gcc -c cliente.c $(CFLAGS)

clean:
	rm -f rede.o cliente.o mensagem.o verificacao.o conversao.o pilha.o *~

purge: clean
	-rm -f cliente
