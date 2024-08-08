#include "rede.h"

int cria_raw_socket (char* nome_interface_rede) { 
    // Cria raw socket
    int soquete = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
    if  (soquete == -1) { 
        fprintf (stderr, "VOCE NAO É ROOT\n");
        exit (-1); 
    }

    int ifindex = if_nametoindex (nome_interface_rede);

    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons (ETH_P_ALL);
    endereco.sll_ifindex = ifindex;

    if  (bind (soquete,  (struct sockaddr *)&endereco, sizeof (endereco)) == -1) {
        perror ("Erro ao fazer bind no socket\n");
        exit (-1);
    }

    struct packet_mreq mr = {0};
    mr.mr_ifindex = ifindex;
    mr.mr_type = PACKET_MR_PROMISC;

    // Não joga fora o que identifica como lixo)
    if  (setsockopt (soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof (mr)) == -1) {
        fprintf (stderr, "Interface de rede errada\n");
        exit (-1);
    }

    return soquete;
}

int manda_ack (int sockfd, unsigned char sequencia) {
    mensagem_t enviado;
    unsigned char dados[64];

    // Prepara a mensagem de retorno
    memset (dados, 0, 64);
    enviado = monta_mensagem (0x00, sequencia, 0x00, dados, 0);

    // Envia ACK
    if  (send (sockfd, &enviado, sizeof (enviado), 0) < 0) {
        perror ("Erro no envio:");
        return 0;
    }
    
    return 1;
}

int manda_nack (int sockfd, unsigned char sequencia) {
    mensagem_t enviado;
    unsigned char dados[64];

    // Prepara a mensagem de retorno
    memset (dados, 0, 64);
    enviado = monta_mensagem (0x00, sequencia, 0x01, dados, 0);

    // Envia NACK
    if  (send (sockfd, &enviado, sizeof (enviado), 0) < 0) {
        perror ("Erro no envio:");
        return 0;
    }

    return 1;
}

long long timestamp () {
    struct timeval tp;
    gettimeofday (&tp, NULL);
    return  (long long)tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int espera_ack (int sockfd, mensagem_t * mensagem_envio, int tempo_timeout) {
    mensagem_t  mensagem_recebimento;
    long long marcador_tempo = 0, tempo_inicial = timestamp ();

    // Aguarda ACK
    if  (recv (sockfd, & mensagem_recebimento, sizeof ( mensagem_recebimento), 0) < 0) {
        perror ("Erro no recebimento:");
        return 0;
    }

    if  (verifica_erros (& mensagem_recebimento))
        return 2;

    while  (! (verifica_ack (& mensagem_recebimento) &&  mensagem_recebimento.sequencia ==  mensagem_envio->sequencia) &&  (marcador_tempo = timestamp () - tempo_inicial) < tempo_timeout) {
        if  (verifica_nack (& mensagem_recebimento) &&  mensagem_recebimento.sequencia ==  mensagem_envio->sequencia) {
            if  (send (sockfd,  mensagem_envio, sizeof (* mensagem_envio), 0) < 0) {
                perror ("Erro no envio:");
                return 0;
            }

            // Reinicia o timeout
            tempo_inicial = timestamp ();
        }

        // Situações decorrentes do não recebimento do ACK pela contraparte:
        // -> Client pede lista, server manda o ACK mas client não recebe...
        // -> Server envia um fim, client manda o ACK mas server não recebe...
        // -> Client pede para baixar, server manda o ACK mas client não recebe...
        if  (verifica_lista (& mensagem_recebimento) || verifica_fim (& mensagem_recebimento) ||  (verifica_baixar (& mensagem_recebimento) && verifica_dados ( mensagem_envio))) {
            if  ( mensagem_envio->sequencia == 0) {
                if  (!manda_ack (sockfd,  (unsigned char)31))
                    return 0;
            }
            else {
                if  (!manda_ack (sockfd,  mensagem_envio->sequencia - 1))
                    return 0;
            }

            if  (send (sockfd,  mensagem_envio, sizeof (* mensagem_envio), 0) < 0) {
                perror ("Erro no envio:");
                return 0;
            }

            // Reinicia o timeout
            tempo_inicial = timestamp ();
        }

        if  (recv (sockfd, & mensagem_recebimento, sizeof ( mensagem_recebimento), 0) < 0) {
            perror ("Erro no recebimento:");
            return 0;
        }

        if  (verifica_erros (& mensagem_recebimento))
            return 2;
    }

    // Se atingiu tempo limite reenvia a mensagem
    if  (marcador_tempo >= tempo_timeout) {
        if  (send (sockfd,  mensagem_envio, sizeof (* mensagem_envio), 0) < 0) {
            perror ("Erro no envio:");
            return 0;
        }

        if  (!espera_ack (sockfd,  mensagem_envio, tempo_timeout * 2))
            return 0;
    }

    return 1;
}