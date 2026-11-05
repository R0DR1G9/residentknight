#include <stdlib.h>
#include <math.h>
#include "inimigo.h"

#define VEL_INIMIGO 110.0f
#define DETECCAO 420.0f
#define VEL_TIRO 950.0f
#define PEITO 36.0f

const Arma ARMAS[NUM_ARMAS] = {
    {"Sem arma", 0, 0, 0.00f, 0.50f},
    {"Pistola", 25, 1, 0.00f, 0.35f},
    {"MP5", 10, 1, 0.06f, 0.09f},
    {"Shotgun", 14, 6, 0.28f, 0.90f}
};

static const int NASCER[8][2] = {
    {14, 6}, {27, 6}, {34, 8}, {30, 5}, {30, 22}, {25, 25}, {35, 28}, {30, 29}
};

static Inimigo *inimigos = NULL;
static Tiro *tiros = NULL;
int vivos = 0;
int (*bloqueado_fn)(float x, float y) = NULL;

static int caixa(float x, float y)
{
    return bloqueado_fn(x - 14, y - 10) || bloqueado_fn(x + 14, y - 10) ||
           bloqueado_fn(x - 14, y + 2) || bloqueado_fn(x + 14, y + 2);
}

void inimigos_criar(void)
{
    int k;

    for (k = 0; k < 8; k++) {
        Inimigo *i = malloc(sizeof(Inimigo));

        if (i == NULL) {
            return;
        }
        i->x = NASCER[k][0] * 64 + 32;
        i->y = NASCER[k][1] * 64 + 56;
        i->anim = k;
        i->ataque = 0.5f;
        i->vida = 60;
        i->direita = 0;
        i->prox = inimigos;
        inimigos = i;
        vivos++;
    }
}

void inimigos_liberar(void)
{
    while (inimigos != NULL) {
        Inimigo *p = inimigos->prox;
        free(inimigos);
        inimigos = p;
    }
    while (tiros != NULL) {
        Tiro *p = tiros->prox;
        free(tiros);
        tiros = p;
    }
    vivos = 0;
}

void atirar(int arma, float x, float y, float alvoX, float alvoY)
{
    float ang = atan2f(alvoY - y, alvoX - x);
    int k;

    for (k = 0; k < ARMAS[arma].balas; k++) {
        float a = ang + GetRandomValue(-100, 100) / 100.0f * ARMAS[arma].espalhar;
        Tiro *t = malloc(sizeof(Tiro));

        if (t == NULL) {
            return;
        }
        t->x = x + cosf(ang) * 18;
        t->y = y + sinf(ang) * 18;
        t->vx = cosf(a) * VEL_TIRO;
        t->vy = sinf(a) * VEL_TIRO;
        t->tempo = 0.8f;
        t->dano = ARMAS[arma].dano;
        t->prox = tiros;
        tiros = t;
    }
}

static void mover_tiros(float dt)
{
    Tiro **p = &tiros;

    while (*p != NULL) {
        Tiro *t = *p;
        int passos = (int)(VEL_TIRO * dt / 10) + 1;
        int fim = 0;
        int s;

        t->tempo -= dt;
        fim = t->tempo <= 0;
        for (s = 0; s < passos && !fim; s++) {
            Inimigo *i;

            t->x += t->vx * dt / passos;
            t->y += t->vy * dt / passos;
            fim = bloqueado_fn(t->x, t->y + PEITO);
            for (i = inimigos; i != NULL && !fim; i = i->prox) {
                float dx = t->x - i->x;
                float dy = t->y - (i->y - PEITO);

                if (i->vida > 0 && dx * dx + dy * dy < 26 * 26) {
                    i->vida -= t->dano;
                    fim = 1;
                }
            }
        }
        if (fim) {
            *p = t->prox;
            free(t);
        } else {
            p = &t->prox;
        }
    }
}

int inimigos_atualizar(float px, float py, float dt)
{
    Inimigo *i;
    Inimigo **p;
    int dano = 0;

    for (i = inimigos; i != NULL; i = i->prox) {
        float dx = px - i->x;
        float dy = py - i->y;
        float d = sqrtf(dx * dx + dy * dy);

        i->ataque -= dt;
        if (d < DETECCAO) {
            i->direita = dx >= 0;
        }
        if (d < DETECCAO && d > 30) {
            float vx = dx / d * VEL_INIMIGO * dt;
            float vy = dy / d * VEL_INIMIGO * dt;

            if (!caixa(i->x + vx, i->y)) {
                i->x += vx;
            }
            if (!caixa(i->x, i->y + vy)) {
                i->y += vy;
            }
            i->anim += dt;
        }
        if (d < 40 && i->ataque <= 0) {
            dano += 10;
            i->ataque = 1;
        }
    }

    mover_tiros(dt);

    p = &inimigos;
    while (*p != NULL) {
        if ((*p)->vida <= 0) {
            Inimigo *morto = *p;
            *p = morto->prox;
            free(morto);
            vivos--;
        } else {
            p = &(*p)->prox;
        }
    }
    return dano;
}

void inimigos_desenhar(float yMin, float yMax)
{
    Inimigo *i;
    float u = 3.1f;

    for (i = inimigos; i != NULL; i = i->prox) {
        float x = i->x;
        float y = i->y;
        float d = i->direita ? 1 : -1;
        float s = sinf(i->anim * 9);
        float q = fabsf(s) * u;

        if (y <= yMin || y > yMax) {
            continue;
        }
        DrawEllipse((int)x, (int)y, 19, 6, Fade(BLACK, 0.4f));
        DrawRectangle(x - 5 * u + s * 2 * u, y - 8 * u, 4 * u, 8 * u, (Color){46, 50, 62, 255});
        DrawRectangle(x + u - s * 2 * u, y - 8 * u, 4 * u, 8 * u, (Color){46, 50, 62, 255});
        DrawRectangle(x - 6 * u, y - 18 * u - q, 12 * u, 11 * u, (Color){74, 72, 66, 255});
        DrawRectangle(d > 0 ? x + 3 * u : x - 14 * u, y - 17 * u - q, 11 * u, 3 * u, (Color){118, 156, 104, 255});
        DrawRectangle(x - 5 * u, y - 27 * u - q, 10 * u, 9 * u, (Color){118, 156, 104, 255});
        DrawRectangle(x + d * 3 * u - u, y - 24 * u - q, 2 * u, 2 * u, RED);
    }
}

void tiros_desenhar(void)
{
    Tiro *t;

    for (t = tiros; t != NULL; t = t->prox) {
        DrawLineEx((Vector2){t->x - t->vx * 0.02f, t->y - t->vy * 0.02f}, (Vector2){t->x, t->y}, 4, YELLOW);
    }
}
