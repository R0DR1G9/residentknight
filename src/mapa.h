#ifndef MAPA_H
#define MAPA_H

#define MAPA_COLUNAS 42
#define MAPA_LINHAS  34
#define MAPA_TILE    64
#define MAPA_COLISAO_PATH "assets/colisao.txt"

int mapa_carregar(void);
void mapa_liberar(void);
int mapa_bloqueado(float x, float y);
int mapa_caixa_bloqueada(float x, float y, float meia, float acima, float abaixo);

#endif