#include "player.h"
#include "mapa.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define TELA_L 1280
#define TELA_A 720
#define PES 36
#define LEON_FRAME_COUNT 8
#define CENARIO_PATH "assets/cenario.png"
#define LUZ_PATH "assets/luz.png"

int main(){
    InitWindow(TELA_L, TELA_A, "Resident Night");
    SetTargetFPS(60);

    if (!mapa_carregar()) {
        TraceLog(LOG_ERROR, "Colisao nao encontrada: %s", MAPA_COLISAO_PATH);
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

    Texture2D luzFixa = LoadTexture(LUZ_PATH);
    if (luzFixa.id == 0) {
        TraceLog(LOG_ERROR, "Luz nao encontrada: %s", LUZ_PATH);
        CloseWindow();
        return 1;
    }

    Image gradiente = GenImageGradientRadial(640, 640, 0.0f, WHITE, BLACK);
    Texture2D luzLeon = LoadTextureFromImage(gradiente);
    UnloadImage(gradiente);

    RenderTexture2D luzTela = LoadRenderTexture(TELA_L, TELA_A);

    Vector2 player = {416, 532};
    float speed = 200.0f;

    Camera2D camera = {0};
    camera.offset = (Vector2){TELA_L / 2.0f, TELA_A / 2.0f};
    camera.zoom = 1.0f;

    while(!WindowShouldClose()){
        Vector2 antes = player;

        PlayerUpdate(&player, speed);

        if (mapa_caixa_bloqueada(player.x, antes.y + PES, 16, 10, 2)) {
            player.x = antes.x;
        }
        if (mapa_caixa_bloqueada(player.x, player.y + PES, 16, 10, 2)) {
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

        Vector2 leonTela = GetWorldToScreen2D(player, camera);

        BeginTextureMode(luzTela);
        ClearBackground((Color){52, 58, 74, 255});
        BeginBlendMode(BLEND_ADDITIVE);
        DrawTextureRec(luzFixa, (Rectangle){camera.target.x - TELA_L / 2.0f, camera.target.y - TELA_A / 2.0f, TELA_L, TELA_A}, (Vector2){0, 0}, WHITE);
        DrawTexture(luzLeon, (int)leonTela.x - 320, (int)leonTela.y - 320, (Color){235, 225, 190, 255});
        EndBlendMode();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);
        DrawTexture(cenario, 0, 0, WHITE);
        DrawTextureEx(leonFrames[frameAtual], (Vector2){player.x - 48, player.y - 48}, 0, 3, WHITE);
        EndMode2D();

        BeginBlendMode(BLEND_MULTIPLIED);
        DrawTextureRec(luzTela.texture, (Rectangle){0, 0, TELA_L, -TELA_A}, (Vector2){0, 0}, WHITE);
        EndBlendMode();

        EndDrawing();
    }

    UnloadTexture(cenario);
    UnloadTexture(luzFixa);
    UnloadTexture(luzLeon);
    UnloadRenderTexture(luzTela);
    for (int i = 0; i < LEON_FRAME_COUNT; i++) {
        UnloadTexture(leonFrames[i]);
    }
    CloseWindow();

    return 0;
}