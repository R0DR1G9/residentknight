#include <stdio.h>
#include "mapa.h"

static char grade[MAPA_LINHAS][MAPA_COLUNAS + 1];

int mapa_carregar(void)
{
    FILE *arq = fopen(MAPA_COLISAO_PATH, "r");
    int l;

    if (arq == NULL) {
        return 0;
    }
    for (l = 0; l < MAPA_LINHAS; l++) {
        if (fscanf(arq, "%42s", grade[l]) != 1) {
            fclose(arq);
            return 0;
        }
    }
    fclose(arq);
    return 1;
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
