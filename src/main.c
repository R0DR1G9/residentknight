#include "player.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define TELA_L 1280
#define TELA_A 720
#define LEON_FRAME_COUNT 8
#define CENARIO_PATH "assets/cenario.png"

int main(){
    InitWindow(TELA_L, TELA_A, "Resident Night");
    SetTargetFPS(60);

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
        PlayerUpdate(&player, speed);

        player.x = Clamp(player.x, 48, cenario.width - 48);
        player.y = Clamp(player.y, 48, cenario.height - 48);

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