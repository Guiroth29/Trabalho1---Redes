#ifndef PILHA_H
#define PILHA_H

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

typedef struct {
    int head;
    char posicao [4000][64];
} t_pilha;

int add_pilha (t_pilha *pilha, const char *str);
int busca_pilha (t_pilha *pilha, char *buffer);

#endif