#include "fase2.h"
#include "raylib.h"
#include "menu.h"
#include "generalFunctions.h"
#include <string.h>
extern AppState state;

// Recursos da fase 2
static Texture2D fundo;
static Texture2D pergunta_img;
// Sprites dos personagens
static Texture2D sprJoao, sprMateus, sprCarlos, sprMamede;

static TypeWriter fase2Writer;
static bool typeStarted = false;
static bool podeAvancar = false;

// Texto exibido
static const char *fase2_fala =
    "Tudo bem, agora que tenho acesso ao computador central o que eu devo fazer primeiro? Essa é uma decisão dificil,\n"
    "talvez eu precise de ajuda...";

void InitFase2(void)
{
    fundo = LoadTexture("src/sprites/empresa3.png");
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");

    // IMPORTANTE: Imagens devem ter fundo transparente!
    sprJoao    = LoadTexture("src/sprites/menina.png");          // João (a menina)
    sprMateus  = LoadTexture("src/sprites/hacker2.png");
    sprCarlos  = LoadTexture("src/sprites/deBone.png");
    sprMamede  = LoadTexture("src/sprites/meninoPdavida.png");

    InitTypeWriter(&fase2Writer, fase2_fala, 18.5f);
    typeStarted = true;
    podeAvancar = false;
}

void UpdateFase2(void)
{
    float delta = GetFrameTime();
    if (typeStarted) {
        UpdateTypeWriter(&fase2Writer, delta, IsKeyPressed(KEY_SPACE));
        if (fase2Writer.drawnChars == (int)strlen(fase2_fala))
            podeAvancar = true;
    }
    if (podeAvancar) {
        Rectangle btnBounds = {
            GetScreenWidth() / 2 - 100,
            GetScreenHeight() / 2 + 100,
            200,
            50};
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, btnBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = APP_PC_SCREEN;
        }
    }
}

void DrawFase2(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Fundo
    DrawTextureEx(fundo, (Vector2){0,0}, 0.0f,
        (float)GetScreenWidth() / fundo.width, RAYWHITE);

    // ==== Desenhar personagem (apenas a silhueta, assumindo PNG transparente) ====
    // Seleciona textura de acordo com o personagem
    const char* name = MenuSelectedCharacterName();
    Texture2D spr = sprJoao; // padrão João
    if(strcmp(name, "Mateus") == 0)
        spr = sprMateus;
    else if(strcmp(name, "João") == 0 || name[0]=='\0')
        spr = sprJoao;
    else if(strcmp(name, "Mamede") == 0)
        spr = sprMamede;
    else if(strcmp(name, "Carlos") == 0)
        spr = sprCarlos;
    else
        spr = sprJoao;

    // Centralizado, com escala
    float scale = 0.6f; // Use a que preferir!
    float tw = spr.width * scale;
    float th = spr.height * scale;
    // Centralize (ajuste -60 se quiser ela mais ou menos acima)
    Vector2 pos = { (GetScreenWidth() - tw) / 2.0f, (GetScreenHeight() - th) / 2.0f - 60 };
    DrawTextureEx(spr, pos, 0, scale, WHITE);
    // ======================

    // Caixa de fala
    int boxX = 60;
    int marginBottom = 220;
    int boxY = GetScreenHeight() - marginBottom;
    int boxWidth = GetScreenWidth() - 120;
    int boxHeight = 130;
    int imgW = 1000;
    int imgH = pergunta_img.height - 130;
    int imgX = boxX;
    int imgY = boxY - imgH;
    DrawTexturePro(pergunta_img, (Rectangle){0, 0, pergunta_img.width, pergunta_img.height},
                   (Rectangle){imgX, imgY, imgW, imgH}, (Vector2){0, 0}, 0.0f, WHITE);
    DrawText("???", imgX + 10, imgY + imgH - 30, 30, WHITE);
    int borderRadius = boxHeight / 2;
    DrawRectangle(boxX, boxY, boxWidth - borderRadius, boxHeight, (Color){20, 20, 20, 220});
    DrawCircle(boxX + boxWidth - borderRadius, boxY + borderRadius, borderRadius, (Color){20, 20, 20, 220});

    if (fase2Writer.drawnChars > 0) {
        char tmp[fase2Writer.drawnChars + 1];
        strncpy(tmp, fase2_fala, fase2Writer.drawnChars);
        tmp[fase2Writer.drawnChars] = '\0';
        DrawText(tmp, boxX + 20, boxY + 30, 28, WHITE);
    }

    EndDrawing();
}

void UnloadFase2(void)
{
    UnloadTexture(fundo);
    UnloadTexture(pergunta_img);
    UnloadTexture(sprJoao);
    UnloadTexture(sprMateus);
    UnloadTexture(sprCarlos);
    UnloadTexture(sprMamede);
}