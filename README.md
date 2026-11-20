# Resident Night

Jogo de ação em C com a biblioteca [raylib](https://www.raylib.com), visto de cima, com a mecânica e o mundo inspirados em Soul Knight e a identidade visual pós-apocalíptica de Resident Evil.

Você controla o Leon por uma instalação abandonada, cheia de zumbis e com um chefe no fim. Quando todos os inimigos forem derrotados, o portal se abre. Ao entrar nele, o jogo termina com os créditos e o placar de melhores tempos.

Este é o tutorial do jogo que pretendemos desenvolver: Um mapa de 8 salas ligadas por corredores.

Vídeo de demonstração: link a ser adicionado.

## Como compilar e rodar

Precisa de um compilador C (gcc ou clang), do `make` e do raylib com o `pkg-config`.

macOS (Homebrew):

    brew install raylib pkg-config

Linux: instale o raylib pelo gerenciador de pacotes da sua distribuição (ou compile a partir do site oficial) e o `pkg-config`.

Windows: use o WSL ou uma máquina virtual Linux, como pede o enunciado.

Depois, na raiz do repositório:

    make run

O `make` compila todos os `.c` de `src/` e gera o executável `src/jogo`. O jogo precisa ser executado a partir da pasta `src/` (o `make run` já faz isso), porque procura as imagens, o áudio e o mapa em `assets/`. O arquivo `src/placar.txt` é criado sozinho na primeira vez que alguém zera o jogo.

## Controles

| Tecla | Ação |
|---|---|
| W A S D | andar |
| Mouse (botão esquerdo) ou ESPAÇO | atirar na direção do mouse |
| 1, 2, 3, 4 | trocar de arma (sem arma, pistola, MP5, shotgun) |
| E | entrar no portal, quando estiver aberto e por perto (aparecerá na sala do boss) |
| ENTER | recomeçar depois de morrer, ou selecionar no menu |
| M | voltar ao menu depois de morrer |
| F1 | mostrar a posição do Leon no mapa (tile) |
| ESC | sair |

O menu permite escolher a dificuldade. No modo difícil, os inimigos são mais rápidos e causam mais dano.

## Placar

O tempo da partida conta desde o início até o Leon entrar no portal. Os 5 melhores tempos ficam guardados em `src/placar.txt` e aparecem na tela de créditos, com a sua posição destacada.

## Conceitos da disciplina e onde estão

| Conceito | Onde |
|---|---|
| Structs | `Jogador` (`player.h`), `Arma`, `Inimigo` e `Tiro` (`inimigo.h`), `Registro` (`placar.h`), `Jogo`, `Recursos` e `LinhaCredito` (`main.c`) |
| Ponteiros | passagem de structs por ponteiro (`Jogo *`, `Jogador *`, `Recursos *`), ponteiro duplo na remoção da lista de inimigos e de tiros, ponteiro de função `bloqueado_fn` |
| Alocação dinâmica | `novo` e `atirar` (`inimigo.c`), `mapa_carregar` (`mapa.c`), `inserir` (`placar.c`), sempre com `free` correspondente |
| Listas encadeadas | inimigos e tiros (`inimigo.c`), com remoção durante o percurso, e o placar ordenado (`placar.c`) |
| Matrizes | grade de colisão `char **` carregada de `assets/colisao.txt` (`mapa.c`) e os vetores 2D de texturas por arma (`main.c`) |
| Leitura e escrita em arquivo | `mapa.c` lê a colisão do mapa, e `placar.c` lê e grava o `placar.txt` |

## Estrutura

    Makefile
    src/
      main.c        menu, laço do jogo, câmera, luz, HUD, portal e créditos
      player.c/.h   movimento do jogador
      inimigo.c/.h  zumbis, chefe, armas e tiros
      mapa.c/.h     colisão do mapa
      placar.c/.h   placar de melhores tempos
      assets/       mapa, sprites, iluminação e áudio

## Créditos

Equipe: Arthur Siqueira, Rodrigo Barbosa e João Miguel.

Música: "Horror Atmosphere", de SubspaceAudio (faixa Juhani Junkala - Post Apocalyptic Wastelands), licença CC0, em https://opengameart.org/content/horror-atmosphere. O arquivo foi recomprimido para ficar menor.
