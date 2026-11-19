#include "player.h"
#include "mapa.h"
#include "inimigo.h"
#include "placar.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define TELA_L 1280
#define TELA_A 720
#define PES 36
#define VIDA_MAX 100
#define VEL_JOGADOR 200.0f
#define INICIO_X 312.0f
#define INICIO_Y 420.0f
#define CENARIO_PATH "assets/mapa.png"
#define LUZ_PATH "assets/iluminacao.png"
#define MUSICA_PATH "assets/audio/ambiente.ogg"
#define MUSICA_VOLUME 0.5f

#define IDLE_FRAMES 8
#define IDLE_VEL 0.15f

#define PORTAL_COL 38
#define PORTAL_LIN 39
#define PORTAL_X (PORTAL_COL * MAPA_TILE + MAPA_TILE / 2)
#define PORTAL_Y (PORTAL_LIN * MAPA_TILE + MAPA_TILE - 8)
#define PORTAL_ALCANCE 80.0f

#define AVISO_TEMPO 5.0f
#define CREDITOS_VEL 70.0f
#define ESPACO_LINHA 14
#define CRED_ESPACO 50
#define CRED_TITULO 28
#define CRED_TEXTO 24

#define ZOOM_CAMERA 1.5f
#define VERSAO_JOGO "BETA 0.1"
#define MENU_ITENS 3
#define DIFICIL_VEL 1.35f
#define DIFICIL_DANO 1.5f

typedef enum { MENU, JOGANDO, CREDITOS } Estado;

typedef struct {
    const char *texto;
    int tam;
    int destaque;
} LinhaCredito;

typedef struct {
    Estado estado;
    int menuSel, posicaoPlacar;
    bool dificil, sair, mostrarTile;
    float tempoIntro, tempoPortal, tempoPartida, creditosY;
} Jogo;

typedef struct {
    Texture2D cenario, luzFixa, luzLeon;
    RenderTexture2D luzTela;
    Music musica;
    bool temMusica;
    Texture2D correr[NUM_ARMAS][8], tiro[NUM_ARMAS][6], parado[IDLE_FRAMES];
} Recursos;

static const LinhaCredito LINHAS_CREDITOS[] = {
    {"CEDRO PRODUÇÕES", 64, 1},
    {"", 60, 0},
    {"DESENVOLVEDORES", 28, 1},
    {"Arthur Siqueira", 24, 0},
    {"Rodrigo Gomes", 24, 0},
    {"João Miguel", 24, 0},
    {"", 50, 0},
    {"ARTE", 28, 1},
    {"Arthur Siqueira", 24, 0},
    {"", 70, 0},
    {"OBRIGADO POR TER JOGADO!", 44, 1}
};
#define NUM_LINHAS_CREDITOS ((int)(sizeof(LINHAS_CREDITOS) / sizeof(LINHAS_CREDITOS[0])))

static const char *PASTA_CORRER[NUM_ARMAS] = {"leon_run_frames", "leon_pistol_frames", "leon_mp5_frames", "leon_shotgun_frames"};
static const char *PASTA_TIRO[NUM_ARMAS] = {"", "leon_firing_frames/pistola", "leon_firing_frames/mp5", "leon_firing_frames/shotgun"};

static void carregar_serie(Texture2D *destino, int n, const char *formato, const char *pasta)
{
    for (int i = 0; i < n; i++) {
        const char *path = TextFormat(formato, pasta, i + 1);

        destino[i] = LoadTexture(path);
        if (destino[i].id == 0) {
            TraceLog(LOG_ERROR, "Imagem nao encontrada: %s", path);
        }
        SetTextureFilter(destino[i], TEXTURE_FILTER_POINT);
    }
}

static void descarregar_serie(Texture2D *serie, int n)
{
    for (int i = 0; i < n; i++) {
        UnloadTexture(serie[i]);
    }
}

static int recursos_carregar(Recursos *r)
{
    Image gradiente;

    r->cenario = LoadTexture(CENARIO_PATH);
    r->luzFixa = LoadTexture(LUZ_PATH);
    if (r->cenario.id == 0 || r->luzFixa.id == 0) {
        TraceLog(LOG_ERROR, "Cenario ou iluminacao nao encontrados: %s / %s", CENARIO_PATH, LUZ_PATH);
        return 0;
    }
    for (int a = 0; a < NUM_ARMAS; a++) {
        carregar_serie(r->correr[a], 8, a ? "assets/%s/frame_%d.png" : "assets/%s/frame_%02d.png", PASTA_CORRER[a]);
        if (a) {
            carregar_serie(r->tiro[a], 6, "assets/%s/frame_%d.png", PASTA_TIRO[a]);
        }
    }
    carregar_serie(r->parado, IDLE_FRAMES, "assets/%s/frame_%02d.png", "leon_idle_frames");
    gradiente = GenImageGradientRadial(640, 640, 0.0f, WHITE, BLACK);
    r->luzLeon = LoadTextureFromImage(gradiente);
    UnloadImage(gradiente);
    r->luzTela = LoadRenderTexture(TELA_L, TELA_A);
    if (IsAudioDeviceReady()) {
        r->musica = LoadMusicStream(MUSICA_PATH);
        r->temMusica = IsMusicValid(r->musica);
    }
    if (r->temMusica) {
        r->musica.looping = true;
        SetMusicVolume(r->musica, MUSICA_VOLUME);
        PlayMusicStream(r->musica);
    } else {
        TraceLog(LOG_WARNING, "Musica indisponivel: %s", MUSICA_PATH);
    }
    return 1;
}

static void recursos_descarregar(Recursos *r)
{
    UnloadTexture(r->cenario);
    UnloadTexture(r->luzFixa);
    UnloadTexture(r->luzLeon);
    UnloadRenderTexture(r->luzTela);
    if (r->temMusica) {
        StopMusicStream(r->musica);
        UnloadMusicStream(r->musica);
    }
    for (int a = 0; a < NUM_ARMAS; a++) {
        descarregar_serie(r->correr[a], 8);
        descarregar_serie(r->tiro[a], 6);
    }
    descarregar_serie(r->parado, IDLE_FRAMES);
}

static void centro(const char *txt, int y, int tam, Color cor)
{
    DrawText(txt, (TELA_L - MeasureText(txt, tam)) / 2, y, tam, cor);
}

static void reiniciar(Jogo *j, Jogador *leon)
{
    inimigos_liberar();
    inimigos_criar();
    leon->pos = (Vector2){INICIO_X, INICIO_Y};
    leon->vida = VIDA_MAX;
    j->tempoIntro = 0.0f;
    j->tempoPartida = 0.0f;
}

static void novo_jogo(Jogo *j, Jogador *leon)
{
    inimigo_mult_vel = j->dificil ? DIFICIL_VEL : 1.0f;
    inimigo_mult_dano = j->dificil ? DIFICIL_DANO : 1.0f;
    reiniciar(j, leon);
    leon->arma = 1;
    leon->direita = 1;
    leon->tempoTiro = 0.25f;
    leon->invencivel = 0.0f;
    j->estado = JOGANDO;
}

static bool portal_aberto(const Jogador *leon)
{
    return leon->vida > 0 && vivos == 0;
}

static bool perto_portal(const Jogador *leon)
{
    float dx = leon->pos.x - PORTAL_X;
    float dy = leon->pos.y + PES - PORTAL_Y;

    return portal_aberto(leon) && dx * dx + dy * dy < PORTAL_ALCANCE * PORTAL_ALCANCE;
}

static Rectangle menu_item(int k)
{
    return (Rectangle){TELA_L / 2.0f - 230, 330.0f + k * 66, 460, 52};
}

static void menu_atualizar(Jogo *j, Jogador *leon)
{
    Vector2 mouse = GetMousePosition();
    Vector2 mexe = GetMouseDelta();
    bool trocar = IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D);

    if (mexe.x != 0 || mexe.y != 0) {
        for (int k = 0; k < MENU_ITENS; k++) {
            if (CheckCollisionPointRec(mouse, menu_item(k))) {
                j->menuSel = k;
            }
        }
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        j->menuSel = (j->menuSel + 1) % MENU_ITENS;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        j->menuSel = (j->menuSel + MENU_ITENS - 1) % MENU_ITENS;
    }
    if (j->menuSel == 1 && trocar) {
        j->dificil = !j->dificil;
    }
    if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, menu_item(j->menuSel)))) {
        if (j->menuSel == 0) {
            novo_jogo(j, leon);
        } else if (j->menuSel == 1) {
            j->dificil = !j->dificil;
        } else {
            j->sair = true;
        }
    }
}

static void menu_desenhar(const Jogo *j, const Recursos *r)
{
    float t = (float)GetTime();
    float sx = fmaxf(0.0f, (r->cenario.width - TELA_L) / 2.0f * (1.0f + 0.8f * sinf(t * 0.1f)));
    float sy = fmaxf(0.0f, (r->cenario.height - TELA_A) / 2.0f);
    float pisca = 0.85f + 0.15f * sinf(t * 7.0f) * sinf(t * 3.3f);
    const char *titulo = "RESIDENT NIGHT";
    const char *desc = j->dificil ? "Difícil: os inimigos causam mais dano e são mais rápidos"
                                  : "Normal: a experiência padrão do jogo";

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTextureRec(r->cenario, (Rectangle){sx, sy, TELA_L, TELA_A}, (Vector2){0, 0}, (Color){80, 80, 95, 255});
    DrawRectangle(0, 0, TELA_L, TELA_A, Fade(BLACK, 0.55f));
    DrawRectangleGradientV(0, 0, TELA_L, 200, BLACK, (Color){0, 0, 0, 0});
    DrawRectangleGradientV(0, TELA_A - 160, TELA_L, 160, (Color){0, 0, 0, 0}, BLACK);

    DrawText(titulo, (TELA_L - MeasureText(titulo, 84)) / 2 + 4, 104, 84, Fade(BLACK, 0.8f));
    centro(titulo, 100, 84, Fade(RED, pisca));
    centro(TextFormat("VERSÃO %s", VERSAO_JOGO), 200, 24, (Color){230, 200, 80, 255});

    for (int k = 0; k < MENU_ITENS; k++) {
        Rectangle caixa = menu_item(k);
        bool sel = k == j->menuSel;
        const char *txt = k == 0 ? "NOVO JOGO"
                        : k == 1 ? TextFormat("DIFICULDADE: < %s >", j->dificil ? "DIFÍCIL" : "NORMAL")
                                 : "SAIR";
        Color cor = k == 1 && j->dificil ? (Color){255, 110, 90, 255} : (sel ? RAYWHITE : GRAY);

        DrawRectangleRec(caixa, sel ? Fade(RED, 0.25f) : Fade(BLACK, 0.45f));
        if (sel) {
            DrawRectangleLinesEx(caixa, 2, RED);
        }
        DrawText(txt, (int)(caixa.x + (caixa.width - MeasureText(txt, 28)) / 2), (int)caixa.y + 12, 28, cor);
    }
    centro(desc, 530, 18, j->dificil ? ORANGE : LIGHTGRAY);
    centro("W/S ou setas: navegar   |   ENTER: selecionar   |   A/D: mudar dificuldade   |   o mouse também funciona", TELA_A - 30, 16, GRAY);
    EndDrawing();
}

static const char *formatar_tempo(float t)
{
    return TextFormat("%02d:%05.2f", (int)(t / 60.0f), fmodf(t, 60.0f));
}

static float linha_credito(const char *txt, float y, int tam, Color cor)
{
    if (txt[0] != '\0' && y > -tam && y < TELA_A) {
        centro(txt, (int)y, tam, cor);
    }
    return y + tam + ESPACO_LINHA;
}

static float creditos_altura(void)
{
    float total = 5 * ESPACO_LINHA + 2 * CRED_ESPACO + 2 * CRED_TITULO + CRED_TEXTO;

    for (int k = 0; k < NUM_LINHAS_CREDITOS; k++) {
        total += LINHAS_CREDITOS[k].tam + ESPACO_LINHA;
    }
    return total + placar_quantidade() * (CRED_TEXTO + ESPACO_LINHA);
}

static void creditos_atualizar(Jogo *j, float dt)
{
    float vel = IsKeyDown(KEY_SPACE) ? CREDITOS_VEL * 4 : CREDITOS_VEL;
    float parada = TELA_A / 2.0f + 30 - creditos_altura();

    if (IsKeyPressed(KEY_ENTER)) {
        j->estado = MENU;
        j->menuSel = 0;
        return;
    }
    j->creditosY = fmaxf(j->creditosY - vel * dt, parada);
}

static void creditos_desenhar(const Jogo *j)
{
    float y = j->creditosY;

    BeginDrawing();
    ClearBackground(BLACK);
    for (int k = 0; k < NUM_LINHAS_CREDITOS; k++) {
        const LinhaCredito *l = &LINHAS_CREDITOS[k];

        y = linha_credito(l->texto, y, l->tam, l->destaque ? RED : RAYWHITE);
    }
    y = linha_credito("", y, CRED_ESPACO, RAYWHITE);
    y = linha_credito("SEU TEMPO", y, CRED_TITULO, RED);
    y = linha_credito(formatar_tempo(j->tempoPartida), y, CRED_TEXTO, RAYWHITE);
    y = linha_credito("", y, CRED_ESPACO, RAYWHITE);
    y = linha_credito("MELHORES TEMPOS", y, CRED_TITULO, RED);
    for (int k = 1; k <= placar_quantidade(); k++) {
        const char *txt = TextFormat("%d. %s", k, formatar_tempo(placar_tempo(k)));

        y = linha_credito(txt, y, CRED_TEXTO, k == j->posicaoPlacar ? SKYBLUE : RAYWHITE);
    }
    DrawRectangleGradientV(0, 0, TELA_L, 90, BLACK, (Color){0, 0, 0, 0});
    DrawRectangleGradientV(0, TELA_A - 90, TELA_L, 90, (Color){0, 0, 0, 0}, BLACK);
    centro("ENTER - voltar ao menu   |   ESC - sair   |   segure ESPACO para acelerar", TELA_A - 30, 16, GRAY);
    EndDrawing();
}

static void mover_leon(Jogador *leon)
{
    Vector2 antes = leon->pos;

    PlayerUpdate(&leon->pos, VEL_JOGADOR);
    if (mapa_caixa_bloqueada(leon->pos.x, antes.y + PES, 16, 10, 2)) {
        leon->pos.x = antes.x;
    }
    if (mapa_caixa_bloqueada(leon->pos.x, leon->pos.y + PES, 16, 10, 2)) {
        leon->pos.y = antes.y;
    }
    leon->andando = leon->pos.x != antes.x || leon->pos.y != antes.y;
    if (leon->pos.x != antes.x) {
        leon->direita = leon->pos.x > antes.x;
    }
}

static void leon_atualizar(Jogador *leon, Camera2D *cam, const Recursos *r, float dt)
{
    float meiaL = TELA_L / 2.0f / cam->zoom;
    float meiaA = TELA_A / 2.0f / cam->zoom;
    int dano;

    for (int i = 0; i < NUM_ARMAS; i++) {
        if (IsKeyPressed(KEY_ONE + i)) {
            leon->arma = i;
        }
    }
    mover_leon(leon);
    cam->target.x = (int)Clamp(leon->pos.x, meiaL, r->cenario.width - meiaL);
    cam->target.y = (int)Clamp(leon->pos.y, meiaA, r->cenario.height - meiaA);

    leon->atirando = leon->arma > 0 && (IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT));
    if (leon->atirando) {
        Vector2 alvo = GetScreenToWorld2D(GetMousePosition(), *cam);

        leon->direita = alvo.x >= leon->pos.x;
        if (leon->tempoTiro <= 0) {
            atirar(leon->arma, leon->pos.x, leon->pos.y, alvo.x, alvo.y);
            leon->tempoTiro = ARMAS[leon->arma].cadencia;
        }
    }

    dano = inimigos_atualizar(leon->pos.x, leon->pos.y + PES, dt);
    if (dano > 0 && leon->invencivel <= 0) {
        leon->vida -= dano;
        leon->invencivel = 0.6f;
    }
}

static void jogo_atualizar(Jogo *j, Jogador *leon, Camera2D *cam, const Recursos *r, float dt)
{
    j->tempoIntro += dt;
    leon->tempoAnim += dt;
    leon->tempoTiro -= dt;
    leon->invencivel -= dt;
    leon->andando = leon->atirando = 0;

    if (leon->vida > 0) {
        j->tempoPartida += dt;
        leon_atualizar(leon, cam, r, dt);
    } else if (IsKeyPressed(KEY_ENTER)) {
        reiniciar(j, leon);
    } else if (IsKeyPressed(KEY_M)) {
        j->estado = MENU;
        j->menuSel = 0;
    }

    if (portal_aberto(leon)) {
        j->tempoPortal += dt;
        if (perto_portal(leon) && IsKeyPressed(KEY_E)) {
            j->posicaoPlacar = placar_registrar(j->tempoPartida);
            j->estado = CREDITOS;
            j->creditosY = TELA_A;
        }
    } else {
        j->tempoPortal = 0.0f;
    }

    if (IsKeyPressed(KEY_F1)) {
        j->mostrarTile = !j->mostrarTile;
    }
}

static void desenhar_aviso(const char *titulo, const char *texto, float t)
{
    float alfa = Clamp(fminf(t / 0.5f, AVISO_TEMPO - t), 0.0f, 1.0f);

    DrawRectangle(0, 110, TELA_L, 110, Fade(BLACK, 0.6f * alfa));
    centro(titulo, 128, 24, Fade(RED, alfa));
    centro(texto, 164, 44, Fade(RAYWHITE, alfa));
}

static void desenhar_portal(float x, float y)
{
    float t = (float)GetTime();
    float pulso = 0.5f + 0.5f * sinf(t * 3.0f);
    float cy = y - 44;

    DrawEllipse((int)x, (int)y, 34, 9, Fade(BLACK, 0.4f));
    DrawEllipse((int)x, (int)cy, (int)(34 + 4 * pulso), (int)(54 + 4 * pulso), Fade((Color){40, 120, 255, 255}, 0.30f));
    DrawEllipse((int)x, (int)cy, 28, 48, Fade((Color){80, 170, 255, 255}, 0.55f));
    DrawEllipse((int)x, (int)cy, 20, 38, Fade((Color){200, 235, 255, 255}, 0.85f));
    DrawEllipseLines((int)x, (int)cy, 30, 50, SKYBLUE);
    for (int k = 0; k < 6; k++) {
        float a = t * 2.0f + k * (2.0f * PI / 6.0f);

        DrawCircle((int)(x + cosf(a) * 22), (int)(cy + sinf(a) * 40), 3, Fade(WHITE, 0.9f));
    }
}

static Texture2D frame_leon(const Jogador *leon, const Recursos *r)
{
    if (leon->atirando) {
        return r->tiro[leon->arma][(int)(leon->tempoAnim * 14) % 6];
    }
    if (leon->andando) {
        return r->correr[leon->arma][(int)(leon->tempoAnim * 10) % 8];
    }
    if (leon->arma == 0) {
        return r->parado[(int)(leon->tempoAnim / IDLE_VEL) % IDLE_FRAMES];
    }
    return r->correr[leon->arma][0];
}

static void desenhar_luz(const Jogador *leon, const Camera2D *cam, const Recursos *r)
{
    Vector2 leonTela = GetWorldToScreen2D(leon->pos, *cam);
    Rectangle visao = {cam->target.x - TELA_L / 2.0f / cam->zoom, cam->target.y - TELA_A / 2.0f / cam->zoom, TELA_L / cam->zoom, TELA_A / cam->zoom};

    BeginTextureMode(r->luzTela);
    ClearBackground((Color){52, 58, 74, 255});
    BeginBlendMode(BLEND_ADDITIVE);
    DrawTexturePro(r->luzFixa, visao, (Rectangle){0, 0, TELA_L, TELA_A}, (Vector2){0, 0}, 0, WHITE);
    DrawTexture(r->luzLeon, (int)leonTela.x - 320, (int)leonTela.y - 320, (Color){235, 225, 190, 255});
    if (portal_aberto(leon)) {
        Vector2 p = GetWorldToScreen2D((Vector2){PORTAL_X, PORTAL_Y - 44}, *cam);

        DrawTexturePro(r->luzLeon, (Rectangle){0, 0, 640, 640}, (Rectangle){p.x - 180 * cam->zoom, p.y - 180 * cam->zoom, 360 * cam->zoom, 360 * cam->zoom},
                       (Vector2){0, 0}, 0, (Color){60, 140, 255, 255});
    }
    EndBlendMode();
    EndTextureMode();
}

static void desenhar_mundo(const Jogador *leon, const Camera2D *cam, const Recursos *r)
{
    Color cor = (leon->invencivel > 0 && (int)(leon->invencivel * 20) % 2 == 0) ? RED : WHITE;
    float pes = leon->pos.y + PES;

    BeginMode2D(*cam);
    DrawTexture(r->cenario, 0, 0, WHITE);
    if (portal_aberto(leon)) {
        desenhar_portal(PORTAL_X, PORTAL_Y);
    }
    inimigos_desenhar(-1e9f, pes);
    DrawTexturePro(frame_leon(leon, r), (Rectangle){0, 0, leon->direita ? 32 : -32, 32},
                   (Rectangle){leon->pos.x - 48, leon->pos.y - 48, 96, 96}, (Vector2){0, 0}, 0, cor);
    inimigos_desenhar(pes, 1e9f);
    tiros_desenhar();
    EndMode2D();
}

static void desenhar_hud(const Jogo *j, const Jogador *leon, const Camera2D *cam)
{
    const char *objetivo = vivos == 0 ? "OBJETIVO: entre no portal" : TextFormat("OBJETIVO: mate todos os zumbis (%d)", vivos);
    int vida = leon->vida > 0 ? leon->vida : 0;

    DrawRectangle(10, 10, MeasureText(objetivo, 18) + 20, 30, Fade(BLACK, 0.55f));
    DrawText(objetivo, 20, 16, 18, vivos == 0 ? SKYBLUE : RAYWHITE);
    if (j->mostrarTile) {
        DrawText(TextFormat("Tile do Leon: col %d, lin %d", (int)(leon->pos.x / MAPA_TILE), (int)((leon->pos.y + PES) / MAPA_TILE)), 20, 50, 18, YELLOW);
    }

    if (j->tempoIntro < AVISO_TEMPO) {
        desenhar_aviso("OBJETIVO", "MATE TODOS OS ZUMBIS", j->tempoIntro);
    } else if (portal_aberto(leon) && j->tempoPortal < AVISO_TEMPO) {
        desenhar_aviso("PORTAL ABERTO", "VA ATE O PORTAL E APERTE E", j->tempoPortal);
    }

    if (perto_portal(leon)) {
        Vector2 p = GetWorldToScreen2D((Vector2){PORTAL_X, PORTAL_Y - 120}, *cam);
        const char *txt = "[E] Entrar no portal";

        DrawText(txt, (int)p.x - MeasureText(txt, 20) / 2, (int)p.y, 20, SKYBLUE);
    }

    DrawRectangle(TELA_L - 240, 12, 220 * vida / VIDA_MAX, 18, leon->vida > 30 ? GREEN : RED);
    DrawText(TextFormat("%s | Zumbis: %d", ARMAS[leon->arma].nome, vivos), TELA_L - 240, 36, 18, RAYWHITE);
    DrawText(j->dificil ? "Dificuldade: DIFÍCIL" : "Dificuldade: Normal", TELA_L - 240, 58, 14, j->dificil ? ORANGE : GRAY);
    if (leon->vida <= 0) {
        DrawText("VOCE MORREU - ENTER recomeca", 330, 330, 40, RED);
        DrawText("M - voltar ao menu", 330, 385, 24, RAYWHITE);
    }
}

static void jogo_desenhar(const Jogo *j, const Jogador *leon, const Camera2D *cam, const Recursos *r)
{
    desenhar_luz(leon, cam, r);

    BeginDrawing();
    ClearBackground(BLACK);
    desenhar_mundo(leon, cam, r);
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawTextureRec(r->luzTela.texture, (Rectangle){0, 0, TELA_L, -TELA_A}, (Vector2){0, 0}, WHITE);
    EndBlendMode();
    inimigos_desenhar_vidas(*cam);
    desenhar_hud(j, leon, cam);
    EndDrawing();
}

int main(void)
{
    Recursos rec = {0};
    Jogo jogo = {.estado = MENU, .creditosY = TELA_A};
    Jogador leon = {.pos = {INICIO_X, INICIO_Y}, .vida = VIDA_MAX, .arma = 1, .direita = 1};
    Camera2D cam = {.offset = {TELA_L / 2.0f, TELA_A / 2.0f}, .zoom = ZOOM_CAMERA};

    InitWindow(TELA_L, TELA_A, "Resident Night");
    InitAudioDevice();
    SetTargetFPS(60);
    ChangeDirectory(GetApplicationDirectory());

    if (!mapa_carregar()) {
        TraceLog(LOG_ERROR, "Colisao nao encontrada: %s", MAPA_COLISAO_PATH);
        CloseAudioDevice();
        CloseWindow();
        return 1;
    }
    bloqueado_fn = mapa_bloqueado;
    if (!recursos_carregar(&rec)) {
        CloseAudioDevice();
        CloseWindow();
        return 1;
    }
    inimigos_carregar();
    placar_carregar();

    while (!jogo.sair && !WindowShouldClose()) {
        float dt = GetFrameTime();

        if (rec.temMusica) {
            UpdateMusicStream(rec.musica);
        }
        if (jogo.estado == MENU) {
            menu_atualizar(&jogo, &leon);
            if (jogo.estado == MENU) {
                menu_desenhar(&jogo, &rec);
            }
        } else if (jogo.estado == CREDITOS) {
            creditos_atualizar(&jogo, dt);
            if (jogo.estado == CREDITOS) {
                creditos_desenhar(&jogo);
            }
        } else {
            jogo_atualizar(&jogo, &leon, &cam, &rec, dt);
            jogo_desenhar(&jogo, &leon, &cam, &rec);
        }
    }

    inimigos_liberar();
    inimigos_descarregar();
    placar_liberar();
    mapa_liberar();
    recursos_descarregar(&rec);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
