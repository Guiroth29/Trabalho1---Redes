#ifndef VERIFICACAO_H
#define VERIFICACAO_H

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

# include "rede.h"
# include "mensagem.h"

// Funções de análise de mensagens
int verifica_ack (mensagem_t *enviado); // Analisa se a mensagem é válida e se é um ACK
int verifica_nack (mensagem_t *enviado); // Analisa se a mensagem é válida e se é um NACK
int verifica_lista (mensagem_t *enviado); // Analisa se a mensagem é válida e se é uma lista
int verifica_baixar (mensagem_t *enviado); // Analisa se a mensagem é válida e se é um BAIXAR
int verifica_descricao (mensagem_t *enviado); // Analisa se a mensagem é válida e se é um DESCRITOR ARQUIVO
int verifica_dados (mensagem_t *enviado); // Analisa se a mensagem é válida e se é um DADOS
int verifica_erros (mensagem_t *enviado); // Analisa se a mensagem é válida e se é um ERRO
int verifica_fim (mensagem_t *enviado); // Analisa se a mensagem é válida e se é um fim

#endif