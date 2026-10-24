# Fase Tutorial

As salas do Resident Knight: três salas ligadas por dois corredores, num cenário pós-apocalíptico em pixel art. Não tem inimigos, tiro nem sprite de personagem. A fase só cuida do mapa, dos objetos, da luz e do portal que encerra a fase. O personagem é o do grupo, e a fase só precisa saber onde ele está e como desenhá-lo.

## Como rodar o teste

    cd src/fase_tutorial
    make
    ./tutorial

No Windows (MinGW): `make tutorial-win` e depois `tutorial.exe`.

O `main_tutorial.c` é só uma demonstração, com uma bolinha azul no lugar do personagem. Controles: W A S D para andar e ESC para sair. Depois de entrar no portal, ENTER fecha o jogo. Ele pode ficar de fora do commit se não for necessário.

## Arquivos

| Arquivo | Conteúdo |
|---|---|
| `fase.h` | structs (com typedef), constantes e protótipos |
| `fase.c` | criação do mapa, listas encadeadas, colisão e liberação da memória |
| `fase_desenho.c` | desenho do cenário, dos objetos, das luzes e do HUD |
| `main_tutorial.c` | demonstração de como usar a fase |

## Assuntos usados

- struct e typedef: `Fase`, `Sala`, `Corredor` e `Objeto`
- Listas encadeadas: salas, corredores e objetos da fase
- Ponteiros e alocação dinâmica: o mapa é uma matriz `char **` criada com `malloc`, e cada nó das listas também. Tudo é liberado em `fase_liberar`. `fase_puxar_para_portal` altera a posição do personagem por ponteiro
- Ponteiro de função: cada `Objeto` guarda a função que o desenha (`DesenharObjeto`), e quem integra passa a função que desenha o personagem (`DesenharJogador`)
- Estruturas de repetição: geração das paredes, desenho dos tiles e percurso das listas

## Como o mapa é montado

1. `fase_criar` cria a matriz do mapa cheia de espaços vazios.
2. Cada sala e cada corredor é um retângulo (coluna, linha, largura, altura em tiles). Eles são guardados nas listas e "esculpidos" na matriz como chão (`.`).
3. `gerar_paredes` transforma em parede (`#`) todo espaço vazio vizinho de um chão.
4. Os objetos são criados e, se forem sólidos, marcam seus tiles como obstáculo (`o`).

A colisão só consulta a matriz: só se anda onde há `.`.

## Como usar com o personagem do grupo

Nesta fase, `x` e `y` são a posição dos pés do personagem, em pixels do mundo. O que o `main.c` precisa fazer:

    Fase *fase = fase_criar();

    void desenhar_personagem(float x, float y, float escala)
    {
        ...
    }

Dentro do laço do jogo, depois de mover o personagem (o `player.c` atual não tem colisão, então volte a posição se bater, eixo por eixo):

    Vector2 antes = player;
    PlayerUpdate(&player, speed);
    if (fase_caixa_bloqueada(fase, player.x, antes.y, 16.0f, 10.0f, 2.0f)) {
        player.x = antes.x;
    }
    if (fase_caixa_bloqueada(fase, player.x, player.y, 16.0f, 10.0f, 2.0f)) {
        player.y = antes.y;
    }
    fase_atualizar(fase, player.x, player.y, GetFrameTime());

    BeginDrawing();
    cena_desenhar(fase, player.x, player.y, desenhar_personagem);
    EndDrawing();

E no fim do programa, `fase_liberar(fase)` antes de `CloseWindow()`.

- `cena_desenhar` chama `desenhar_personagem` no momento certo, para o personagem ficar na frente ou atrás dos objetos conforme a posição.
- `escala` vai de 1 até 0 enquanto o personagem é puxado pelo portal. Quem quiser pode ignorar, ou usar para encolher o sprite.
- Quando `fase->terminou` vale 1, o personagem entrou no portal. Pare o movimento, chame `fase_puxar_para_portal(fase, &x, &y, dt)` e feche o jogo depois de `fase->tempoFim` passar de 2 segundos.
- A tela da fase usa `LARGURA_TELA` e `ALTURA_TELA` (1024 por 640) de `fase.h`. Se o jogo abrir a janela em outro tamanho, mude essas duas constantes.

## Como adicionar uma sala

Em `fase_criar` (`fase.c`):

1. Chame `sala_add` com id, nome, dica e o retângulo da sala.
2. Chame `corredor_add` para ligar a sala às outras.
3. Coloque objetos com `add_solido` (bloqueia), `add_chao` (enfeite no chão) ou `add_luz` (emite luz).
4. Se o mapa crescer, aumente `COLUNAS` e `LINHAS` em `fase.h`.

Para criar um objeto novo, escreva uma função com a assinatura `void obj_nome(Objeto *o, float tempo)` em `fase_desenho.c`, declare em `fase.h` e passe o nome dela para `add_solido`, `add_chao` ou `add_luz`.
