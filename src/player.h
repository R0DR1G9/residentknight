#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct {
    Vector2 pos;
    int vida, arma, direita;
    float tempoAnim, tempoTiro, invencivel;
} Jogador;

void PlayerUpdate(Vector2 *player, float speed);

#endif