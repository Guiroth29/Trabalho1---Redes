#include "mensagem.h"

void inicia_mensagem (mensagem_t *enviado)  {
    memset (enviado, 0, sizeof (mensagem_t));
    memset (enviado->dados, 0, 64);
}

mensagem_t monta_mensagem (unsigned char tam, unsigned char sequencia, unsigned char tipo, unsigned char* dados, int crc_flag) {
    mensagem_t enviado;
    
    enviado.inicio = 0x7E; //  (0111 1110)
    enviado.tamanho = tam;
    enviado.sequencia = sequencia;
    enviado.tipo = tipo;

    if  (dados != NULL)
        memcpy (enviado.dados, dados, tam);

    // caso seja necessário, calcula o crc
    if  (crc_flag) {
        enviado.crc8 = calcula_crc (&enviado);
    } else {
        enviado.crc8 = 0x00;
    }

    return enviado;
}

void print_bits (unsigned char byte, int num_bits, FILE *arq) {
    for  (int i = num_bits - 1; i >= 0; --i)
        fprintf (arq, "%d",  (byte >> i) & 1);
}

void print_mensagem (mensagem_t *enviado, FILE *arq) {
    fprintf (arq, "\nMarcador de inicio: ");
    print_bits (enviado->inicio, 8, arq);
    fprintf (arq, "\n");

    fprintf (arq, "Tamanho: ");
    print_bits (enviado->tamanho, 6, arq);
    fprintf (arq, "\n");

    fprintf (arq, "Sequência: ");
    print_bits (enviado->sequencia, 5, arq);
    fprintf (arq, "\n");

    fprintf (arq, "Tipo: ");
    print_bits (enviado->tipo, 5, arq);
    fprintf (arq, "\n");

    fprintf (arq, "Dados: ");
    for  (int i = 0; i < 64-1; ++i) {
        print_bits (enviado->dados[i], 8, arq);
    }
    fprintf (arq, "\n");

    fprintf (arq, "Crc-8: ");
    print_bits (enviado->crc8, 8, arq);
    fprintf (arq, "\n\n");
}

unsigned char calcula_crc (mensagem_t *enviado) {
    unsigned char gerador = 0x07;
    unsigned char crc = 0x00;

    for  (unsigned int i = 0; i < enviado->tamanho; i++) {
        crc ^= enviado->dados[i];

        for  (unsigned int j = 0; j < 8; j++) {
            if  (crc & 0x80) {
                crc =  (crc << 1) ^ gerador;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}