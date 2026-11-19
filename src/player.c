#include "player.h"
#include "raymath.h"

void PlayerUpdate(Vector2 *player, float speed)
{
    Vector2 direcao = {
        IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
        IsKeyDown(KEY_S) - IsKeyDown(KEY_W)
    };
    float passo = speed * GetFrameTime();

    direcao = Vector2Normalize(direcao);
    player->x += direcao.x * passo;
    player->y += direcao.y * passo;
}
