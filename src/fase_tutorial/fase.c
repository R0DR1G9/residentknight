#include <stdlib.h>
#include "fase.h"

static int sala_add(Fase *f, int id, const char *nome, const char *dica,
                    int col, int lin, int larg, int alt)
{
    Sala *s = malloc(sizeof(Sala));
    Sala *ultima;

    if (s == NULL) {
        return 0;
    }
    s->id = id;
    s->nome = nome;
    s->dica = dica;
    s->col = col;
    s->lin = lin;
    s->larg = larg;
    s->alt = alt;
    s->proxima = NULL;

    if (f->salas == NULL) {
        f->salas = s;
    } else {
        ultima = f->salas;
        while (ultima->proxima != NULL) {
            ultima = ultima->proxima;
        }
        ultima->proxima = s;
    }
    return 1;
}

static int corredor_add(Fase *f, int col, int lin, int larg, int alt)
{
    Corredor *c = malloc(sizeof(Corredor));

    if (c == NULL) {
        return 0;
    }
    c->col = col;
    c->lin = lin;
    c->larg = larg;
    c->alt = alt;
    c->proximo = f->corredores;
    f->corredores = c;
    return 1;
}

static int objeto_add(Fase *f, DesenharObjeto desenhar, int col, int lin,
                      int larg, int alt, int solido, int plano, int luz,
                      int variante)
{
    Objeto *o = malloc(sizeof(Objeto));
    int l;
    int c;

    if (o == NULL) {
        return 0;
    }
    o->col = col;
    o->lin = lin;
    o->larg = larg;
    o->alt = alt;
    o->solido = solido;
    o->plano = plano;
    o->luz = luz;
    o->variante = variante;
    o->desenhar = desenhar;
    o->proximo = f->objetos;
    f->objetos = o;

    if (solido) {
        for (l = lin; l < lin + alt; l++) {
            for (c = col; c < col + larg; c++) {
                if (l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS && f->mapa[l][c] == '.') {
                    f->mapa[l][c] = 'o';
                }
            }
        }
    }
    return 1;
}

static int add_solido(Fase *f, DesenharObjeto d, int col, int lin, int larg, int alt, int variante)
{
    return objeto_add(f, d, col, lin, larg, alt, 1, 1, 0, variante);
}

static int add_chao(Fase *f, DesenharObjeto d, int col, int lin, int larg, int alt, int variante)
{
    return objeto_add(f, d, col, lin, larg, alt, 0, 0, 0, variante);
}

static int add_luz(Fase *f, DesenharObjeto d, int col, int lin, int solido, int luz, int variante)
{
    return objeto_add(f, d, col, lin, 1, 1, solido, 1, luz, variante);
}

static int mapa_criar(Fase *f)
{
    int l;
    int c;

    f->mapa = malloc(LINHAS * sizeof(char *));
    if (f->mapa == NULL) {
        return 0;
    }
    for (l = 0; l < LINHAS; l++) {
        f->mapa[l] = NULL;
    }
    for (l = 0; l < LINHAS; l++) {
        f->mapa[l] = malloc(COLUNAS * sizeof(char));
        if (f->mapa[l] == NULL) {
            return 0;
        }
        for (c = 0; c < COLUNAS; c++) {
            f->mapa[l][c] = ' ';
        }
    }
    return 1;
}

static void esculpir(Fase *f, int col, int lin, int larg, int alt)
{
    int l;
    int c;

    for (l = lin; l < lin + alt; l++) {
        for (c = col; c < col + larg; c++) {
            if (l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS) {
                f->mapa[l][c] = '.';
            }
        }
    }
}

static void gerar_paredes(Fase *f)
{
    int l;
    int c;
    int dl;
    int dc;

    for (l = 0; l < LINHAS; l++) {
        for (c = 0; c < COLUNAS; c++) {
            if (f->mapa[l][c] == ' ') {
                int perto = 0;
                for (dl = -1; dl <= 1; dl++) {
                    for (dc = -1; dc <= 1; dc++) {
                        int nl = l + dl;
                        int nc = c + dc;
                        if (nl >= 0 && nl < LINHAS && nc >= 0 && nc < COLUNAS && f->mapa[nl][nc] == '.') {
                            perto = 1;
                        }
                    }
                }
                if (perto) {
                    f->mapa[l][c] = '#';
                }
            }
        }
    }
}

void fase_liberar(Fase *f)
{
    int l;

    if (f == NULL) {
        return;
    }

    if (f->luzesPronta) {
        UnloadRenderTexture(f->luzes);
    }

    if (f->mapa != NULL) {
        for (l = 0; l < LINHAS; l++) {
            free(f->mapa[l]);
        }
        free(f->mapa);
    }

    while (f->salas != NULL) {
        Sala *prox = f->salas->proxima;
        free(f->salas);
        f->salas = prox;
    }

    while (f->corredores != NULL) {
        Corredor *prox = f->corredores->proximo;
        free(f->corredores);
        f->corredores = prox;
    }

    while (f->objetos != NULL) {
        Objeto *prox = f->objetos->proximo;
        free(f->objetos);
        f->objetos = prox;
    }

    free(f);
}

Fase *fase_criar(void)
{
    Fase *f = malloc(sizeof(Fase));
    Sala *s;
    Corredor *c;
    int ok;

    if (f == NULL) {
        return NULL;
    }
    f->mapa = NULL;
    f->salas = NULL;
    f->corredores = NULL;
    f->objetos = NULL;
    f->atual = NULL;
    f->inicioX = 6 * TAM_TILE + TAM_TILE / 2;
    f->inicioY = 8 * TAM_TILE + TAM_TILE - 8;
    f->portalX = 30 * TAM_TILE + TAM_TILE / 2;
    f->portalY = 26 * TAM_TILE + TAM_TILE / 2;
    f->tempo = 0.0f;
    f->terminou = 0;
    f->tempoFim = 0.0f;
    f->camX = 0.0f;
    f->camY = 0.0f;
    f->camIniciada = 0;
    f->salaHud = NULL;
    f->tempoSalaHud = 0.0f;
    f->luzesPronta = 0;

    ok = mapa_criar(f);

    ok = ok && sala_add(f, 0, "ABRIGO",
                        "Use W A S D para andar. Siga o corredor para leste.",
                        3, 4, 13, 9);
    ok = ok && sala_add(f, 1, "HALL EM RUÍNAS",
                        "O prédio desabou. Siga o corredor ao sul.",
                        24, 3, 13, 11);
    ok = ok && sala_add(f, 2, "LABORATÓRIO DE EVACUAÇÃO",
                        "Entre no portal para escapar.",
                        23, 20, 15, 11);
    ok = ok && corredor_add(f, 16, 7, 8, 3);
    ok = ok && corredor_add(f, 29, 14, 3, 6);

    if (!ok) {
        fase_liberar(f);
        return NULL;
    }

    s = f->salas;
    while (s != NULL) {
        esculpir(f, s->col, s->lin, s->larg, s->alt);
        s = s->proxima;
    }
    c = f->corredores;
    while (c != NULL) {
        esculpir(f, c->col, c->lin, c->larg, c->alt);
        c = c->proximo;
    }
    gerar_paredes(f);
    f->atual = f->salas;

    ok = ok && add_chao(f, obj_faixa, 16, 7, 1, 3, 0);
    ok = ok && add_chao(f, obj_faixa, 23, 7, 1, 3, 0);
    ok = ok && add_chao(f, obj_faixa, 29, 14, 3, 1, 0);
    ok = ok && add_chao(f, obj_faixa, 29, 19, 3, 1, 0);

    ok = ok && add_luz(f, obj_lampada_parede, 7, 3, 0, 2, 0);
    ok = ok && add_luz(f, obj_lampada_parede, 12, 3, 0, 2, 0);
    ok = ok && add_solido(f, obj_maquina, 4, 4, 1, 1, 0);
    ok = ok && add_solido(f, obj_bau, 5, 4, 1, 1, 0);
    ok = ok && add_solido(f, obj_placa, 8, 6, 1, 1, 0);
    ok = ok && add_solido(f, obj_caixote, 3, 11, 1, 1, 0);
    ok = ok && add_solido(f, obj_caixote, 4, 11, 1, 1, 0);
    ok = ok && add_solido(f, obj_caixote, 3, 12, 1, 1, 1);
    ok = ok && add_solido(f, obj_caixote, 4, 12, 1, 1, 0);
    ok = ok && add_solido(f, obj_caixote, 3, 10, 1, 1, 1);
    ok = ok && add_solido(f, obj_barril, 14, 12, 1, 1, 0);
    ok = ok && add_solido(f, obj_barril, 15, 12, 1, 1, 1);
    ok = ok && add_solido(f, obj_sacos, 12, 5, 2, 1, 0);
    ok = ok && add_solido(f, obj_bloco, 9, 11, 1, 1, 0);
    ok = ok && add_solido(f, obj_bloco, 10, 11, 1, 1, 0);
    ok = ok && add_chao(f, obj_sangue, 10, 8, 2, 1, 1);
    ok = ok && add_chao(f, obj_sangue, 6, 6, 1, 1, 0);
    ok = ok && add_chao(f, obj_sangue, 12, 10, 1, 1, 2);

    ok = ok && add_luz(f, obj_lampada_parede, 27, 2, 0, 2, 1);
    ok = ok && add_luz(f, obj_lampada_parede, 33, 2, 0, 2, 0);
    ok = ok && add_solido(f, obj_placa, 25, 5, 1, 1, 0);
    ok = ok && add_solido(f, obj_carro, 28, 10, 3, 2, 0);
    ok = ok && add_luz(f, obj_barril_fogo, 26, 12, 1, 1, 0);
    ok = ok && add_luz(f, obj_barril_fogo, 35, 5, 1, 1, 0);
    ok = ok && add_solido(f, obj_sacos, 32, 7, 2, 1, 0);
    ok = ok && add_solido(f, obj_bloco, 33, 10, 1, 1, 0);
    ok = ok && add_solido(f, obj_bloco, 34, 10, 1, 1, 0);
    ok = ok && add_solido(f, obj_caixote, 35, 12, 1, 1, 0);
    ok = ok && add_solido(f, obj_caixote, 36, 12, 1, 1, 0);
    ok = ok && add_solido(f, obj_caixote, 36, 11, 1, 1, 1);
    ok = ok && add_solido(f, obj_barril, 33, 12, 1, 1, 2);
    ok = ok && add_solido(f, obj_barril, 24, 4, 1, 1, 0);
    ok = ok && add_luz(f, obj_poste, 31, 3, 1, 2, 1);
    ok = ok && add_chao(f, obj_sangue, 27, 8, 2, 1, 1);
    ok = ok && add_chao(f, obj_sangue, 31, 11, 1, 1, 0);
    ok = ok && add_chao(f, obj_sangue, 34, 8, 1, 1, 2);
    ok = ok && add_chao(f, obj_sangue, 25, 12, 1, 1, 2);

    ok = ok && add_luz(f, obj_lampada_parede, 25, 19, 0, 2, 0);
    ok = ok && add_luz(f, obj_lampada_parede, 35, 19, 0, 2, 1);
    ok = ok && add_solido(f, obj_placa, 27, 21, 1, 1, 0);
    ok = ok && add_solido(f, obj_bloco, 26, 23, 1, 1, 0);
    ok = ok && add_solido(f, obj_bloco, 27, 23, 1, 1, 0);
    ok = ok && add_solido(f, obj_bloco, 33, 23, 1, 1, 0);
    ok = ok && add_solido(f, obj_bloco, 34, 23, 1, 1, 0);
    ok = ok && add_solido(f, obj_caixote, 24, 28, 1, 1, 1);
    ok = ok && add_solido(f, obj_caixote, 25, 28, 1, 1, 0);
    ok = ok && add_solido(f, obj_caixote, 24, 27, 1, 1, 1);
    ok = ok && add_solido(f, obj_barril, 36, 22, 1, 1, 2);
    ok = ok && add_solido(f, obj_barril, 37, 22, 1, 1, 1);
    ok = ok && add_solido(f, obj_barril, 36, 29, 1, 1, 0);
    ok = ok && add_luz(f, obj_barril_fogo, 24, 21, 1, 1, 0);
    ok = ok && add_luz(f, obj_barril_fogo, 37, 29, 1, 1, 0);
    ok = ok && add_luz(f, obj_poste, 36, 26, 1, 2, 0);
    ok = ok && add_chao(f, obj_sangue, 29, 22, 3, 1, 1);
    ok = ok && add_chao(f, obj_sangue, 30, 24, 1, 1, 0);
    ok = ok && add_chao(f, obj_sangue, 26, 26, 1, 1, 2);
    ok = ok && add_solido(f, obj_pilar, 28, 25, 1, 1, 0);
    ok = ok && add_solido(f, obj_pilar, 32, 25, 1, 1, 0);
    ok = ok && objeto_add(f, obj_portal, 29, 25, 3, 3, 0, 0, 3, 0);

    if (!ok) {
        fase_liberar(f);
        return NULL;
    }
    return f;
}

int fase_bloqueado(Fase *f, float x, float y)
{
    int c;
    int l;

    if (x < 0.0f || y < 0.0f) {
        return 1;
    }
    c = (int)(x / TAM_TILE);
    l = (int)(y / TAM_TILE);
    if (c >= COLUNAS || l >= LINHAS) {
        return 1;
    }
    return f->mapa[l][c] != '.';
}

int fase_caixa_bloqueada(Fase *f, float x, float y, float meiaLarg, float acima, float abaixo)
{
    if (fase_bloqueado(f, x - meiaLarg, y - acima)) {
        return 1;
    }
    if (fase_bloqueado(f, x + meiaLarg, y - acima)) {
        return 1;
    }
    if (fase_bloqueado(f, x - meiaLarg, y + abaixo)) {
        return 1;
    }
    if (fase_bloqueado(f, x + meiaLarg, y + abaixo)) {
        return 1;
    }
    return 0;
}

void fase_atualizar(Fase *f, float x, float y, float dt)
{
    Sala *s = f->salas;
    int c = (int)(x / TAM_TILE);
    int l = (int)(y / TAM_TILE);
    float dx = x - f->portalX;
    float dy = y - f->portalY;

    f->tempo += dt;

    while (s != NULL) {
        if (c >= s->col && c < s->col + s->larg && l >= s->lin && l < s->lin + s->alt) {
            f->atual = s;
        }
        s = s->proxima;
    }

    if (dx * dx + dy * dy < 44.0f * 44.0f) {
        f->terminou = 1;
    }
    if (f->terminou) {
        f->tempoFim += dt;
    }
}

void fase_puxar_para_portal(Fase *f, float *x, float *y, float dt)
{
    float forca = 5.0f * dt;

    if (forca > 1.0f) {
        forca = 1.0f;
    }
    *x = *x + (f->portalX - *x) * forca;
    *y = *y + (f->portalY - *y) * forca;
}
