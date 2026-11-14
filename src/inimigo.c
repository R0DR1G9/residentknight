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
#define ZUMBI_ESCALA 0.27f
#define ZUMBI_PE 430.0f
#define ZUMBI_TOPO 85.0f
#define ZUMBI_PE_ATAQUE 412.0f
#define GOLPE_TEMPO 0.8f
#define GOLPE_ACERTO 0.4f
#define ALCANCE_GOLPE 45
#define BOSS_VIDA 600
#define BOSS_VEL 85.0f
#define BOSS_DANO 25
#define BOSS_ALCANCE 85
#define BOSS_ESCALA 0.25f
#define BOSS_PE 672.0f
#define BOSS_TOPO 24.0f
#define BOSS_GOLPE_TEMPO 0.9f
#define BOSS_GOLPE_ACERTO 0.5f
#define BOSS_MORTE_TEMPO 0.22f

const Arma ARMAS[NUM_ARMAS] = {
    {"Sem arma", 0, 0, 0.00f, 0.50f},
    {"Pistola", 25, 1, 0.00f, 0.35f},
    {"MP5", 10, 1, 0.06f, 0.09f},
    {"Shotgun", 14, 6, 0.28f, 0.90f}
};

static const int NASCER[8][2] = {
    {14, 6}, {27, 6}, {34, 8}, {30, 5}, {30, 22}, {25, 25}, {35, 28}, {26, 29}
};

static Texture2D texZumbi[ZUMBI_FRAMES];
static Texture2D texAtaque[ZUMBI_FRAMES];
static Texture2D texBossAndar[ZUMBI_FRAMES];
static Texture2D texBossAtaque[ZUMBI_FRAMES];
static Texture2D texBossMorte[ZUMBI_FRAMES];
static Inimigo *inimigos = NULL;
static Tiro *tiros = NULL;
int vivos = 0;
int (*bloqueado_fn)(float x, float y) = NULL;

static int caixa(float x, float y)
{
    return bloqueado_fn(x - 14, y - 10) || bloqueado_fn(x + 14, y - 10) ||
           bloqueado_fn(x - 14, y + 2) || bloqueado_fn(x + 14, y + 2);
}

static Texture2D frame(const char *pasta, const char *nome, int k)
{
    char path[120];
    Texture2D t;

    snprintf(path, sizeof(path), "assets/%s/%s_%02d.png", pasta, nome, k + 1);
    t = LoadTexture(path);
    if (t.id == 0) {
        TraceLog(LOG_ERROR, "Frame nao encontrado: %s", path);
    }
    return t;
}

void inimigos_carregar(void)
{
    int k;

    for (k = 0; k < ZUMBI_FRAMES; k++) {
        texZumbi[k] = frame("zumbi_frames", "zumbi_frame", k);
        texAtaque[k] = frame("zumbi_ataque_frames", "ataque_zumbi_frame", k);
        texBossAndar[k] = frame("boss_frames", "boss_frame", k);
        texBossAtaque[k] = frame("boss_ataque_frames", "boss_ataque_frame", k);
        texBossMorte[k] = frame("boss_morte_frames", "boss_morte_frame", k);
    }
}

void inimigos_descarregar(void)
{
    int k;

    for (k = 0; k < ZUMBI_FRAMES; k++) {
        UnloadTexture(texZumbi[k]);
        UnloadTexture(texAtaque[k]);
        UnloadTexture(texBossAndar[k]);
        UnloadTexture(texBossAtaque[k]);
        UnloadTexture(texBossMorte[k]);
    }
}

static void novo(float x, float y, int vida, int boss)
{
    Inimigo *i = malloc(sizeof(Inimigo));

    if (i == NULL) {
        return;
    }
    i->x = x;
    i->y = y;
    i->anim = vivos;
    i->ataque = 0.5f;
    i->golpe = 0;
    i->morte = -1;
    i->acertou = 0;
    i->vida = vida;
    i->vidaMax = vida;
    i->direita = 0;
    i->boss = boss;
    i->prox = inimigos;
    inimigos = i;
    vivos++;
}

void inimigos_criar(void)
{
    int k;

    for (k = 0; k < 8; k++) {
        novo(NASCER[k][0] * 64 + 32, NASCER[k][1] * 64 + 56, VIDA_INIMIGO, 0);
    }
    novo(31 * 64 + 32, 29 * 64 + 56, BOSS_VIDA, 1);
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
                float raio = i->boss ? 42 : 26;
                float dx = t->x - i->x;
                float dy = t->y - (i->y - (i->boss ? 70 : PEITO));

                if (i->vida > 0 && dx * dx + dy * dy < raio * raio) {
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

        float alcance = i->boss ? BOSS_ALCANCE : ALCANCE_GOLPE;
        float tempo = i->boss ? BOSS_GOLPE_TEMPO : GOLPE_TEMPO;
        float acerto = i->boss ? BOSS_GOLPE_ACERTO : GOLPE_ACERTO;
        float vel = i->boss ? BOSS_VEL : VEL_INIMIGO;
        float visao = i->boss ? 560 : DETECCAO;

        if (i->vida <= 0) {
            if (i->boss && i->morte < 0) {
                i->morte = 0;
                vivos--;
            }
            if (i->boss) {
                i->morte += dt;
            }
            continue;
        }
        i->ataque -= dt;
        if (i->golpe > 0) {
            i->golpe -= dt;
            if (!i->acertou && i->golpe <= acerto) {
                i->acertou = 1;
                if (d < alcance + 15) {
                    dano += i->boss ? BOSS_DANO : 10;
                }
            }
            continue;
        }
        if (d < visao) {
            i->direita = dx >= 0;
        }
        if (d < visao && d > (i->boss ? 50 : 30)) {
            float vx = dx / d * vel * dt;
            float vy = dy / d * vel * dt;

            if (!caixa(i->x + vx, i->y)) {
                i->x += vx;
            }
            if (!caixa(i->x, i->y + vy)) {
                i->y += vy;
            }
            i->anim += dt;
        }
        if (d < alcance && i->ataque <= 0) {
            i->golpe = tempo;
            i->acertou = 0;
            i->ataque = i->boss ? 1.8f : 1.1f;
        }
    }

    mover_tiros(dt);

    p = &inimigos;
    while (*p != NULL) {
        if ((*p)->vida <= 0 && !(*p)->boss) {
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

    for (i = inimigos; i != NULL; i = i->prox) {
        float x = i->x;
        float y = i->y;
        int quadro = ((int)(i->anim * 10) % ZUMBI_FRAMES + ZUMBI_FRAMES) % ZUMBI_FRAMES;

        if (y <= yMin || y > yMax) {
            continue;
        }
        Texture2D tex = texZumbi[quadro];
        float pe = ZUMBI_PE;
        float esc = ZUMBI_ESCALA;

        if (i->boss) {
            pe = BOSS_PE;
            esc = BOSS_ESCALA;
            if (i->morte >= 0) {
                tex = texBossMorte[(int)fminf(ZUMBI_FRAMES - 1, i->morte / BOSS_MORTE_TEMPO)];
            } else if (i->golpe > 0) {
                tex = texBossAtaque[(int)((BOSS_GOLPE_TEMPO - i->golpe) / BOSS_GOLPE_TEMPO * ZUMBI_FRAMES) % ZUMBI_FRAMES];
            } else {
                tex = texBossAndar[quadro];
            }
        } else if (i->golpe > 0) {
            tex = texAtaque[(int)((GOLPE_TEMPO - i->golpe) / GOLPE_TEMPO * ZUMBI_FRAMES) % ZUMBI_FRAMES];
            pe = ZUMBI_PE_ATAQUE;
        }
        DrawEllipse((int)x, (int)y, i->boss ? 44 : 19, i->boss ? 13 : 6, Fade(BLACK, 0.4f));
        DrawTexturePro(tex,
                       (Rectangle){0, 0, i->direita ? (float)tex.width : -(float)tex.width, (float)tex.height},
                       (Rectangle){x - tex.width * esc / 2, y - pe * esc, tex.width * esc, tex.height * esc},
                       (Vector2){0, 0}, 0, WHITE);
    }
}

void inimigos_desenhar_vidas(Camera2D camera)
{
    Inimigo *i;
    const int alt = 6;

    for (i = inimigos; i != NULL; i = i->prox) {
        float topo = i->boss ? (BOSS_PE - BOSS_TOPO) * BOSS_ESCALA : (ZUMBI_PE - ZUMBI_TOPO) * ZUMBI_ESCALA;
        Vector2 p = GetWorldToScreen2D((Vector2){i->x, i->y - topo - 12}, camera);
        int larg = i->boss ? 120 : 44;
        int x = (int)p.x - larg / 2;
        int y = (int)p.y;
        float f = i->vida > 0 ? (float)i->vida / i->vidaMax : 0.0f;
        Color cor = f > 0.5f ? GREEN : (f > 0.25f ? ORANGE : RED);
        const char *txt = TextFormat("%d", i->vida > 0 ? i->vida : 0);

        if (i->vida <= 0 && i->boss) {
            continue;
        }

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
