# include "conversao.h"

void int64_para_bytes (int64_t valor, unsigned char* bytes)  {
    for  (int i = 0; i < 8; i++)
        bytes[i] =  (unsigned char) ( (valor >>  (i * 8)) & 0xFF);
}

int bytes_para_int (unsigned char* bytes, int tam)  {
    int valor = 0;
    for  (int i = 0; i < tam; i++)
        valor |=  (int)bytes[i] <<  (i * 8);

    return valor;
}

void int_para_unsigned_char (int valor, unsigned char* bytes, int tam)  {
    for  (int i = 0; i < tam; i++)
        bytes[i] =  (valor >>  (i * 8)) & 0xFF;
}