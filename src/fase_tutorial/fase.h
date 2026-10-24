#ifndef FASE_H
#define FASE_H

#include "raylib.h"

#define PX 4
#define TAM_TILE (16 * PX)
#define COLUNAS 42
#define LINHAS 34
#define LARGURA_TELA 1024
#define ALTURA_TELA 640

typedef struct Objeto Objeto;
typedef struct Sala Sala;
typedef struct Corredor Corredor;
typedef struct Fase Fase;

typedef void (*DesenharObjeto)(Objeto *o, float tempo);
typedef void (*DesenharJogador)(float x, float y, float escala);

struct Objeto {
    int col;
    int lin;
    int larg;
    int alt;
    int solido;
    int plano;
    int luz;
    int variante;
    DesenharObjeto desenhar;
    Objeto *proximo;
};

struct Sala {
    int id;
    const char *nome;
    const char *dica;
    int col;
    int lin;
    int larg;
    int alt;
    Sala *proxima;
};

struct Corredor {
    int col;
    int lin;
    int larg;
    int alt;
    Corredor *proximo;
};

struct Fase {
    char **mapa;
    Sala *salas;
    Corredor *corredores;
    Objeto *objetos;
    Sala *atual;
    float inicioX;
    float inicioY;
    float portalX;
    float portalY;
    float tempo;
    int terminou;
    float tempoFim;
    float camX;
    float camY;
    int camIniciada;
    Sala *salaHud;
    float tempoSalaHud;
    RenderTexture2D luzes;
    int luzesPronta;
};

Fase *fase_criar(void);
void fase_liberar(Fase *f);

int fase_bloqueado(Fase *f, float x, float y);
int fase_caixa_bloqueada(Fase *f, float x, float y, float meiaLarg, float acima, float abaixo);

void fase_atualizar(Fase *f, float x, float y, float dt);
void fase_puxar_para_portal(Fase *f, float *x, float *y, float dt);
void cena_desenhar(Fase *f, float x, float y, DesenharJogador desenhar_jogador);

void obj_caixote(Objeto *o, float tempo);
void obj_barril(Objeto *o, float tempo);
void obj_barril_fogo(Objeto *o, float tempo);
void obj_sacos(Objeto *o, float tempo);
void obj_bloco(Objeto *o, float tempo);
void obj_placa(Objeto *o, float tempo);
void obj_sangue(Objeto *o, float tempo);
void obj_carro(Objeto *o, float tempo);
void obj_maquina(Objeto *o, float tempo);
void obj_bau(Objeto *o, float tempo);
void obj_lampada_parede(Objeto *o, float tempo);
void obj_poste(Objeto *o, float tempo);
void obj_faixa(Objeto *o, float tempo);
void obj_pilar(Objeto *o, float tempo);
void obj_portal(Objeto *o, float tempo);

#endif
