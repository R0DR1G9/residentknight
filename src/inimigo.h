#ifndef INIMIGO_H
#define INIMIGO_H

#include "raylib.h"

#define NUM_ARMAS 4

typedef struct Arma {
    const char *nome;
    int dano, balas;
    float espalhar, cadencia;
} Arma;

typedef struct Inimigo {
    float x, y, anim, ataque;
    int vida, vidaMax, direita;
    struct Inimigo *prox;
} Inimigo;

typedef struct Tiro {
    float x, y, vx, vy, tempo;
    int dano;
    struct Tiro *prox;
} Tiro;

extern const Arma ARMAS[NUM_ARMAS];
extern int vivos;
extern int (*bloqueado_fn)(float x, float y);

void inimigos_carregar(void);
void inimigos_descarregar(void);
void inimigos_criar(void);
void inimigos_liberar(void);
void atirar(int arma, float x, float y, float alvoX, float alvoY);
int inimigos_atualizar(float px, float py, float dt);
void inimigos_desenhar(float yMin, float yMax);
void inimigos_desenhar_vidas(Camera2D camera);
void tiros_desenhar(void);

#endif
