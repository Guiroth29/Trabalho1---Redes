# include "pilha.h"

// Adiciona um elemento na pilha
int add_pilha (t_pilha *pilha, const char *str) {
    pilha->head++;
    strncpy(pilha->posicao[pilha->head], str, 63);
    pilha->posicao[pilha->head][63] = '\0';
    return 0;
}

// Busca um elemento na pilha
int busca_pilha (t_pilha *pilha, char *buffer) {
    for (int i = 0; i <= pilha->head; i++) {
        if (strcmp(buffer, pilha->posicao[i]) == 0) {
            return 1;
        }
    }
    return 0;
}