#include <stdlib.h>
#include "raylib.h"
#include "fase.h"

#define VELOCIDADE 270.0f
#define MEIA_LARGURA 16.0f
#define ACIMA 10.0f
#define ABAIXO 2.0f

static void desenhar_teste(float x, float y, float escala)
{
    DrawEllipse((int)x, (int)y, 18.0f * escala, 6.0f * escala, Fade(BLACK, 0.4f));
    DrawCircle((int)x, (int)(y - 20.0f * escala), 18.0f * escala, BLUE);
}

int main(void)
{
    Fase *fase;
    float x;
    float y;

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Resident Knight - Fase Tutorial");
    SetTargetFPS(60);

    fase = fase_criar();
    if (fase == NULL) {
        CloseWindow();
        return 1;
    }
    x = fase->inicioX;
    y = fase->inicioY;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (dt > 0.05f) {
            dt = 0.05f;
        }

        if (fase->terminou == 0) {
            float dx = 0.0f;
            float dy = 0.0f;
            float novoX;
            float novoY;

            if (IsKeyDown(KEY_W)) {
                dy = dy - 1.0f;
            }
            if (IsKeyDown(KEY_S)) {
                dy = dy + 1.0f;
            }
            if (IsKeyDown(KEY_A)) {
                dx = dx - 1.0f;
            }
            if (IsKeyDown(KEY_D)) {
                dx = dx + 1.0f;
            }
            if (dx != 0.0f && dy != 0.0f) {
                dx = dx * 0.7071f;
                dy = dy * 0.7071f;
            }

            novoX = x + dx * VELOCIDADE * dt;
            if (!fase_caixa_bloqueada(fase, novoX, y, MEIA_LARGURA, ACIMA, ABAIXO)) {
                x = novoX;
            }
            novoY = y + dy * VELOCIDADE * dt;
            if (!fase_caixa_bloqueada(fase, x, novoY, MEIA_LARGURA, ACIMA, ABAIXO)) {
                y = novoY;
            }
        } else {
            fase_puxar_para_portal(fase, &x, &y, dt);
            if (fase->tempoFim > 2.0f && IsKeyPressed(KEY_ENTER)) {
                break;
            }
        }
        fase_atualizar(fase, x, y, dt);

        BeginDrawing();
        cena_desenhar(fase, x, y, desenhar_teste);
        EndDrawing();
    }

    fase_liberar(fase);
    CloseWindow();
    return 0;
}
