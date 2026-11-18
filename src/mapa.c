#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "mapa.h"

static char **grade = NULL;

int mapa_carregar(void)
{
    FILE *arq = fopen(MAPA_COLISAO_PATH, "r");
    char linha[256];
    int l;

    if (arq == NULL) {
        return 0;
    }
    grade = calloc(MAPA_LINHAS, sizeof(char *));
    if (grade == NULL) {
        fclose(arq);
        return 0;
    }
    for (l = 0; l < MAPA_LINHAS; l++) {
        /* fgets (e nao fscanf com largura fixa) para aceitar qualquer largura de mapa e fim de linha \r\n */
        if (fgets(linha, sizeof(linha), arq) == NULL) {
            fclose(arq);
            mapa_liberar();
            return 0;
        }
        linha[strcspn(linha, "\r\n")] = '\0';
        if ((int)strlen(linha) < MAPA_COLUNAS) {
            TraceLog(LOG_ERROR, "colisao.txt: linha %d tem menos de %d colunas", l + 1, MAPA_COLUNAS);
            fclose(arq);
            mapa_liberar();
            return 0;
        }
        grade[l] = malloc(MAPA_COLUNAS + 1);
        if (grade[l] == NULL) {
            fclose(arq);
            mapa_liberar();
            return 0;
        }
        memcpy(grade[l], linha, MAPA_COLUNAS);
        grade[l][MAPA_COLUNAS] = '\0';
    }
    fclose(arq);
    return 1;
}

void mapa_liberar(void)
{
    int l;

    if (grade == NULL) {
        return;
    }
    for (l = 0; l < MAPA_LINHAS; l++) {
        free(grade[l]);
    }
    free(grade);
    grade = NULL;
}

int mapa_bloqueado(float x, float y)
{
    int c = (int)(x / MAPA_TILE);
    int l = (int)(y / MAPA_TILE);

    if (x < 0 || y < 0 || c >= MAPA_COLUNAS || l >= MAPA_LINHAS) {
        return 1;
    }
    return grade[l][c] != '.';
}

int mapa_caixa_bloqueada(float x, float y, float meia, float acima, float abaixo)
{
    return mapa_bloqueado(x - meia, y - acima) ||
           mapa_bloqueado(x + meia, y - acima) ||
           mapa_bloqueado(x - meia, y + abaixo) ||
           mapa_bloqueado(x + meia, y + abaixo);
}