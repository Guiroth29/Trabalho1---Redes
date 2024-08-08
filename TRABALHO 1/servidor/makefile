CFLAGS := -Wall
LDFLAGS := -lncurses

all: servidor

servidor: rede.o servidor.o mensagem.o verificacao.o conversao.o
	gcc -o servidor rede.o servidor.o mensagem.o verificacao.o conversao.o $(LDFLAGS)

rede.o: rede.h rede.c
	gcc -c rede.c $(CFLAGS)

mensagem.o: mensagem.h mensagem.c
	gcc -c mensagem.c $(CFLAGS)

verificacao.o: verificacao.h verificacao.c
	gcc -c verificacao.c $(CFLAGS)

conversao.o: conversao.h conversao.c
	gcc -c conversao.c $(CFLAGS)

servidor.o: servidor.c
	gcc -c servidor.c $(CFLAGS)

clean:
	rm -f rede.o servidor.o mensagem.o verificacao.o conversao.o *~

purge: clean
	-rm -f servidor
