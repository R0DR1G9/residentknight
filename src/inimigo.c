#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "inimigo.h"

#define VEL_INIMIGO 110.0f
#define DETECCAO 420.0f
#define VEL_TIRO 950.0f
#define PEITO 36.0f
#define VIDA_INIMIGO 60
#define ZUMBI_FRAMES 8
#define ZUMBI_L 384.0f
#define ZUMBI_A 512.0f
#define ZUMBI_ESCALA 0.27f
#define ZUMBI_PE 430.0f
#define ZUMBI_TOPO 85.0f

const Arma ARMAS[NUM_ARMAS] = {
    {"Sem arma", 0, 0, 0.00f, 0.50f},
    {"Pistola", 25, 1, 0.00f, 0.35f},
    {"MP5", 10, 1, 0.06f, 0.09f},
    {"Shotgun", 14, 6, 0.28f, 0.90f}
};

static const int NASCER[8][2] = {
    {14, 6}, {27, 6}, {34, 8}, {30, 5}, {30, 22}, {25, 25}, {35, 28}, {30, 29}
};

static Texture2D texZumbi[ZUMBI_FRAMES];
static Inimigo *inimigos = NULL;
static Tiro *tiros = NULL;
int vivos = 0;
int (*bloqueado_fn)(float x, float y) = NULL;

static int caixa(float x, float y)
{
    return bloqueado_fn(x - 14, y - 10) || bloqueado_fn(x + 14, y - 10) ||
           bloqueado_fn(x - 14, y + 2) || bloqueado_fn(x + 14, y + 2);
}

void inimigos_carregar(void)
{
    char path[100];
    int k;

    for (k = 0; k < ZUMBI_FRAMES; k++) {
        snprintf(path, sizeof(path), "assets/zumbi_frames/zumbi_frame_%02d.png", k + 1);
        texZumbi[k] = LoadTexture(path);
        if (texZumbi[k].id == 0) {
            TraceLog(LOG_ERROR, "Frame do zumbi nao encontrado: %s", path);
        }
    }
}

void inimigos_descarregar(void)
{
    int k;

    for (k = 0; k < ZUMBI_FRAMES; k++) {
        UnloadTexture(texZumbi[k]);
    }
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
        i->vida = VIDA_INIMIGO;
        i->vidaMax = VIDA_INIMIGO;
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
    float w = ZUMBI_L * ZUMBI_ESCALA;
    float h = ZUMBI_A * ZUMBI_ESCALA;

    for (i = inimigos; i != NULL; i = i->prox) {
        float x = i->x;
        float y = i->y;
        int quadro = ((int)(i->anim * 10) % ZUMBI_FRAMES + ZUMBI_FRAMES) % ZUMBI_FRAMES;

        if (y <= yMin || y > yMax) {
            continue;
        }
        DrawEllipse((int)x, (int)y, 19, 6, Fade(BLACK, 0.4f));
        DrawTexturePro(texZumbi[quadro],
                       (Rectangle){0, 0, i->direita ? ZUMBI_L : -ZUMBI_L, ZUMBI_A},
                       (Rectangle){x - w / 2, y - ZUMBI_PE * ZUMBI_ESCALA, w, h},
                       (Vector2){0, 0}, 0, WHITE);
    }
}

void inimigos_desenhar_vidas(Camera2D camera)
{
    Inimigo *i;
    const int larg = 44;
    const int alt = 6;

    for (i = inimigos; i != NULL; i = i->prox) {
        Vector2 p = GetWorldToScreen2D((Vector2){i->x, i->y - (ZUMBI_PE - ZUMBI_TOPO) * ZUMBI_ESCALA - 12}, camera);
        int x = (int)p.x - larg / 2;
        int y = (int)p.y;
        float f = i->vida > 0 ? (float)i->vida / i->vidaMax : 0.0f;
        Color cor = f > 0.5f ? GREEN : (f > 0.25f ? ORANGE : RED);
        const char *txt = TextFormat("%d", i->vida > 0 ? i->vida : 0);

        DrawRectangle(x - 1, y - 1, larg + 2, alt + 2, BLACK);
        DrawRectangle(x, y, larg, alt, (Color){60, 20, 20, 255});
        DrawRectangle(x, y, (int)(larg * f), alt, cor);
        DrawText(txt, (int)p.x - MeasureText(txt, 10) / 2, y - 12, 10, RAYWHITE);
    }
}

void tiros_desenhar(void)
{
    Tiro *t;

    for (t = tiros; t != NULL; t = t->prox) {
        DrawLineEx((Vector2){t->x - t->vx * 0.02f, t->y - t->vy * 0.02f}, (Vector2){t->x, t->y}, 4, YELLOW);
    }
}
