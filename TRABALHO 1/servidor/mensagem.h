#ifndef MENSAGEM_H
#define MENSAGEM_H

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

// Estrutura do enviado
typedef struct {
    unsigned char inicio: 8;
    unsigned char tamanho: 6;
    unsigned char sequencia: 5;
    unsigned char tipo: 5;
    unsigned char dados[64];
    unsigned char crc8: 8;
} mensagem_t;

void inicia_mensagem (mensagem_t *enviado); // Preenche todo o enviado com 0's
mensagem_t monta_mensagem (unsigned char tam, unsigned char sequencia, unsigned char tipo, unsigned char* dados, int crc_flag);
unsigned char calcula_crc(mensagem_t *enviado); // Calcula o CRC-8 da mensagem e o retorna
int verifica_crc(mensagem_t *enviado); // Detecta erros nos dados a partir do crc

#endif // MENSAGEM_H