#include "player.h"
#include "raylib.h"
#include <stdio.h>

#define CENARIO_PATH "assets/cenario_teste/cenario_teste.png"   
#define LEON_FRAME_COUNT 8

int main(){
    InitWindow(1280, 720, "Resident Night");

    Texture2D leonFrames[LEON_FRAME_COUNT] = {0};

    for (int i = 0; i < LEON_FRAME_COUNT; i++) {
        char path[100];
        snprintf(path, sizeof(path),
                "assets/leon_run_frames/frame_%02d.png", i + 1);
        leonFrames[i] = LoadTexture(path);
    }
    SetTargetFPS(60);
    int frameAtual = 0;
    float tempoFrame = 0.0f;
    float duracaoFrame = 0.1f;

    Texture2D cenario = LoadTexture(CENARIO_PATH);
    bool cenarioOk = (cenario.id != 0);
    if (!cenarioOk) TraceLog(LOG_WARNING, "Cenario nao encontrado: %s", CENARIO_PATH);

    Vector2 player ={400,300};
    float speed =200.0f;

    while(!WindowShouldClose()){
        PlayerUpdate(&player, speed);
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

        BeginDrawing();
        ClearBackground(DARKGRAY);

        if (cenarioOk) {
            DrawTexturePro(
                cenario,
                (Rectangle){0, 0, cenario.width, cenario.height},
                (Rectangle){0, 0, 1280, 720},
                (Vector2){0, 0},0,WHITE);
        }
        DrawTextureEx(leonFrames[frameAtual],(Vector2){player.x - 48, player.y - 48},0,3,WHITE);

        EndDrawing();
   
    }

    UnloadTexture(cenario);
    for (int i = 0; i < LEON_FRAME_COUNT; i++) {
        UnloadTexture(leonFrames[i]);
    }
    CloseWindow();


    return 0;
}

