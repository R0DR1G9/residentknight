#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "fase.h"

#define COR(r, g, b) ((Color){ (r), (g), (b), 255 })
#define CORA(r, g, b, a) ((Color){ (r), (g), (b), (a) })

static int hash2(int a, int b)
{
    int h = (a * 7919 + b * 104729 + 12345) % 65521;

    h = (h * 31337 + 7) % 65521;
    h = (h * 17 + a * 131 + b * 71) % 65521;
    return h;
}

static void R(Objeto *o, float x, float y, float w, float h, Color c)
{
    DrawRectangle(o->col * TAM_TILE + (int)(x * PX),
                  o->lin * TAM_TILE + (int)(y * PX),
                  (int)(w * PX), (int)(h * PX), c);
}

static void sombra(Objeto *o, float x, float y, float w, float h)
{
    R(o, x, y, w, h, CORA(0, 0, 0, 90));
}

static float cintilar(Objeto *o, float t)
{
    if (o->variante == 1) {
        float f = sinf(t * 31.0f + o->col * 1.7f) * sinf(t * 7.0f + o->lin);

        if (f > 0.55f) {
            return 0.35f;
        }
        return 1.0f;
    }
    return 0.92f + 0.08f * sinf(t * 5.0f + o->col);
}

void obj_caixote(Objeto *o, float tempo)
{
    Color topo = COR(116, 88, 54);
    Color frente = COR(88, 64, 38);
    Color ripa = COR(44, 30, 20);

    (void)tempo;
    if (o->variante) {
        topo = COR(104, 80, 50);
        frente = COR(78, 56, 34);
    }
    sombra(o, 1, 13, 15, 3);
    R(o, 1, 1, 14, 5, topo);
    R(o, 1, 3, 14, 1, ripa);
    R(o, 1, 6, 14, 8, frente);
    R(o, 1, 6, 14, 1, ripa);
    R(o, 1, 10, 14, 1, ripa);
    R(o, 1, 13, 14, 1, ripa);
    R(o, 1, 1, 1, 13, ripa);
    R(o, 14, 1, 1, 13, ripa);
    R(o, 7, 6, 2, 8, ripa);
    R(o, 2, 2, 12, 1, COR(140, 110, 70));
}

void obj_barril(Objeto *o, float tempo)
{
    Color corpo;

    (void)tempo;
    if (o->variante == 0) {
        corpo = COR(66, 88, 72);
    } else if (o->variante == 1) {
        corpo = COR(120, 64, 40);
    } else {
        corpo = COR(62, 74, 96);
    }
    sombra(o, 3, 13, 11, 3);
    R(o, 3, 3, 10, 11, corpo);
    R(o, 4, 1, 8, 3, CORA(255, 255, 255, 60));
    R(o, 3, 2, 10, 2, COR(corpo.r + 30, corpo.g + 30, corpo.b + 30));
    R(o, 3, 6, 10, 1, CORA(0, 0, 0, 110));
    R(o, 3, 10, 10, 1, CORA(0, 0, 0, 110));
    R(o, 4, 4, 2, 9, CORA(255, 255, 255, 40));
    R(o, 11, 4, 2, 9, CORA(0, 0, 0, 70));
    if (o->variante == 1) {
        R(o, 5, 7, 6, 3, COR(190, 160, 40));
        R(o, 7, 8, 2, 1, COR(20, 20, 20));
    }
}

void obj_barril_fogo(Objeto *o, float tempo)
{
    float h1 = 4.0f + 1.5f * sinf(tempo * 13.0f + o->col);
    float h2 = 5.0f + 1.5f * sinf(tempo * 17.0f + 1.0f + o->lin);
    float h3 = 3.5f + 1.2f * sinf(tempo * 21.0f + 2.0f);

    sombra(o, 3, 13, 11, 3);
    R(o, 3, 4, 10, 10, COR(70, 56, 48));
    R(o, 3, 3, 10, 2, COR(96, 78, 64));
    R(o, 3, 8, 10, 1, CORA(0, 0, 0, 110));
    R(o, 3, 11, 10, 1, CORA(0, 0, 0, 110));
    R(o, 4, 5, 2, 8, CORA(255, 255, 255, 30));
    R(o, 11, 5, 2, 8, CORA(0, 0, 0, 80));
    R(o, 5, 3.5f, 6, 1, COR(30, 10, 6));

    R(o, 4, 3.5f - h1, 3, h1, COR(220, 80, 20));
    R(o, 6.5f, 3.5f - h2, 3, h2, COR(240, 120, 30));
    R(o, 9.5f, 3.5f - h3, 3, h3, COR(220, 80, 20));
    R(o, 6, 3.5f - h1 * 0.6f, 4, h1 * 0.6f, COR(255, 210, 80));
}

void obj_sacos(Objeto *o, float tempo)
{
    Color a = COR(126, 112, 78);
    Color b = COR(110, 98, 68);
    Color luz = COR(150, 136, 98);
    Color costura = CORA(0, 0, 0, 90);

    (void)tempo;
    sombra(o, 1, 13, 30, 3);
    R(o, 6, 3, 10, 5, b);
    R(o, 6, 3, 10, 1, luz);
    R(o, 16, 3, 10, 5, a);
    R(o, 16, 3, 10, 1, luz);
    R(o, 1, 8, 10, 6, a);
    R(o, 1, 8, 10, 1, luz);
    R(o, 11, 8, 10, 6, b);
    R(o, 11, 8, 10, 1, luz);
    R(o, 21, 8, 10, 6, a);
    R(o, 21, 8, 10, 1, luz);
    R(o, 5, 10, 2, 1, costura);
    R(o, 15, 10, 2, 1, costura);
    R(o, 25, 10, 2, 1, costura);
    R(o, 1, 13, 30, 1, costura);
}

void obj_bloco(Objeto *o, float tempo)
{
    (void)tempo;
    sombra(o, 1, 13, 15, 3);
    R(o, 1, 4, 14, 10, COR(98, 100, 100));
    R(o, 1, 2, 14, 3, COR(124, 126, 124));
    R(o, 5, 5, 1, 6, COR(50, 52, 52));
    R(o, 6, 9, 3, 1, COR(50, 52, 52));
    R(o, 11, 6, 1, 4, COR(50, 52, 52));
    R(o, 11, 0, 1, 3, COR(120, 70, 40));
    R(o, 13, 1, 1, 2, COR(120, 70, 40));
    R(o, 1, 13, 14, 1, CORA(0, 0, 0, 80));
}

void obj_placa(Objeto *o, float tempo)
{
    Color branco = COR(210, 208, 198);

    (void)tempo;
    sombra(o, 4, 14, 8, 2);
    R(o, 7, 8, 2, 7, COR(60, 60, 64));
    R(o, 2, 1, 12, 7, COR(140, 34, 34));
    R(o, 2, 1, 12, 1, branco);
    R(o, 2, 7, 12, 1, branco);
    R(o, 2, 1, 1, 7, branco);
    R(o, 13, 1, 1, 7, branco);
    R(o, 7, 2, 2, 3, branco);
    R(o, 7, 5.5f, 2, 1, branco);
    R(o, 4, 8, 1, 2, CORA(110, 40, 20, 160));
}

void obj_sangue(Objeto *o, float tempo)
{
    Color a = COR(104, 10, 14);
    Color b = COR(74, 6, 10);
    Color c = COR(134, 16, 20);
    float w = o->larg * 16.0f;
    int i;

    (void)tempo;
    if (o->variante == 0) {
        R(o, 3, 5, 10, 7, b);
        R(o, 5, 3, 7, 10, b);
        R(o, 4, 6, 8, 5, a);
        R(o, 6, 7, 3, 2, c);
    } else if (o->variante == 1) {
        R(o, 2, 6, w - 4, 5, b);
        R(o, 4, 7, w - 8, 3, a);
        R(o, w - 8, 4, 6, 8, b);
        for (i = 0; i < 4; i++) {
            R(o, w * 0.15f + i * (w * 0.22f), 3 + (i % 2) * 9, 2, 2, a);
        }
    } else {
        R(o, 3, 3, 3, 3, a);
        R(o, 9, 5, 2, 2, b);
        R(o, 6, 10, 4, 3, a);
        R(o, 11, 11, 2, 2, b);
        R(o, 2, 9, 2, 2, b);
    }
}

void obj_carro(Objeto *o, float tempo)
{
    Color corpo = COR(70, 82, 92);
    Color claro = COR(92, 106, 116);
    Color escuro = COR(44, 52, 60);
    Color vidro = COR(26, 38, 44);

    (void)tempo;
    sombra(o, 1, 28, 46, 4);
    R(o, 2, 14, 44, 12, corpo);
    R(o, 2, 14, 44, 2, claro);
    R(o, 2, 24, 44, 2, escuro);
    R(o, 12, 5, 22, 10, claro);
    R(o, 14, 6, 8, 7, vidro);
    R(o, 24, 6, 8, 7, vidro);
    R(o, 16, 7, 1, 5, COR(120, 140, 150));
    R(o, 17, 9, 3, 1, COR(120, 140, 150));
    R(o, 27, 7, 1, 4, COR(120, 140, 150));
    R(o, 34, 10, 12, 5, escuro);
    R(o, 2, 11, 10, 4, escuro);
    R(o, 20, 18, 6, 3, COR(120, 64, 36));
    R(o, 8, 16, 4, 2, COR(120, 64, 36));
    R(o, 2, 21, 44, 1, escuro);
    R(o, 7, 23, 9, 6, COR(14, 14, 16));
    R(o, 32, 23, 9, 6, COR(14, 14, 16));
    R(o, 10, 25, 3, 2, COR(110, 110, 114));
    R(o, 35, 25, 3, 2, COR(110, 110, 114));
    R(o, 45, 17, 2, 3, COR(200, 190, 130));
    R(o, 1, 17, 2, 3, COR(150, 20, 20));
}

void obj_maquina(Objeto *o, float tempo)
{
    int i;

    sombra(o, 2, 14, 13, 2);
    R(o, 2, 2, 12, 13, COR(52, 58, 62));
    R(o, 2, 2, 12, 1, COR(78, 84, 88));
    R(o, 4, 4, 8, 5, COR(10, 28, 18));
    for (i = 0; i < 3; i++) {
        float g = 0.6f + 0.4f * sinf(tempo * 3.0f + i);
        R(o, 5, 5 + i * 1.5f, 3 + (i % 2) * 2, 1,
          COR(40, (int)(200 * g), (int)(70 * g)));
    }
    R(o, 4, 10, 2, 2, COR(160, 40, 40));
    R(o, 7, 10, 2, 2, COR(180, 160, 50));
    R(o, 10, 10, 2, 2, COR(40, 120, 60));
    R(o, 4, 13, 8, 1, COR(20, 20, 22));
}

void obj_bau(Objeto *o, float tempo)
{
    (void)tempo;
    sombra(o, 2, 14, 13, 2);
    R(o, 2, 6, 12, 8, COR(78, 58, 38));
    R(o, 2, 4, 12, 3, COR(96, 72, 46));
    R(o, 2, 4, 12, 1, COR(120, 94, 60));
    R(o, 4, 4, 1, 10, COR(60, 60, 64));
    R(o, 11, 4, 1, 10, COR(60, 60, 64));
    R(o, 2, 7, 12, 1, CORA(0, 0, 0, 90));
    R(o, 7, 7, 2, 3, COR(200, 170, 60));
    R(o, 2, 13, 12, 1, CORA(0, 0, 0, 90));
}

void obj_lampada_parede(Objeto *o, float tempo)
{
    float f = cintilar(o, tempo);
    Color bulbo = COR((int)(120 + 135 * f), (int)(100 + 136 * f), (int)(60 + 90 * f));
    Color metal = COR(44, 44, 48);

    R(o, 5, 3, 6, 2, metal);
    R(o, 4, 4, 8, 7, metal);
    R(o, 5, 5, 6, 5, bulbo);
    R(o, 6, 6, 2, 1, COR(255, 250, 220));
    R(o, 4, 11, 8, 1, CORA(0, 0, 0, 120));
}

void obj_poste(Objeto *o, float tempo)
{
    float f = cintilar(o, tempo);
    Color bulbo = COR((int)(120 + 130 * f), (int)(120 + 130 * f), (int)(80 + 100 * f));
    Color metal = COR(58, 58, 62);

    sombra(o, 4, 14, 8, 2);
    R(o, 7, -12, 2, 27, metal);
    R(o, 5, 13, 6, 2, COR(46, 46, 50));
    R(o, 3, -15, 10, 4, COR(72, 72, 76));
    R(o, 5, -11, 6, 2, bulbo);
}

void obj_faixa(Objeto *o, float tempo)
{
    int i;
    int n;
    int vertical = o->alt > o->larg;

    (void)tempo;
    if (vertical) {
        n = o->alt * 16 / 3;
    } else {
        n = o->larg * 16 / 3;
    }
    for (i = 0; i < n; i++) {
        Color c = COR(24, 24, 24);

        if (i % 2 == 1) {
            c = COR(150, 120, 30);
        }
        if (vertical) {
            R(o, 6, i * 3, 4, 3, c);
        } else {
            R(o, i * 3, 6, 3, 4, c);
        }
    }
}

void obj_pilar(Objeto *o, float tempo)
{
    (void)tempo;
    sombra(o, 2, 13, 12, 3);
    R(o, 3, -14, 10, 29, COR(88, 90, 92));
    R(o, 3, -14, 3, 29, COR(108, 110, 112));
    R(o, 11, -14, 2, 29, COR(62, 64, 66));
    R(o, 2, -16, 12, 3, COR(110, 112, 114));
    R(o, 2, 12, 12, 3, COR(76, 78, 80));
    R(o, 3, 6, 10, 2, COR(150, 36, 36));
    R(o, 3, -4, 10, 2, COR(150, 36, 36));
    R(o, 7, -9, 1, 5, COR(40, 42, 44));
}

void obj_portal(Objeto *o, float tempo)
{
    float cx = (o->col + o->larg / 2.0f) * TAM_TILE;
    float cy = (o->lin + o->alt / 2.0f) * TAM_TILE;
    float pulso = 0.5f + 0.5f * sinf(tempo * 3.0f);
    Vector2 c = { cx, cy };
    int i;

    DrawCircleV(c, 94.0f, COR(18, 18, 22));
    DrawRing(c, 82.0f, 94.0f, 0.0f, 360.0f, 48, COR(72, 72, 78));
    for (i = 0; i < 12; i++) {
        float a = i * 30.0f + tempo * 25.0f;
        Color faixa = COR(210, 205, 195);

        if (i % 2 == 1) {
            faixa = COR(170, 30, 30);
        }
        DrawRing(c, 64.0f, 80.0f, a, a + 22.0f, 8, faixa);
    }
    DrawCircleV(c, 62.0f, COR(20, 6, 8));
    DrawPoly(c, 6, 50.0f, -tempo * 40.0f, COR(120, 14, 18));
    DrawPoly(c, 6, 40.0f, tempo * 30.0f, COR(30, 6, 8));
    DrawCircleV(c, 30.0f + 8.0f * pulso, CORA(255, 50, 50, 170));
    DrawCircleV(c, 16.0f, CORA(255, 235, 220, 230));

    DrawRectangleGradientV((int)cx - 40, (int)cy - 200, 80, 200,
                           CORA(255, 40, 40, 0),
                           CORA(255, 60, 60, (int)(70 + 70 * pulso)));
    for (i = 0; i < 14; i++) {
        float seed = i * 37.7f;
        float sobe = fmodf(tempo * 45.0f + i * 23.0f, 190.0f);
        float alfa = 1.0f - sobe / 190.0f;
        DrawRectangle((int)(cx + sinf(seed) * 50.0f), (int)(cy - sobe), 4, 4,
                      CORA(255, 90, 70, (int)(220 * alfa)));
    }
}

static int tile_chao(Fase *f, int c, int l)
{
    if (c < 0 || c >= COLUNAS || l < 0 || l >= LINHAS) {
        return 0;
    }
    return f->mapa[l][c] == '.' || f->mapa[l][c] == 'o';
}

static int estilo_chao(Fase *f, int c, int l)
{
    Sala *s = f->salas;

    while (s != NULL) {
        if (c >= s->col && c < s->col + s->larg && l >= s->lin && l < s->lin + s->alt) {
            return s->id;
        }
        s = s->proxima;
    }
    return 3;
}

static void desenhar_chao(int c, int l, int estilo)
{
    int x = c * TAM_TILE;
    int y = l * TAM_TILE;
    int h = hash2(c, l);
    int par = (c + l) % 2;
    Color base;
    Color linha;
    int k;

    if (estilo == 0) {
        base = COR(76, 76, 70);
        if (par) {
            base = COR(86, 86, 80);
        }
        linha = COR(40, 40, 38);
        DrawRectangle(x, y, TAM_TILE, TAM_TILE, base);
        DrawRectangle(x, y, TAM_TILE, 2, linha);
        DrawRectangle(x, y, 2, TAM_TILE, linha);
        if (h % 7 == 0) {
            DrawRectangle(x + (int)((h / 8) % 36) + 4, y + (int)((h / 512) % 36) + 4,
                          12 + (int)(h % 10), 8, CORA(30, 30, 26, 110));
        }
    } else if (estilo == 1) {
        base = COR(62, 62, 64);
        if (par) {
            base = COR(70, 70, 70);
        }
        DrawRectangle(x, y, TAM_TILE, TAM_TILE, base);
        DrawRectangle(x, y, TAM_TILE, 1, CORA(0, 0, 0, 60));
        DrawRectangle(x, y, 1, TAM_TILE, CORA(0, 0, 0, 60));
        if (h % 5 == 0) {
            DrawRectangle(x + 8, y + (int)((h / 16) % 48), 24 + (int)((h / 256) % 24), 2, COR(30, 30, 30));
            DrawRectangle(x + 30, y + (int)((h / 16) % 48), 2, 14, COR(30, 30, 30));
        }
        for (k = 0; k < 3; k++) {
            int hh = hash2(c * 7 + k, l * 3);
            DrawRectangle(x + (int)(hh % 56), y + (int)((hh / 256) % 56), 4, 4, CORA(30, 30, 30, 120));
        }
    } else if (estilo == 2) {
        base = COR(56, 70, 72);
        if (par) {
            base = COR(64, 80, 80);
        }
        linha = COR(30, 40, 40);
        DrawRectangle(x, y, TAM_TILE, TAM_TILE, base);
        DrawRectangle(x, y, TAM_TILE, 2, linha);
        DrawRectangle(x, y + TAM_TILE - 2, TAM_TILE, 2, linha);
        DrawRectangle(x, y, 2, TAM_TILE, linha);
        DrawRectangle(x + TAM_TILE - 2, y, 2, TAM_TILE, linha);
        DrawRectangle(x + 6, y + 6, 4, 4, COR(100, 116, 116));
        DrawRectangle(x + 54, y + 6, 4, 4, COR(100, 116, 116));
        DrawRectangle(x + 6, y + 54, 4, 4, COR(100, 116, 116));
        DrawRectangle(x + 54, y + 54, 4, 4, COR(100, 116, 116));
        if (h % 3 == 0) {
            for (k = 0; k < 3; k++) {
                DrawRectangle(x + 14, y + 14 + k * 12, 36, 4, COR(24, 34, 34));
            }
        }
    } else {
        base = COR(52, 52, 54);
        if (par) {
            base = COR(58, 58, 60);
        }
        DrawRectangle(x, y, TAM_TILE, TAM_TILE, base);
        DrawRectangle(x, y, TAM_TILE, 1, CORA(0, 0, 0, 70));
        if (h % 6 == 0) {
            DrawRectangle(x + 10, y + 14, 30, 18, CORA(20, 20, 20, 110));
        }
        for (k = 0; k < 3; k++) {
            int hh = hash2(c * 5 + k, l * 11);
            DrawRectangle(x + (int)(hh % 58), y + (int)((hh / 256) % 58), 3, 3, CORA(26, 26, 28, 140));
        }
    }
}

static void desenhar_parede_frente(int c, int l, int estilo)
{
    int x = c * TAM_TILE;
    int y = l * TAM_TILE;
    int h = hash2(c, l);
    int r;

    if (estilo == 0) {
        DrawRectangle(x, y, TAM_TILE, TAM_TILE, COR(84, 58, 50));
        DrawRectangle(x, y, TAM_TILE, 12, COR(110, 104, 98));
        DrawRectangle(x, y + 12, TAM_TILE, 4, COR(40, 28, 26));
        for (r = 0; r < 3; r++) {
            int yy = y + 16 + r * 16;
            int off = (r % 2) * 16;
            DrawRectangle(x, yy + 14, TAM_TILE, 2, COR(44, 32, 30));
            DrawRectangle(x + 8 + off, yy, 2, 14, COR(44, 32, 30));
            DrawRectangle(x + 40 + off, yy, 2, 14, COR(44, 32, 30));
        }
    } else if (estilo == 1) {
        DrawRectangle(x, y, TAM_TILE, TAM_TILE, COR(84, 84, 86));
        DrawRectangle(x, y, TAM_TILE, 12, COR(120, 120, 122));
        DrawRectangle(x, y + 12, TAM_TILE, 4, COR(40, 40, 44));
        DrawRectangle(x + 31, y + 16, 2, 48, COR(52, 52, 56));
        if (h % 3 == 0) {
            DrawRectangle(x + 10, y + 24, 2, 20, COR(40, 40, 44));
            DrawRectangle(x + 12, y + 40, 10, 2, COR(40, 40, 44));
        }
        if (h % 5 == 0) {
            DrawRectangle(x + 44, y + 16, 3, 16, COR(120, 70, 40));
        }
    } else if (estilo == 2) {
        DrawRectangle(x, y, TAM_TILE, TAM_TILE, COR(58, 74, 76));
        DrawRectangle(x, y, TAM_TILE, 12, COR(96, 114, 114));
        DrawRectangle(x, y + 12, TAM_TILE, 4, COR(28, 38, 40));
        DrawRectangle(x + 4, y + 20, 56, 32, COR(68, 86, 88));
        DrawRectangle(x + 8, y + 24, 4, 4, COR(110, 128, 128));
        DrawRectangle(x + 52, y + 24, 4, 4, COR(110, 128, 128));
        DrawRectangle(x + 8, y + 44, 4, 4, COR(110, 128, 128));
        DrawRectangle(x + 52, y + 44, 4, 4, COR(110, 128, 128));
        DrawRectangle(x, y + 56, TAM_TILE, 4, COR(140, 30, 30));
    } else {
        DrawRectangle(x, y, TAM_TILE, TAM_TILE, COR(66, 66, 70));
        DrawRectangle(x, y, TAM_TILE, 12, COR(100, 100, 104));
        DrawRectangle(x, y + 12, TAM_TILE, 4, COR(32, 32, 36));
        DrawRectangle(x + 31, y + 16, 2, 48, COR(46, 46, 50));
        DrawRectangle(x, y + 36, TAM_TILE, 3, COR(50, 50, 54));
    }
    if (h % 4 == 0) {
        DrawRectangle(x + (int)(h % 48) + 4, y + 16, 6, 30, CORA(0, 0, 0, 50));
    }
    DrawRectangle(x, y + 56, TAM_TILE, 8, CORA(0, 0, 0, 70));
}

static void desenhar_parede_topo(Fase *f, int c, int l)
{
    int x = c * TAM_TILE;
    int y = l * TAM_TILE;
    Color borda = COR(74, 74, 80);

    DrawRectangle(x, y, TAM_TILE, TAM_TILE, COR(22, 22, 26));
    if (tile_chao(f, c + 1, l)) {
        DrawRectangle(x + 58, y, 6, TAM_TILE, borda);
    }
    if (tile_chao(f, c - 1, l)) {
        DrawRectangle(x, y, 6, TAM_TILE, borda);
    }
    if (tile_chao(f, c, l - 1)) {
        DrawRectangle(x, y, TAM_TILE, 8, borda);
    }
}

static void desenhar_tiles(Fase *f, Camera2D cam)
{
    int c0 = (int)((cam.target.x - LARGURA_TELA / 2.0f) / TAM_TILE) - 1;
    int c1 = (int)((cam.target.x + LARGURA_TELA / 2.0f) / TAM_TILE) + 1;
    int l0 = (int)((cam.target.y - ALTURA_TELA / 2.0f) / TAM_TILE) - 1;
    int l1 = (int)((cam.target.y + ALTURA_TELA / 2.0f) / TAM_TILE) + 1;
    int c;
    int l;

    if (c0 < 0) {
        c0 = 0;
    }
    if (l0 < 0) {
        l0 = 0;
    }
    if (c1 >= COLUNAS) {
        c1 = COLUNAS - 1;
    }
    if (l1 >= LINHAS) {
        l1 = LINHAS - 1;
    }

    for (l = l0; l <= l1; l++) {
        for (c = c0; c <= c1; c++) {
            char t = f->mapa[l][c];

            if (t == '.' || t == 'o') {
                desenhar_chao(c, l, estilo_chao(f, c, l));
            } else if (t == '#') {
                if (tile_chao(f, c, l + 1)) {
                    desenhar_parede_frente(c, l, estilo_chao(f, c, l + 1));
                } else {
                    desenhar_parede_topo(f, c, l);
                }
            }
        }
    }
}

static void luz(Camera2D cam, float wx, float wy, float raio, Color cor, float intens)
{
    Vector2 s = GetWorldToScreen2D((Vector2){ wx, wy }, cam);
    int n = 14;
    int i;

    if (s.x < -raio || s.x > LARGURA_TELA + raio || s.y < -raio || s.y > ALTURA_TELA + raio) {
        return;
    }
    for (i = 0; i < n; i++) {
        float r = raio * (1.0f - (float)i / n);
        float a = 255.0f * intens / n;

        if (a > 255.0f) {
            a = 255.0f;
        }
        DrawCircleV(s, r, CORA(cor.r, cor.g, cor.b, (int)a));
    }
}

static void luz_do_objeto(Fase *f, Camera2D cam, Objeto *o)
{
    float cx;
    float cy;

    if (o->luz <= 0) {
        return;
    }
    cx = o->col * TAM_TILE + o->larg * TAM_TILE / 2.0f;
    cy = o->lin * TAM_TILE + o->alt * TAM_TILE / 2.0f;

    if (o->desenhar == obj_barril_fogo) {
        float it = 0.75f + 0.25f * sinf(f->tempo * 17.0f + o->col) * sinf(f->tempo * 11.0f);
        luz(cam, cx, cy, 220.0f, COR(255, 140, 50), it);
    } else if (o->desenhar == obj_portal) {
        float it = 0.8f + 0.2f * sinf(f->tempo * 3.0f);
        luz(cam, cx, cy, 340.0f, COR(255, 25, 35), it);
    } else if (o->desenhar == obj_poste) {
        luz(cam, cx, cy - 48.0f, 290.0f, COR(220, 230, 190), cintilar(o, f->tempo));
    } else {
        luz(cam, cx, cy + 16.0f, 230.0f, COR(255, 235, 170), cintilar(o, f->tempo));
    }
}

static void luzes_renderizar(Fase *f, float x, float y, Camera2D cam)
{
    Objeto *o;

    if (!f->luzesPronta) {
        f->luzes = LoadRenderTexture(LARGURA_TELA, ALTURA_TELA);
        f->luzesPronta = 1;
    }

    BeginTextureMode(f->luzes);
    ClearBackground(COR(52, 58, 74));
    BeginBlendMode(BLEND_ADDITIVE);

    luz(cam, x, y - 24.0f, 320.0f, COR(235, 225, 190), 1.0f);

    for (o = f->objetos; o != NULL; o = o->proximo) {
        luz_do_objeto(f, cam, o);
    }

    EndBlendMode();
    EndTextureMode();
}

static int objeto_visivel(Objeto *o, Camera2D cam)
{
    float x0 = o->col * TAM_TILE;
    float y0 = (o->lin - 2) * TAM_TILE;
    float x1 = (o->col + o->larg) * TAM_TILE;
    float y1 = (o->lin + o->alt) * TAM_TILE;
    float margem = 256.0f;

    if (x1 < cam.target.x - LARGURA_TELA / 2.0f - margem) {
        return 0;
    }
    if (x0 > cam.target.x + LARGURA_TELA / 2.0f + margem) {
        return 0;
    }
    if (y1 < cam.target.y - ALTURA_TELA / 2.0f - margem) {
        return 0;
    }
    if (y0 > cam.target.y + ALTURA_TELA / 2.0f + margem) {
        return 0;
    }
    return 1;
}

static void texto_centro(const char *txt, int y, int tam, Color cor)
{
    int w = MeasureText(txt, tam);

    DrawText(txt, (LARGURA_TELA - w) / 2, y, tam, cor);
}

static void hud_desenhar(Fase *f)
{
    float tfim = f->tempoFim;
    float alfa = 1.0f;

    if (f->atual != f->salaHud) {
        f->salaHud = f->atual;
        f->tempoSalaHud = 0.0f;
    }
    f->tempoSalaHud += GetFrameTime();

    if (f->atual != NULL && tfim <= 0.0f) {
        if (f->tempoSalaHud > 5.0f) {
            alfa = 6.0f - f->tempoSalaHud;
            if (alfa < 0.0f) {
                alfa = 0.0f;
            }
        }
        if (alfa > 0.0f) {
            int a = (int)(255.0f * alfa);

            texto_centro(f->atual->nome, 28, 32, CORA(190, 40, 40, a));
            DrawRectangle(40, ALTURA_TELA - 84, LARGURA_TELA - 80, 52, CORA(0, 0, 0, (int)(170.0f * alfa)));
            texto_centro(f->atual->dica, ALTURA_TELA - 70, 22, CORA(225, 225, 215, a));
        }
        DrawText(f->atual->nome, 16, 12, 16, CORA(170, 170, 160, 200));
        DrawText("ESC: sair", LARGURA_TELA - 90, ALTURA_TELA - 24, 14,
                 CORA(150, 150, 145, 180));
    }

    if (tfim > 0.0f) {
        float a = tfim / 1.5f;

        if (a > 1.0f) {
            a = 1.0f;
        }
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, CORA(18, 0, 2, (int)(235.0f * a)));
        if (tfim > 1.0f) {
            texto_centro("FASE TUTORIAL CONCLUÍDA", ALTURA_TELA / 2 - 40, 40, COR(200, 40, 40));
        }
        if (tfim > 2.0f && ((int)(tfim * 2.0f) % 2) == 0) {
            texto_centro("Pressione ENTER para sair", ALTURA_TELA / 2 + 20, 22, COR(220, 220, 210));
        }
    }
}

void cena_desenhar(Fase *f, float x, float y, DesenharJogador desenhar_jogador)
{
    Camera2D cam = { 0 };
    Vector2 alvo;
    float escala;
    float mw = COLUNAS * TAM_TILE;
    float mh = LINHAS * TAM_TILE;
    Objeto *o;

    alvo.x = x;
    alvo.y = y - 28.0f;
    if (!f->camIniciada) {
        f->camX = alvo.x;
        f->camY = alvo.y;
        f->camIniciada = 1;
    }
    f->camX += (alvo.x - f->camX) * 0.15f;
    f->camY += (alvo.y - f->camY) * 0.15f;

    cam.target.x = floorf(f->camX);
    cam.target.y = floorf(f->camY);
    if (cam.target.x < LARGURA_TELA / 2.0f) {
        cam.target.x = LARGURA_TELA / 2.0f;
    }
    if (cam.target.x > mw - LARGURA_TELA / 2.0f) {
        cam.target.x = mw - LARGURA_TELA / 2.0f;
    }
    if (cam.target.y < ALTURA_TELA / 2.0f) {
        cam.target.y = ALTURA_TELA / 2.0f;
    }
    if (cam.target.y > mh - ALTURA_TELA / 2.0f) {
        cam.target.y = mh - ALTURA_TELA / 2.0f;
    }
    cam.offset = (Vector2){ LARGURA_TELA / 2.0f, ALTURA_TELA / 2.0f };
    cam.rotation = 0.0f;
    cam.zoom = 1.0f;

    luzes_renderizar(f, x, y, cam);

    ClearBackground(COR(4, 4, 6));
    BeginMode2D(cam);

    desenhar_tiles(f, cam);

    for (o = f->objetos; o != NULL; o = o->proximo) {
        if (o->plano == 0 && objeto_visivel(o, cam)) {
            o->desenhar(o, f->tempo);
        }
    }
    for (o = f->objetos; o != NULL; o = o->proximo) {
        if (o->plano == 1 && objeto_visivel(o, cam) &&
            (o->lin + o->alt) * TAM_TILE <= y) {
            o->desenhar(o, f->tempo);
        }
    }
    escala = 1.0f - f->tempoFim / 1.2f;
    if (escala > 0.0f) {
        desenhar_jogador(x, y, escala);
    }
    for (o = f->objetos; o != NULL; o = o->proximo) {
        if (o->plano == 1 && objeto_visivel(o, cam) &&
            (o->lin + o->alt) * TAM_TILE > y) {
            o->desenhar(o, f->tempo);
        }
    }

    EndMode2D();

    BeginBlendMode(BLEND_MULTIPLIED);
    DrawTextureRec(f->luzes.texture, (Rectangle){ 0, 0, LARGURA_TELA, -ALTURA_TELA },
                   (Vector2){ 0, 0 }, WHITE);
    EndBlendMode();

    hud_desenhar(f);
}
