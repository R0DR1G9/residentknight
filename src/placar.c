#include <stdio.h>
#include <stdlib.h>
#include "placar.h"

static Registro *lista = NULL;

static int inserir(float tempo)
{
    Registro **p = &lista;
    Registro *novo;
    int posicao = 1;
    int total = 0;

    while (*p != NULL && (*p)->tempo <= tempo) {
        p = &(*p)->prox;
        posicao++;
    }
    novo = malloc(sizeof(Registro));
    if (novo == NULL) {
        return 0;
    }
    novo->tempo = tempo;
    novo->prox = *p;
    *p = novo;

    p = &lista;
    while (*p != NULL && total < PLACAR_MAX) {
        p = &(*p)->prox;
        total++;
    }
    while (*p != NULL) {
        Registro *sobra = *p;

        *p = sobra->prox;
        free(sobra);
    }
    if (posicao > PLACAR_MAX) {
        return 0;
    }
    return posicao;
}

static void salvar(void)
{
    FILE *arq = fopen(PLACAR_ARQUIVO, "w");

    if (arq == NULL) {
        return;
    }
    for (Registro *r = lista; r != NULL; r = r->prox) {
        fprintf(arq, "%.2f\n", r->tempo);
    }
    fclose(arq);
}

void placar_liberar(void)
{
    while (lista != NULL) {
        Registro *proximo = lista->prox;

        free(lista);
        lista = proximo;
    }
}

void placar_carregar(void)
{
    FILE *arq = fopen(PLACAR_ARQUIVO, "r");
    float tempo;

    placar_liberar();
    if (arq == NULL) {
        return;
    }
    while (fscanf(arq, "%f", &tempo) == 1) {
        if (tempo > 0.0f) {
            inserir(tempo);
        }
    }
    fclose(arq);
}

int placar_registrar(float tempo)
{
    int posicao = inserir(tempo);

    salvar();
    return posicao;
}

int placar_quantidade(void)
{
    int total = 0;

    for (Registro *r = lista; r != NULL; r = r->prox) {
        total++;
    }
    return total;
}

float placar_tempo(int posicao)
{
    Registro *r = lista;

    for (int k = 1; k < posicao && r != NULL; k++) {
        r = r->prox;
    }
    if (r == NULL) {
        return 0.0f;
    }
    return r->tempo;
}
