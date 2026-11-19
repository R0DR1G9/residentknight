#ifndef PLACAR_H
#define PLACAR_H

#define PLACAR_MAX 5
#define PLACAR_ARQUIVO "placar.txt"

typedef struct Registro {
    float tempo;
    struct Registro *prox;
} Registro;

void placar_carregar(void);
int placar_registrar(float tempo);
int placar_quantidade(void);
float placar_tempo(int posicao);
void placar_liberar(void);

#endif
