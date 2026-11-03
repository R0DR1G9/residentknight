#include "player.h"
#include "raymath.h"
#include "raylib.h"

#define CENARIO_PATH "assets/cenario_teste/cenario.png"

int main(){
    InitWindow(1280, 720, "Resident Night");
    SetTargetFPS(60);

    Texture2D cenario = LoadTexture(CENARIO_PATH);
    bool cenarioOk = (cenario.id != 0);
    if (!cenarioOk) TraceLog(LOG_WARNING, "Cenario nao encontrado: %s", CENARIO_PATH);

    Vector2 player ={400,300};
    float speed =200.0f;

    while(!WindowShouldClose()){
        PlayerUpdate(&player, speed);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        if (cenarioOk) 
            DrawTexture(cenario, 0, 0, WHITE);

        DrawCircleV(player, 20, BLUE);

        EndDrawing();
   
    }

    UnloadTexture(cenario);
    CloseWindow();


    return 0;
}

