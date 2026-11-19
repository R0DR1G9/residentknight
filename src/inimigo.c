#include <stdlib.h>
#include <math.h>
#include "inimigo.h"
#include "mapa.h"

#define VEL_TIRO 950.0f
#define PEITO 36.0f
#define FRAMES 8
#define MORTE_TEMPO 0.22f
#define SPRITE_MORTE 4
#define BOSS_COL 32
#define BOSS_LIN 38
#define TILE_X(c) ((c) * MAPA_TILE + MAPA_TILE / 2)
#define TILE_Y(l) ((l) * MAPA_TILE + MAPA_TILE - 8)
#define NUM_NASCER ((int)(sizeof(NASCER) / sizeof(NASCER[0])))

typedef struct {
    float vel, visao, alcance, parar;
    float golpeTempo, golpeAcerto, recarga;
    float raioTiro, alturaPeito;
    float escala, pe, peAtaque, topo;
    int dano, vida, sombraX, sombraY, barra;
} Tipo;

static const Tipo TIPOS[2] = {
    {.vel = 110, .visao = 420, .alcance = 45, .parar = 30,
     .golpeTempo = 0.8f, .golpeAcerto = 0.4f, .recarga = 1.1f,
     .raioTiro = 26, .alturaPeito = 36,
     .escala = 0.27f, .pe = 430, .peAtaque = 412, .topo = 85,
     .dano = 10, .vida = 60, .sombraX = 19, .sombraY = 6, .barra = 44},
    {.vel = 85, .visao = 560, .alcance = 85, .parar = 50,
     .golpeTempo = 0.9f, .golpeAcerto = 0.5f, .recarga = 1.8f,
     .raioTiro = 42, .alturaPeito = 70,
     .escala = 0.25f, .pe = 672, .peAtaque = 672, .topo = 24,
     .dano = 25, .vida = 600, .sombraX = 44, .sombraY = 13, .barra = 120}
};

const Arma ARMAS[NUM_ARMAS] = {
    {"Sem arma", 0, 0, 0.00f, 0.50f},
    {"Pistola", 25, 1, 0.00f, 0.35f},
    {"MP5", 10, 1, 0.06f, 0.09f},
    {"Shotgun", 14, 6, 0.28f, 0.90f}
};

<<<<<<< HEAD
static const int NASCER[][2] = {
    {15, 5}, {16, 11},
    {35, 4}, {28, 5}, {27, 10}, {34, 12},
    {59, 5}, {48, 6}, {54, 7}, {43, 9}, {60, 10}, {50, 12},
    {7, 18}, {13, 22}, {3, 23}, {8, 26},
    {35, 22}, {30, 24}, {25, 26}, {36, 29}, {31, 30},
    {52, 21}, {60, 22}, {45, 26}, {60, 27}, {51, 28},
    {6, 35}, {13, 39}, {6, 40},
=======
/* Pontos de nascimento dos zumbis: {coluna, linha} em tiles do mapa (colisao.txt), agrupados por sala.
   Para ter mais (ou menos) inimigos basta adicionar/remover linhas aqui. Todos estao em chao livre. */
static const int NASCER[][2] = {
    /* sala superior esquerda */
    {15, 5}, {16, 11},
    /* sala superior central */
    {35, 4}, {28, 5}, {27, 10}, {34, 12},
    /* sala superior direita */
    {59, 5}, {48, 6}, {54, 7}, {43, 9}, {60, 10}, {50, 12},
    /* sala do meio esquerda */
    {7, 18}, {13, 22}, {3, 23}, {8, 26},
    /* sala central (reator) */
    {35, 22}, {30, 24}, {25, 26}, {36, 29}, {31, 30},
    /* sala direita (enfermaria) */
    {52, 21}, {60, 22}, {45, 26}, {60, 27}, {51, 28},
    /* sala inferior esquerda */
    {6, 35}, {13, 39}, {6, 40},
    /* sala inferior central (servidores) */
>>>>>>> f1c1fc7668397b901117770d75846915401169fa
    {31, 37}, {24, 39}, {38, 39}
};
#define NUM_NASCER ((int)(sizeof(NASCER) / sizeof(NASCER[0])))
#define BOSS_COL 32
#define BOSS_LIN 38

/* centro do tile em x; y fica perto da base do tile (os pes do inimigo) */
#define TILE_X(c) ((c) * MAPA_TILE + MAPA_TILE / 2)
#define TILE_Y(l) ((l) * MAPA_TILE + MAPA_TILE - 8)

static const char *PASTAS[5][2] = {
    {"zumbi_frames", "zumbi_frame"},
    {"zumbi_ataque_frames", "ataque_zumbi_frame"},
    {"boss_frames", "boss_frame"},
    {"boss_ataque_frames", "boss_ataque_frame"},
    {"boss_morte_frames", "boss_morte_frame"}
};

static Texture2D sprites[5][FRAMES];
static Inimigo *inimigos = NULL;
static Tiro *tiros = NULL;
int vivos = 0;
float inimigo_mult_vel = 1.0f;
float inimigo_mult_dano = 1.0f;
int (*bloqueado_fn)(float x, float y) = NULL;

static int caixa(float x, float y)
{
    return bloqueado_fn(x - 14, y - 10) || bloqueado_fn(x + 14, y - 10) ||
           bloqueado_fn(x - 14, y + 2) || bloqueado_fn(x + 14, y + 2);
}

void inimigos_carregar(void)
{
    for (int s = 0; s < 5; s++) {
        for (int k = 0; k < FRAMES; k++) {
            const char *path = TextFormat("assets/%s/%s_%02d.png", PASTAS[s][0], PASTAS[s][1], k + 1);

            sprites[s][k] = LoadTexture(path);
            if (sprites[s][k].id == 0) {
                TraceLog(LOG_ERROR, "Frame nao encontrado: %s", path);
            }
        }
    }
}

void inimigos_descarregar(void)
{
    for (int s = 0; s < 5; s++) {
        for (int k = 0; k < FRAMES; k++) {
            UnloadTexture(sprites[s][k]);
        }
    }
}

static void novo(int col, int lin, int boss)
{
    Inimigo *i = malloc(sizeof(Inimigo));

    if (i == NULL) {
        return;
    }
    *i = (Inimigo){
        .x = TILE_X(col), .y = TILE_Y(lin), .anim = vivos, .ataque = 0.5f, .morte = -1,
        .vida = TIPOS[boss].vida, .vidaMax = TIPOS[boss].vida, .boss = boss, .prox = inimigos
    };
    inimigos = i;
    vivos++;
}

void inimigos_criar(void)
{
<<<<<<< HEAD
    for (int k = 0; k < NUM_NASCER; k++) {
        novo(NASCER[k][0], NASCER[k][1], 0);
    }
    novo(BOSS_COL, BOSS_LIN, 1);
=======
    int k;

    for (k = 0; k < NUM_NASCER; k++) {
        novo(TILE_X(NASCER[k][0]), TILE_Y(NASCER[k][1]), VIDA_INIMIGO, 0);
    }
    novo(TILE_X(BOSS_COL), TILE_Y(BOSS_LIN), BOSS_VIDA, 1);
>>>>>>> f1c1fc7668397b901117770d75846915401169fa
}

void inimigos_liberar(void)
{
    while (inimigos != NULL) {
        Inimigo *proximo = inimigos->prox;

        free(inimigos);
        inimigos = proximo;
    }
    while (tiros != NULL) {
        Tiro *proximo = tiros->prox;

        free(tiros);
        tiros = proximo;
    }
    vivos = 0;
}

void atirar(int arma, float x, float y, float alvoX, float alvoY)
{
    const Arma *a = &ARMAS[arma];
    float ang = atan2f(alvoY - y, alvoX - x);

    for (int k = 0; k < a->balas; k++) {
        float desvio = ang + GetRandomValue(-100, 100) / 100.0f * a->espalhar;
        Tiro *t = malloc(sizeof(Tiro));

        if (t == NULL) {
            return;
        }
        *t = (Tiro){
            .x = x + cosf(ang) * 18, .y = y + sinf(ang) * 18,
            .vx = cosf(desvio) * VEL_TIRO, .vy = sinf(desvio) * VEL_TIRO,
            .tempo = 0.8f, .dano = a->dano, .prox = tiros
        };
        tiros = t;
    }
}

static int acertou_inimigo(const Tiro *t)
{
    for (Inimigo *i = inimigos; i != NULL; i = i->prox) {
        const Tipo *tipo = &TIPOS[i->boss];
        float dx = t->x - i->x;
        float dy = t->y - (i->y - tipo->alturaPeito);

        if (i->vida > 0 && dx * dx + dy * dy < tipo->raioTiro * tipo->raioTiro) {
            i->vida -= t->dano;
            return 1;
        }
    }
    return 0;
}

static int tiro_andar(Tiro *t, float dt)
{
    int passos = (int)(VEL_TIRO * dt / 10) + 1;

    t->tempo -= dt;
    for (int s = 0; s < passos && t->tempo > 0; s++) {
        t->x += t->vx * dt / passos;
        t->y += t->vy * dt / passos;
        if (bloqueado_fn(t->x, t->y + PEITO) || acertou_inimigo(t)) {
            return 0;
        }
    }
    return t->tempo > 0;
}

static void mover_tiros(float dt)
{
    Tiro **p = &tiros;

    while (*p != NULL) {
        Tiro *t = *p;

        if (tiro_andar(t, dt)) {
            p = &t->prox;
        } else {
            *p = t->prox;
            free(t);
        }
    }
}

static void remover_mortos(void)
{
    Inimigo **p = &inimigos;

    while (*p != NULL) {
        Inimigo *i = *p;

        if (i->vida <= 0 && !i->boss) {
            *p = i->prox;
            free(i);
            vivos--;
        } else {
            p = &i->prox;
        }
    }
}

static void andar(Inimigo *i, float vx, float vy)
{
    if (!caixa(i->x + vx, i->y)) {
        i->x += vx;
    }
    if (!caixa(i->x, i->y + vy)) {
        i->y += vy;
    }
}

static int atualizar_um(Inimigo *i, float px, float py, float dt)
{
    const Tipo *t = &TIPOS[i->boss];
    float dx = px - i->x;
    float dy = py - i->y;
    float d = sqrtf(dx * dx + dy * dy);

    if (i->vida <= 0) {
        if (i->boss) {
            if (i->morte < 0) {
                i->morte = 0;
                vivos--;
            }
            i->morte += dt;
        }
        return 0;
    }
    i->ataque -= dt;
    if (i->golpe > 0) {
        i->golpe -= dt;
        if (!i->acertou && i->golpe <= t->golpeAcerto) {
            i->acertou = 1;
            return d < t->alcance + 15 ? (int)(t->dano * inimigo_mult_dano + 0.5f) : 0;
        }
        return 0;
    }
    if (d < t->visao) {
        i->direita = dx >= 0;
        if (d > t->parar) {
            float passo = t->vel * inimigo_mult_vel * dt / d;

            andar(i, dx * passo, dy * passo);
            i->anim += dt;
        }
    }
    if (d < t->alcance && i->ataque <= 0) {
        i->golpe = t->golpeTempo;
        i->acertou = 0;
        i->ataque = t->recarga;
    }
    return 0;
}

int inimigos_atualizar(float px, float py, float dt)
{
    int dano = 0;

    for (Inimigo *i = inimigos; i != NULL; i = i->prox) {
        dano += atualizar_um(i, px, py, dt);
    }
    mover_tiros(dt);
    remover_mortos();
    return dano;
}

static Texture2D sprite_atual(const Inimigo *i)
{
    const Tipo *t = &TIPOS[i->boss];
    int quadro = ((int)(i->anim * 10) % FRAMES + FRAMES) % FRAMES;
    int ataque = (int)((t->golpeTempo - i->golpe) / t->golpeTempo * FRAMES) % FRAMES;

    if (i->morte >= 0) {
        return sprites[SPRITE_MORTE][(int)fminf(FRAMES - 1, i->morte / MORTE_TEMPO)];
    }
    if (i->golpe > 0) {
        return sprites[i->boss * 2 + 1][ataque];
    }
    return sprites[i->boss * 2][quadro];
}

void inimigos_desenhar(float yMin, float yMax)
{
    for (Inimigo *i = inimigos; i != NULL; i = i->prox) {
        const Tipo *t = &TIPOS[i->boss];
        Texture2D tex = sprite_atual(i);
        float pe = i->golpe > 0 ? t->peAtaque : t->pe;
        float larg = tex.width * t->escala;
        float alt = tex.height * t->escala;

        if (i->y <= yMin || i->y > yMax) {
            continue;
        }
        DrawEllipse((int)i->x, (int)i->y, t->sombraX, t->sombraY, Fade(BLACK, 0.4f));
        DrawTexturePro(tex,
                       (Rectangle){0, 0, i->direita ? (float)tex.width : -(float)tex.width, (float)tex.height},
                       (Rectangle){i->x - larg / 2, i->y - pe * t->escala, larg, alt},
                       (Vector2){0, 0}, 0, WHITE);
    }
}

void inimigos_desenhar_vidas(Camera2D camera)
{
    for (Inimigo *i = inimigos; i != NULL; i = i->prox) {
        const Tipo *t = &TIPOS[i->boss];
        Vector2 p = GetWorldToScreen2D((Vector2){i->x, i->y - (t->pe - t->topo) * t->escala - 12}, camera);
        int x = (int)p.x - t->barra / 2;
        int y = (int)p.y;
        float f = (float)i->vida / i->vidaMax;
        Color cor = f > 0.5f ? GREEN : (f > 0.25f ? ORANGE : RED);
        const char *txt = TextFormat("%d", i->vida);

        if (i->vida <= 0) {
            continue;
        }
        DrawRectangle(x - 1, y - 1, t->barra + 2, 8, BLACK);
        DrawRectangle(x, y, t->barra, 6, (Color){60, 20, 20, 255});
        DrawRectangle(x, y, (int)(t->barra * f), 6, cor);
        DrawText(txt, (int)p.x - MeasureText(txt, 10) / 2, y - 12, 10, RAYWHITE);
    }
}

void tiros_desenhar(void)
{
    for (Tiro *t = tiros; t != NULL; t = t->prox) {
        DrawLineEx((Vector2){t->x - t->vx * 0.02f, t->y - t->vy * 0.02f}, (Vector2){t->x, t->y}, 4, YELLOW);
    }
}
