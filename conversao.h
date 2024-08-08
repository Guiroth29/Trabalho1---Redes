#ifndef CONVERSAO_H
#define CONVERSAO_H

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

// Funções de conversão de tipos
void int64_para_bytes (int64_t valor, unsigned char* bytes);
int bytes_para_int (unsigned char* bytes, int tam);
void int_para_unsigned_char (int valor, unsigned char* bytes, int tam);

#endif // CONVERSAO_H