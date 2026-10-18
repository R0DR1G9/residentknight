#include "player.h"
#include "raymath.h"

int main(){
    InitWindow(1280, 720, "Resident Night");
    SetTargetFPS(60);

    Vector2 player ={400,300};
    float speed =200.0f;

    while(!WindowShouldClose()){
        PlayerUpdate(&player, speed);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawCircleV(player, 20, BLUE);

        EndDrawing();
   
    }

    CloseWindow;

    return 0;
}