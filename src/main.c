#include "player.h"
#include "mapa.h"
#include "inimigo.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define TELA_L 1280
#define TELA_A 720
#define PES 36
#define VIDA_MAX 100
#define CENARIO_PATH "assets/cenario.png"
#define LUZ_PATH "assets/luz.png"

int main(){
    InitWindow(TELA_L, TELA_A, "Resident Night");
    SetTargetFPS(60);
    ChangeDirectory(GetApplicationDirectory());

    if (!mapa_carregar()) {
        TraceLog(LOG_ERROR, "Colisao nao encontrada: %s", MAPA_COLISAO_PATH);
        CloseWindow();
        return 1;
    }
    bloqueado_fn = bloqueado;

    Texture2D cenario = LoadTexture(CENARIO_PATH);
    if (cenario.id == 0) {
        TraceLog(LOG_ERROR, "Cenario nao encontrado: %s", CENARIO_PATH);
        CloseWindow();
        return 1;
    }

    Texture2D texCorrer[NUM_ARMAS][8] = {0};
    Texture2D texTiro[NUM_ARMAS][6] = {0};
    const char *pasta[NUM_ARMAS] = {"leon_run_frames", "leon_pistol_frames", "leon_mp5_frames", "leon_shotgun_frames"};
    const char *pastaTiro[NUM_ARMAS] = {"", "pistola", "mp5", "shotgun"};

    for (int a = 0; a < NUM_ARMAS; a++) {
        char path[100];

        for (int i = 0; i < 8; i++) {
            snprintf(path, sizeof(path), a ? "assets/%s/frame_%d.png" : "assets/%s/frame_%02d.png", pasta[a], i + 1);
            texCorrer[a][i] = LoadTexture(path);
        }
        for (int i = 0; a && i < 6; i++) {
            snprintf(path, sizeof(path), "assets/leon_firing_frames/%s/frame_%d.png", pastaTiro[a], i + 1);
            texTiro[a][i] = LoadTexture(path);
        }
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
    float tempoAnim = 0.0f;
    float tempoTiro = 0.0f;
    float invencivel = 0.0f;
    int arma = 1;
    int vida = VIDA_MAX;
    int direita = 1;

    Camera2D camera = {0};
    camera.offset = (Vector2){TELA_L / 2.0f, TELA_A / 2.0f};
    camera.zoom = 1.0f;

    inimigos_criar();

    while(!WindowShouldClose()){
        float dt = GetFrameTime();
        bool andando = false;
        bool atirando = false;

        tempoAnim += dt;
        tempoTiro -= dt;
        invencivel -= dt;

        if (vida > 0) {
            Vector2 antes = player;

            for (int i = 0; i < NUM_ARMAS; i++) {
                if (IsKeyPressed(KEY_ONE + i)) {
                    arma = i;
                }
            }

            PlayerUpdate(&player, speed);

            if (caixa_bloqueada(player.x, antes.y + PES)) {
                player.x = antes.x;
            }
            if (caixa_bloqueada(player.x, player.y + PES)) {
                player.y = antes.y;
            }

            andando = player.x != antes.x || player.y != antes.y;
            if (player.x != antes.x) {
                direita = player.x > antes.x;
            }

            camera.target.x = (int)Clamp(player.x, TELA_L / 2.0f, cenario.width - TELA_L / 2.0f);
            camera.target.y = (int)Clamp(player.y, TELA_A / 2.0f, cenario.height - TELA_A / 2.0f);

            atirando = arma > 0 && (IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT));
            if (atirando) {
                Vector2 alvo = GetScreenToWorld2D(GetMousePosition(), camera);

                direita = alvo.x >= player.x;
                if (tempoTiro <= 0) {
                    atirar(arma, player.x, player.y, alvo.x, alvo.y);
                    tempoTiro = ARMAS[arma].cadencia;
                }
            }

            int dano = inimigos_atualizar(player.x, player.y + PES, dt);
            if (dano > 0 && invencivel <= 0) {
                vida -= dano;
                invencivel = 0.6f;
            }
        } else if (IsKeyPressed(KEY_ENTER)) {
            inimigos_liberar();
            inimigos_criar();
            player = (Vector2){416, 532};
            vida = VIDA_MAX;
        }

        Texture2D tex = atirando ? texTiro[arma][(int)(tempoAnim * 14) % 6]
                                 : texCorrer[arma][andando ? (int)(tempoAnim * 10) % 8 : 0];
        Color cor = (invencivel > 0 && (int)(invencivel * 20) % 2 == 0) ? RED : WHITE;

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
        inimigos_desenhar(-1e9f, player.y + PES);
        DrawTexturePro(tex, (Rectangle){0, 0, direita ? 32 : -32, 32}, (Rectangle){player.x - 48, player.y - 48, 96, 96}, (Vector2){0, 0}, 0, cor);
        inimigos_desenhar(player.y + PES, 1e9f);
        tiros_desenhar();
        EndMode2D();

        BeginBlendMode(BLEND_MULTIPLIED);
        DrawTextureRec(luzTela.texture, (Rectangle){0, 0, TELA_L, -TELA_A}, (Vector2){0, 0}, WHITE);
        EndBlendMode();

        BeginBlendMode(BLEND_MULTIPLIED);
        DrawTextureRec(luzTela.texture, (Rectangle){0, 0, TELA_L, -TELA_A}, (Vector2){0, 0}, WHITE);
        EndBlendMode();

        DrawRectangle(TELA_L - 240, 12, 220 * (vida > 0 ? vida : 0) / VIDA_MAX, 18, vida > 30 ? GREEN : RED);
        DrawText(TextFormat("%s | Zumbis: %d", ARMAS[arma].nome, vivos), TELA_L - 240, 36, 18, RAYWHITE);
        if (vida <= 0) {
            DrawText("VOCE MORREU - ENTER recomeca", 330, 330, 40, RED);
        }
        EndDrawing();
    }

    inimigos_liberar();
    UnloadTexture(cenario);
    UnloadTexture(luzFixa);
    UnloadTexture(luzLeon);
    UnloadRenderTexture(luzTela);
    for (int a = 0; a < NUM_ARMAS; a++) {
        for (int i = 0; i < 8; i++) {
            UnloadTexture(texCorrer[a][i]);
        }
        for (int i = 0; i < 6; i++) {
            UnloadTexture(texTiro[a][i]);
        }
    }
    CloseWindow();

    return 0;
}
