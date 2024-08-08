#ifndef REDE_H
#define REDE_H

// Includes de bibliotecas padrão
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/statvfs.h>

// Includes de bibliotecas específicas de rede
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <net/if.h>

#include "mensagem.h"
#include "verificacao.h"

// Definições de constantes
#define FILMES_PATH "./Filmes/"
#define ACESSO_NEGADO 1
#define NAO_EXISTE 2

// Funções de comunicação entre cliente e servidor
int cria_raw_socket (char* nome_interface_rede); // Meio de comunicar entre cliente e servidor
int manda_ack (int sockfd, unsigned char sequencia); // Envia um ACK através do socket
int manda_nack (int sockfd, unsigned char sequencia); // Envia um NACK através do socket
int espera_ack (int sockfd, mensagem_t * mensagem_envio, int tempo_timeout); // Espera com timeout e trata NACK. return 1=recebeu, 0=problema, 2=ERRO

#endif // REDE_H