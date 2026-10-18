#include "player.h"
#include "raymath.h"

void PlayerUpdate(Vector2 *player, float speed){
    
    float dt = GetFrameTime();

    Vector2 direction = {0};

    if (IsKeyDown(KEY_W)){
        direction.y -= 1;
    }

    if (IsKeyDown(KEY_S)){
        direction.y += 1;
    }

    if (IsKeyDown(KEY_D)){
        direction.x += 1;
    }

    if (IsKeyDown(KEY_A)){
        direction.x -= 1;
    }

    if (Vector2Length(direction) > 0){
        direction = Vector2Normalize(direction);
    }

    player->x += direction.x * speed * dt;
    player->y += direction.y * speed * dt;


}