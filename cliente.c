// Bibliotecas prontas
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>

// Bibiotecas criadas
# include "rede.h"
# include "verificacao.h"
# include "conversao.h"
# include "mensagem.h"
# include "pilha.h"

# define SAIR -100

// ============================= FUNÇÕES CLIENTE =========================================================================

void processar_mensagens_lista(int sockfd, t_pilha *pilha, mensagem_t *recebido, unsigned char *sequencia) {
    while (!verifica_fim(recebido)) {
        if (verifica_dados(recebido)) {
            if (recebido->sequencia == *sequencia) {
                if (verifica_crc(recebido)) {
                    if (!busca_pilha(pilha, (char*)recebido->dados)) {
                        add_pilha(pilha, (char*)recebido->dados);
                    }
                    if (!manda_ack(sockfd, *sequencia)) {
                        exit(EXIT_FAILURE);
                    }
                    *sequencia = (*sequencia + 1) % 32;
                } else {
                    if (!manda_nack(sockfd, *sequencia)) {
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }

        if (recv(sockfd, recebido, sizeof(*recebido), 0) < 0) {
            perror("Erro no recebimento!");
            exit(EXIT_FAILURE);
        }
    }
}

void imprimir_opcoes_filmes(t_pilha *pilha) {
    printf("\n\n *** FILMES ***\n\n====================\n\n");

    for (int aux = 0; aux <= pilha->head; aux++) {
        printf(" %d) %s\n-----------\n", aux + 1, pilha->posicao[aux]);
    }

    printf("\n\n====================\n\n %d) SAIR\n\n====================\n\n", pilha->head + 2);
}

int processar_download(int sockfd, mensagem_t *recebido, mensagem_t *enviado, t_pilha *pilha, unsigned char *sequencia) {
    int erro = 0;
    int escolha;
    FILE *arquivo;
    erro = 0;

    strcpy((char*)enviado->dados, (char*)recebido->dados);

    // Usuário seleciona o filme ou escolhe sair do programa
    printf("\n Escolha: ");
    scanf("%d", &escolha);

    while (escolha < 1 || escolha > pilha->head + 2) {
        printf("\n OPCAO INVALIDA\n");

        printf("\n Escolha:");
        scanf("%d", &escolha);
    }

    if (escolha == pilha->head + 2) { // Opção de Sair
        if (system("rm -rf downloads") != 0) {
            perror("Erro ao excluir o diretório de downloads e seus arquivos");
            return -1;
        }
        printf("\n\n");
        return SAIR;
    }

    DIR *dir = opendir("downloads");
    if (dir) {
        closedir(dir);
    } else if (ENOENT == errno) {
        if (mkdir("downloads", 0777) == -1) {
            perror("Erro ao criar o diretório de downloads");
            return -1;
        }
    } else {
        perror("Erro ao verificar o diretório de downloads");
        return -1;
    }

    strncpy((char*)enviado->dados, pilha->posicao[escolha - 1], 64 - 1);
    enviado->inicio = 0x7E;
    enviado->tamanho = (unsigned char)strlen(pilha->posicao[escolha - 1]);
    enviado->sequencia = *sequencia;
    enviado->tipo = 0x0B;
    enviado->crc8 = calcula_crc(enviado);
    *sequencia = (*sequencia + 1) % 32;

    if (send(sockfd, enviado, sizeof(*enviado), 0) < 0) {
        perror("Erro no envio:");
        return -1;
    }

    if (espera_ack(sockfd, enviado, 1000) == 0)
        return -1;

    arquivo = fopen(pilha->posicao[escolha - 1], "wb");
    if (!arquivo) {
        perror("Erro ao abrir/criar o arquivo");
        return -1;
    }

    if (recv(sockfd, recebido, sizeof(*recebido), 0) < 0) {
        perror("Erro no recebimento:");
        return -1;
    }

    while (!verifica_fim(recebido)) {
        if (verifica_erros(recebido)) {
            erro = 1;
            if (!manda_ack(sockfd, *sequencia))
                return -1;
            *sequencia = (*sequencia + 1) % 32;
            break;
        } else if (verifica_descricao(recebido)) {
            if (recebido->sequencia == *sequencia) {
                if (verifica_crc(recebido)) {
                    if (!manda_ack(sockfd, *sequencia))
                        return -1;
                    *sequencia = (*sequencia + 1) % 32;
                } else {
                    if (!manda_nack(sockfd, *sequencia))
                        return -1;
                }
            }
        } else if (verifica_dados(recebido)) {
            if (recebido->sequencia == *sequencia) {
                if (verifica_crc(recebido)) {
                    fwrite(recebido->dados, sizeof(unsigned char), recebido->tamanho, arquivo);
                    if (!manda_ack(sockfd, *sequencia))
                        return -1;
                    *sequencia = (*sequencia + 1) % 32;
                } else {
                    if (!manda_nack(sockfd, *sequencia))
                        return -1;
                }
            } else if (*sequencia == 0) {
                if (recebido->sequencia == 31) {
                    if (!manda_ack(sockfd, (unsigned char)31))
                        return -1;
                }
            } else if (recebido->sequencia == *sequencia - 1) {
                if (!manda_ack(sockfd, *sequencia - 1))
                    return -1;
            }
        }

        if (recv(sockfd, recebido, sizeof(*recebido), 0) < 0) {
            perror("Erro no recebimento:");
            return -1;
        }
    }

    fclose(arquivo);

    if (!erro) {
        if (!manda_ack(sockfd, *sequencia))
            return -1;

        *sequencia = (*sequencia + 1) % 32;

        printf("\n DOWNLOAD CONCLUÍDO!\n\n");

        char comando[256];
        setenv("XDG_RUNTIME_DIR", "/run/user/1000", 1);
        snprintf(comando, sizeof(comando), "mpv --fullscreen --no-terminal %s", pilha->posicao[escolha - 1]);
        system(comando);

        if (chmod("downloads", 0777) == -1) {
            perror("Erro ao alterar as permissões do diretório de downloads");
            return -1;
        }

        char novoCaminho[256];
        snprintf(novoCaminho, sizeof(novoCaminho), "downloads/%s", pilha->posicao[escolha - 1]);
        if (rename(pilha->posicao[escolha - 1], novoCaminho) != 0) {
            perror("Erro ao mover o arquivo para o diretório de downloads");
            return -1;
        }
    }

    return 0;
}

// ======================= MAIN =================================================================================

int main (int argc, char *argv []) {

    // Variáveis
    unsigned char dados [64]; // 1 byte = 8 bits, 64 bytes = 512 bits
    unsigned char sequencia;
    mensagem_t recebido;
    mensagem_t enviado;
    t_pilha pilha;
    int sockfd;

    // Inicializa
    inicia_mensagem (&enviado);
    inicia_mensagem (&recebido);
    pilha.head = -1;
    sequencia = 0;

    // Cria Raw Socket
    sockfd = cria_raw_socket ("enp3s0f4u1");
    if  (sockfd == -1)
        return -1;

    memset (dados, 0, 64);
    enviado = monta_mensagem (0x00, sequencia, 0x0A, dados, 0);
    sequencia =  (sequencia + 1) % 32;

    // Envia o frame com tipo == LISTA
    if  (send (sockfd, &enviado, sizeof (enviado), 0) < 0)
        return -1;

    if  (espera_ack (sockfd, &enviado, 1000) == 0)
        return -1;

    // Começa a receber os dados referentes ao comando "lista"
    if  (recv (sockfd, &recebido, sizeof (recebido), 0) < 0)
        return -1;

    // Chamar a função para processar as mensagens até receber "fim"
    processar_mensagens_lista (sockfd, &pilha, &recebido, &sequencia);

    if  (!manda_ack (sockfd, sequencia))
        return -1;

    sequencia =  (sequencia + 1) % 32;

    while  (1) {
        system ("clear"); // Limpa a tela
        imprimir_opcoes_filmes(&pilha);
        if (processar_download(sockfd, &recebido, &enviado, &pilha, &sequencia)) {
            close (sockfd);
            return 0;
        }
    }
}