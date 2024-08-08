Streaming de vídeo baseado no protocolo definido em aula, descrito a seguir.



1. Descrição:

O sistema desenvolvido consiste em dois elementos, um cliente e um servidor. Podem ser o mesmo
programa, com parâmetros de chamada especificando a funcionalidade, ou dois programas
especializados em cada papel.

O servidor e o cliente devem executar em máquinas distintas, conectadas por um cabo de rede.
Uma vez conectados e iniciados cliente e servidor, no cliente é mostrada uma lista de títulos de
vídeos disponíveis no servidor, em mp4 e/ou avi.

O usuário, no cliente, pode escolher um dos vídeos, a partir da escolha o vídeo é
transferido para o cliente. Após a transferência o cliente chama um player para a exibição iniciar.

==========================================================================================

2. Detalhes de implementação:

As máquinas devem operar em modo root (pode fazer boot por pendrive).

As máquinas devem estar interligadas por cabo de rede diretamente.

Comunicação baseada em raw sockets.

A linguagem precisa ser C ou C++ devido ao raw socket.

É necessário tratar timeouts e possíveis erros e falhas na comunicação.

O protocolo é inspirado no Kermit.

Campos do frame devem ser unsigned.

O controle de fluxo deve ser por janela deslizante, go-back N (volta N), com tamanho de janela fixo
em 5 mensagens nas transmissões de arquivos e em uma mensagem (para-espera) nas demais
mensagens.

O nome de cada arquivo pode ter no máximo 63 bytes, possuindo
somente caracteres ASCII na faixa de 0x20 0x7E.

Os tipos de mensagens (campo “Tipo”) são:
00000 ack
00001 nack
01010 lista
01011 baixar
10000 mostra na tela
10001 descritor arquivo
10010 dados
11110 fim tx
11111 erro

Os erros reportados podem ser:
1 acesso negado
2 não encontrado
3 disco cheio

==========================================================================================

3. ANTES DE RODAR O PROGRAMA:

1) 2 computadores com boot por USB ou 2 computadores com Root (se quiser fazer com só um para testar muda o "eno1" por exemplo pra "lo").
2) "sudo dnf install make gcc ncurses-devel openh264 -y"
3) "sudo dnf install mpv ou modificar para xdg-open"
4) os dois conectados entre si apenas por um cabo de rede. Nenhum dos dois precisa estar conectado à internet.
5) "make"
6) em uma máquina faz "sudo ./server" e na outra "sudo ./client"

==========================================================================================

4. OPCIONAL:

1- "ifconfig" para ver o tipo (lo, eno1, eth0, etc...) e daí alterar em client.c e server.c na parte de "cria_raw_socket()"

2 - "touch *", depois make clean purge e por fim make de novo caso dê diferença entre os clocks das máquinas

3 - "id -u" caso precise mudar o UID
