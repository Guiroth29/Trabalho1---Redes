# include "verificacao.h"

int verifica_crc (mensagem_t *enviado) {
    if  (enviado->crc8 == calcula_crc (enviado))
        return 1;
    
    return 0;
}

int verifica_ack (mensagem_t *enviado) {
    if  (enviado->inicio != 0x7E || enviado->tipo != 0x00)
        return 0;
    return 1;
}

int verifica_nack (mensagem_t *enviado) {
    if  (enviado->inicio != 0x7E || enviado->tipo != 0x01)
        return 0;
    return 1;
}

int verifica_lista (mensagem_t *enviado) {
    if  (enviado->inicio != 0x7E || enviado->tipo != 0x0A)
        return 0;
    return 1;
}

int verifica_baixar (mensagem_t *enviado) {
    if  (enviado->inicio != 0x7E || enviado->tipo != 0x0B)
        return 0;
    return 1;
}

int verifica_descricao (mensagem_t *enviado) {
    if  (enviado->inicio != 0x7E || enviado->tipo != 0x11)
        return 0;
    return 1;
}

int verifica_dados (mensagem_t *enviado) {
    if  (enviado->inicio != 0x7E || enviado->tipo != 0x12)
        return 0;
    return 1;
}

int verifica_erros (mensagem_t *enviado) {
    if  (enviado->inicio != 0x7E || enviado->tipo != 0x1F)
        return 0;
    return 1;
}

int verifica_fim (mensagem_t *enviado) {
    if  (enviado->inicio != 0x7E || enviado->tipo != 0x1E)
        return 0;
    return 1;
}