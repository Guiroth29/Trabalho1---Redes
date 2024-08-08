// Biblioteca criada
#include "rede.h"
#include "conversao.h"

int main (int argc, char *argv []) {

    // Variaveis
    int sockfd, bytes_prontos, retorno_ACK, erro, tipo_erro;
    char buffer [64], caminho [64 + 9];
    unsigned char dados [64], sequencia;
    unsigned char bytes [8];
    char versaoVideo [32];
    mensagem_t recebido;
    mensagem_t enviado;
    struct stat st;
    FILE *arquivo;

    // Inicializa
    sequencia = 0;
    retorno_ACK = 0;
    erro = 0;
    inicia_mensagem (&enviado);
    inicia_mensagem (&recebido);

    printf ("SERVIDOR ONLINE\n\n");

    // Cria raw Socket
    sockfd = cria_raw_socket ("eno1");
    if  (sockfd == -1) {
        printf ("CRIACAO DO SOCKET FALHA");
        return -1;
    }

    // Recebe os dados
    if  (recv (sockfd, &enviado, sizeof (enviado), 0) < 0)
        return -1;

    while  (1) {

        // recebeu uma lista
        if  (verifica_lista (&enviado)) {

            // ACK
            if  (!manda_ack (sockfd, sequencia))
                return -1;

            sequencia =  (sequencia + 1) % 32;
            
            // procura nomes dos filmes
            struct dirent *entry;

            DIR *dir = opendir (FILMES_PATH);
            if  (dir == NULL)
                return 1;

            while  ( (entry = readdir (dir))) {
                if  (strcmp (entry->d_name, ".") && strcmp (entry->d_name, "..")) {
                    // Envia o nome do filme
                    strncpy ( (char*)recebido.dados, entry->d_name, 64-1);

                    recebido.inicio = 0x7E;
                    recebido.tamanho =  (unsigned char)strlen (entry->d_name);
                    recebido.sequencia = sequencia;
                    recebido.tipo = 0x12;
                    recebido.crc8 = calcula_crc (&recebido);

                    sequencia =  (sequencia + 1) % 32;

                    if  (send (sockfd, &recebido, sizeof (recebido), 0) < 0)
                        return -1;

                    if  (espera_ack (sockfd, &recebido, 1000) == 0)
                        return -1;
                }
            }

            closedir (dir);

            // Prepara fim
            memset (dados, 0, 64);
            recebido = monta_mensagem (0x00, sequencia, 0x1E, dados, 0);
            sequencia =  (sequencia + 1) % 32;

            // Envia fim
            if  (send (sockfd, &recebido, sizeof (recebido), 0) < 0)
                return -1;

            if  (espera_ack (sockfd, &recebido, 1000) == 0)
                return -1;
        }
        
        // recebe BAIXAR
        else if  (verifica_baixar (&enviado)) {
            if  (verifica_crc (&enviado)) {
                // ACK
                if  (!manda_ack (sockfd, sequencia))
                    return -1;

                sequencia =  (sequencia + 1) % 32;

                strncpy (buffer,  (char*)enviado.dados, 64-1);
                snprintf (caminho, 64 + 9, "%s%s", FILMES_PATH, buffer);

                printf ("DOWNLOAD EM ANDAMENTO!\n");

                arquivo = fopen (caminho, "rb");
                if  (!arquivo) {
                    erro = 1;

                    if  (errno == EACCES) {
                        printf ("ERRO DE PERMISSAO\n");
                        tipo_erro = ACESSO_NEGADO;
                    }
                    else if  (errno == ENOENT) {
                        printf ("ARQUIVO NAO ENCONTRADO\n");
                        tipo_erro = NAO_EXISTE;
                    }
                    else {
                        return -1;
                    }
                }

                if  (erro) {
                    unsigned char bytes [4];
                    int_para_unsigned_char (tipo_erro, bytes, sizeof (int));

                    // Envia erro
                    recebido = monta_mensagem (sizeof (bytes), sequencia, 0x1F, bytes, 1);
                    sequencia =  (sequencia + 1) % 32;
                    if  (send (sockfd, &recebido, sizeof (recebido), 0) < 0)
                        return -1;

                    if  (espera_ack (sockfd, &recebido, 1000) == 0)
                            return -1;
                }

                if  (!erro) {
                    // Envia 'descritor_arquivo' 

                    // vê tamanho do filme
                    stat (caminho, &st);
                    int64_t tamVideo = st.st_size;

                    // envia tamanho do filme
                    int64_para_bytes (tamVideo, bytes);
                    recebido = monta_mensagem (sizeof (bytes), sequencia, 0x11, bytes, 1);
                    sequencia =  (sequencia + 1) % 32;
                    if  (send (sockfd, &recebido, sizeof (recebido), 0) < 0)
                        return -1;

                    if  (espera_ack (sockfd, &recebido, 1000) == 0)
                        return -1;

                    // vê modificação
                    strftime (versaoVideo, sizeof (versaoVideo), "%Y-%m-%d %H:%M:%S", localtime (&st.st_mtime));

                    // envia modificação
                    strncpy ( (char*)recebido.dados, versaoVideo, 64-1);

                    recebido.inicio = 0x7E;
                    recebido.tamanho =  (unsigned char)strlen (versaoVideo);
                    recebido.sequencia = sequencia;
                    recebido.tipo = 0x11;
                    recebido.crc8 = calcula_crc (&recebido);

                    sequencia =  (sequencia + 1) % 32;

                    // Envia tamanho do filme
                    if  (send (sockfd, &recebido, sizeof (recebido), 0) < 0)
                        return -1;

                    if  (espera_ack (sockfd, &recebido, 1000) == 0)
                        return -1;

                    while ( (bytes_prontos = fread (dados, sizeof (unsigned char), 64-1, arquivo)) > 0) {

                        // envia dados do filme
                        recebido = monta_mensagem ( (unsigned char)bytes_prontos, sequencia, 0x12, dados, 1);
                        sequencia =  (sequencia + 1) % 32;

                        if  (send (sockfd, &recebido, sizeof (recebido), 0) < 0)
                            return -1;

                        retorno_ACK = espera_ack (sockfd, &recebido, 1000);

                        if  (retorno_ACK == 0)
                            return -1;
                        else if  (retorno_ACK == 2)
                            break;
                    }

                    fclose (arquivo);

                    if  (retorno_ACK == 2) {
                        while  (!verifica_erros (&enviado)) {
                            if  (recv (sockfd, &enviado, sizeof (enviado), 0) < 0)
                                return 0;
                        }

                        // ACK
                        if  (!manda_ack (sockfd, sequencia))
                            return -1;
                        sequencia =  (sequencia + 1) % 32;

                        retorno_ACK = 0;
                    }
                    else {
                        // Envia fim
                        memset (dados, 0, 64);
                        recebido = monta_mensagem (0x00, sequencia, 0x1E, dados, 0);
                        sequencia =  (sequencia + 1) % 32;
                        if  (send (sockfd, &recebido, sizeof (recebido), 0) < 0)
                            return -1;

                        if  (espera_ack (sockfd, &recebido, 1000) == 0)
                            return -1;
                    }

                    fprintf (stdout, "DOWNLOAD CONCLUÍDO!\n\n");
                }
                erro = 0;
            }
            else {
                if  (!manda_nack (sockfd, sequencia))
                    return -1;
            }
        }

        if  (recv (sockfd, &enviado, sizeof (enviado), 0) < 0)
            return -1;
    }
    close (sockfd);
    return 0;
}