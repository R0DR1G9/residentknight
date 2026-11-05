#include "player.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define TELA_L 1280
#define TELA_A 720
#define TILE 64
#define COLUNAS 42
#define LINHAS 34
#define PES 36
#define LEON_FRAME_COUNT 8
#define CENARIO_PATH "assets/cenario.png"
#define COLISAO_PATH "assets/colisao.txt"

static char mapa[LINHAS][COLUNAS + 1];

static int carregar_colisao(void)
{
    FILE *arq = fopen(COLISAO_PATH, "r");

    if (arq == NULL) {
        return 0;
    }
    for (int l = 0; l < LINHAS; l++) {
        if (fscanf(arq, "%42s", mapa[l]) != 1) {
            fclose(arq);
            return 0;
        }
    }
    fclose(arq);
    return 1;
}

static int bloqueado(float x, float y)
{
    int c = (int)(x / TILE);
    int l = (int)(y / TILE);

    if (x < 0 || y < 0 || c >= COLUNAS || l >= LINHAS) {
        return 1;
    }
    return mapa[l][c] != '.';
}

static int caixa_bloqueada(float x, float y)
{
    return bloqueado(x - 16, y - 10) || bloqueado(x + 16, y - 10) ||
           bloqueado(x - 16, y + 2) || bloqueado(x + 16, y + 2);
}

int main(){
    InitWindow(TELA_L, TELA_A, "Resident Night");
    SetTargetFPS(60);

    if (!carregar_colisao()) {
        TraceLog(LOG_ERROR, "Colisao nao encontrada: %s", COLISAO_PATH);
        CloseWindow();
        return 1;
    }

    Texture2D leonFrames[LEON_FRAME_COUNT] = {0};

    for (int i = 0; i < LEON_FRAME_COUNT; i++) {
        char path[100];
        snprintf(path, sizeof(path),
                "assets/leon_run_frames/frame_%02d.png", i + 1);
        leonFrames[i] = LoadTexture(path);
    }
    int frameAtual = 0;
    float tempoFrame = 0.0f;
    float duracaoFrame = 0.1f;

    Texture2D cenario = LoadTexture(CENARIO_PATH);
    if (cenario.id == 0) {
        TraceLog(LOG_ERROR, "Cenario nao encontrado: %s", CENARIO_PATH);
        CloseWindow();
        return 1;
    }

    Vector2 player = {416, 532};
    float speed = 200.0f;

    Camera2D camera = {0};
    camera.offset = (Vector2){TELA_L / 2.0f, TELA_A / 2.0f};
    camera.zoom = 1.0f;

while(!WindowShouldClose()){
        Vector2 antes = player;

        PlayerUpdate(&player, speed);

        if (caixa_bloqueada(player.x, antes.y + PES)) {
            player.x = antes.x;
        }
        if (caixa_bloqueada(player.x, player.y + PES)) {
            player.y = antes.y;
        }

        bool andando = IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D);

        if (andando) {
            tempoFrame += GetFrameTime();

            if (tempoFrame >= duracaoFrame) {
                frameAtual = (frameAtual + 1) % LEON_FRAME_COUNT;
                tempoFrame = 0.0f;
            }
        } else {
            frameAtual = 0;
            tempoFrame = 0.0f;
        }

        camera.target.x = (int)Clamp(player.x, TELA_L / 2.0f, cenario.width - TELA_L / 2.0f);
        camera.target.y = (int)Clamp(player.y, TELA_A / 2.0f, cenario.height - TELA_A / 2.0f);

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);
        DrawTexture(cenario, 0, 0, WHITE);
        DrawTextureEx(leonFrames[frameAtual], (Vector2){player.x - 48, player.y - 48}, 0, 3, WHITE);
        EndMode2D();

        EndDrawing();
    }

    UnloadTexture(cenario);
    for (int i = 0; i < LEON_FRAME_COUNT; i++) {
        UnloadTexture(leonFrames[i]);
    }
    CloseWindow();

    return 0;
}