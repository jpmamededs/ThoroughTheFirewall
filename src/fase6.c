#include "fase6.h"
#include "generalFunctions.h"
#include "menu.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

// SPRITES E TEXTURAS
static Texture2D fundo;
static Texture2D pergunta_img;
static Texture2D sprJoao, sprJoao2, sprJoao3;
static Texture2D sprMateus, sprMateus2, sprMateus3;
static Texture2D sprCarlos, sprCarlos2, sprCarlos3;
static Texture2D sprMamede, sprMamede2, sprMamede3;
static Texture2D sprEnterButton;
static Texture2D sprGemini;

// Gemini box
static const char* gemini_help_msg_default = "Clique aqui caso precise de ajuda!";
static const char* gemini_help_msg_ajuda = "Preste atenção nos detalhes. Só avance quando tiver certeza!";
static float geminiRectW = 550;
static float geminiRectH = 0;
static int   geminiTextWidth = 0;
static float geminiRectAnim = 0.0f;
static bool geminiMouseOver = false;
static bool geminiHelpClicked = false;
static float geminiAnimSpeed = 6.0f;
#define GEMINI_RECT_PADRAO 550
#define GEMINI_PAD_X 36

static void AtualizaTamanhoGeminiBox(void)
{
    float geminiScale = 0.1f;
    float geminiH = sprGemini.height * geminiScale;
    int txtSize = 20;
    const char* gemini_msg = !geminiHelpClicked ? gemini_help_msg_default : gemini_help_msg_ajuda;
    geminiTextWidth = MeasureText(gemini_msg, txtSize);
    float minW = GEMINI_RECT_PADRAO;
    float maxW = GetScreenWidth() - 140;
    geminiRectW = minW;
    if (geminiTextWidth + 2 * GEMINI_PAD_X > minW)
        geminiRectW = (geminiTextWidth + 2 * GEMINI_PAD_X > maxW) ? maxW : geminiTextWidth + 2 * GEMINI_PAD_X;
    geminiRectH = geminiH * 0.75f;
}

// FALAS DAS FASES
#define FALA_NORMAL "O que devo fazer agora? Qualquer passo em falso pode comprometer tudo."
#define FALA_JOAO_ACERTO "Boa... agi na medida certa. Ninguém percebeu."
#define FALA_CARLOS_ACERTO "Hahaha, ninguém notou nem um movimento suspeito."
#define FALA_MAMEDE_ACERTO "Controle total. Plano segue perfeito!"
#define FALA_MATEUS_ACERTO "Pronto, tudo como planejado. Agora é seguir em frente."

static TypeWriter writer;
static char fala_exibida[512];
static bool podeAvancar = false;
static bool fadeout = false;
static float fadeout_time = 0.0f;
#define FADEOUT_DURACAO 0.8f
#define ESPERA_ANTES_FADE 1.5f

// fala por personagem (apenas fala de acerto conforme padrão de transição)
static const char* FalaPorResultadoFase6(const char* name)
{
    if (!name || !name[0]) name = "Mateus";
    if (strcmp(name, "João") == 0)    return FALA_JOAO_ACERTO;
    if (strcmp(name, "Carlos") == 0)  return FALA_CARLOS_ACERTO;
    if (strcmp(name, "Mamede") == 0)  return FALA_MAMEDE_ACERTO;
    return FALA_MATEUS_ACERTO;
}


void InitFase6(void)
{
    fundo = LoadTexture("src/sprites/password3.png"); // Idêntico ao fase3, mude o arquivo se desejar
    pergunta_img = LoadTexture("src/sprites/pergunta3.png"); // Idêntico, troque por pergunta6 se quiser
    sprJoao    = LoadTexture("src/sprites/joaoSprite.png");
    sprJoao2   = LoadTexture("src/sprites/joao2.png");
    sprJoao3   = LoadTexture("src/sprites/joao3.png");
    sprMateus  = LoadTexture("src/sprites/mateusSprite.png");
    sprMateus2 = LoadTexture("src/sprites/mateus2.png");
    sprMateus3 = LoadTexture("src/sprites/mateus3.png");
    sprCarlos  = LoadTexture("src/sprites/carlosSprite.png");
    sprCarlos2 = LoadTexture("src/sprites/carlos2.png");
    sprCarlos3 = LoadTexture("src/sprites/carlos3.png");
    sprMamede  = LoadTexture("src/sprites/mamedeSprite.png");
    sprMamede2 = LoadTexture("src/sprites/mamede2.png");
    sprMamede3 = LoadTexture("src/sprites/mamede3.png");
    sprEnterButton  = LoadTexture("src/sprites/enter_button.png");
    sprGemini  = LoadTexture("src/sprites/os/gemini.png");

    strcpy(fala_exibida, FALA_NORMAL);
    InitTypeWriter(&writer, fala_exibida, 18.5f);
    podeAvancar = false;
    fadeout = false;
    fadeout_time = 0.0f;
    geminiHelpClicked = false;
    geminiRectAnim = 0.0f;
    geminiMouseOver = false;
    AtualizaTamanhoGeminiBox();
}

void UpdateFase6(void)
{
    float delta = GetFrameTime();

    // Gemini logic igual ao fase3
    float geminiX = 49, geminiY = 67, geminiScale = 0.1f;
    float geminiW = sprGemini.width * geminiScale;
    float geminiH = sprGemini.height * geminiScale;
    AtualizaTamanhoGeminiBox();
    float rx = geminiX + geminiW - 20.0f;
    float ry = geminiY + (geminiH - geminiRectH) / 2.0f;
    Rectangle geminiLogoRec = { geminiX, geminiY, geminiW, geminiH };
    Rectangle geminiRectRec = { rx, ry, geminiRectW, geminiRectH };
    Vector2 mouseGem = GetMousePosition();
    bool mouseOverLogo = CheckCollisionPointRec(mouseGem, geminiLogoRec);
    bool mouseOverRect = CheckCollisionPointRec(mouseGem, geminiRectRec);
    geminiMouseOver = mouseOverLogo || mouseOverRect;
    float dir = geminiMouseOver ? 1.0f : -1.0f;
    geminiRectAnim += dir * geminiAnimSpeed * delta;
    if (geminiRectAnim > 1.0f) geminiRectAnim = 1.0f;
    if (geminiRectAnim < 0.0f) geminiRectAnim = 0.0f;

    if ((mouseOverLogo || mouseOverRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!geminiHelpClicked) {
            geminiHelpClicked = true;
            AtualizaTamanhoGeminiBox();
            geminiRectAnim = 1.0f;
        }
    }

    UpdateTypeWriter(&writer, delta, IsKeyPressed(KEY_SPACE));

    // Após o TypeWriter acabar, e uma breve pausa, já permite avançar
    static float timerFalaFinal = 0.0f;
    if (writer.done) timerFalaFinal += delta;
    else timerFalaFinal = 0.0f;
    podeAvancar = (writer.done && timerFalaFinal > ESPERA_ANTES_FADE);

    // Avançar por ENTER ou clique (igual ao botão de continuar no fase3, mas aqui ENTER/click global)
    if (podeAvancar && !fadeout) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            fadeout = true;
            fadeout_time = 0.0f;
        }
    }
    if (fadeout) {
        fadeout_time += delta;
    }
}

void DrawFase6(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Novo método para desenhar o fundo: cobre toda a tela SEM faixas!
    {
        float screenRatio = (float)GetScreenWidth() / (float)GetScreenHeight();
        float imgRatio = (float)fundo.width / (float)fundo.height;

        float scale = (screenRatio > imgRatio)
            ? (float)GetScreenWidth() / fundo.width
            : (float)GetScreenHeight() / fundo.height;

        float drawW = fundo.width * scale;
        float drawH = fundo.height * scale;
        float drawX = (GetScreenWidth() - drawW) / 2;
        float drawY = (GetScreenHeight() - drawH) / 2;

        DrawTextureEx(fundo, (Vector2){drawX, drawY}, 0.0f, scale, WHITE);
    }

    // Resto do seu código, INALTERADO...

    // Alinhamento da zona central/padrão conforme fase3
    int boxX = 60;
    int marginBottom = 220;
    int boxY = GetScreenHeight() - marginBottom;
    int boxWidth = GetScreenWidth() - 120;
    int boxHeight = 130;
    int imgW = 1000;
    int imgH = pergunta_img.height - 130;
    int imgX = boxX;
    int imgY = boxY - imgH;

    // Pergunta visual mesma posição que fase3
    DrawTexturePro(pergunta_img, (Rectangle){0, 0, pergunta_img.width, pergunta_img.height},
                   (Rectangle){imgX, imgY, imgW, imgH}, (Vector2){0, 0}, 0.0f, WHITE);

    const char *name = MenuSelectedCharacterName();
    DrawText((name && name[0]) ? name : "???", imgX + 10, imgY + imgH - 30, 30, WHITE);

    // Caixa de fala idêntica
    int borderRadius = boxHeight / 2;
    DrawRectangle(boxX, boxY, boxWidth - borderRadius, boxHeight, (Color){20, 20, 20, 220});
    DrawCircle(boxX + boxWidth - borderRadius, boxY + borderRadius, borderRadius, (Color){20, 20, 20, 220});
    if (writer.drawnChars > 0) {
        char txtbuf[512];
        strncpy(txtbuf, fala_exibida, writer.drawnChars);
        txtbuf[writer.drawnChars] = '\0';
        DrawText(txtbuf, boxX + 20, boxY + 30, 28, WHITE);
    }

    // Personagem igual ao fase3
    Texture2D spr = sprJoao;
    float scale = 0.6f;
    int carlosExtraOffset = 0, mamedeExtraOffset = 0;
    if(strcmp(name, "Mateus") == 0) {
        spr = sprMateus;   scale = 1.3f;
    } else if(strcmp(name, "João") == 0) {
        spr = sprJoao;     scale = 0.6f;
    } else if(strcmp(name, "Carlos") == 0) {
        spr = sprCarlos;   scale = 0.56f; carlosExtraOffset = 0;
    } else if(strcmp(name, "Mamede") == 0) {
        spr = sprMamede;   scale = 1.0f;
    } else if(name[0]=='\0') {
        spr = sprJoao;     scale = 0.6f;
    }
    float tw2 = spr.width * scale;
    float th  = spr.height * scale;
    Vector2 pos;
    pos.x = imgX - 330 + (imgW - tw2)/2.0f;
    pos.y = imgY - th + 210;
    if (strcmp(name, "Carlos") == 0)
        pos.x += 100 + carlosExtraOffset;
    else if (strcmp(name, "Mamede") == 0)
        pos.x += mamedeExtraOffset;
    DrawTextureEx(spr, pos, 0, scale, WHITE);

    // Gemini help, igual ao fase3
    {
        float geminiX = 49, geminiY = 67, geminiScale = 0.1f;
        float geminiW = sprGemini.width * geminiScale;
        float geminiH = sprGemini.height * geminiScale;
        const char* gemini_msg = !geminiHelpClicked ? gemini_help_msg_default : gemini_help_msg_ajuda;
        int txtSize = 20;
        float rx = geminiX + geminiW - 20.0f;
        float ry = geminiY + (geminiH - geminiRectH)/2.0f;
        float animW = geminiRectW * geminiRectAnim;
        Color logoCol = geminiMouseOver ? (Color){ 18, 60, 32, 255 } : (Color){ 26, 110, 51, 255 };
        if (geminiRectAnim > 0.01f) {
            Color rectCol = (Color){ 15, 42, 26, (unsigned char)( 210 * geminiRectAnim ) };
            float round = 0.33f;
            DrawRectangleRounded((Rectangle){ rx, ry, animW, geminiRectH }, round, 12, rectCol);
            if (geminiRectAnim > 0.05f) {
                if (animW > geminiTextWidth + 2*GEMINI_PAD_X - 4) {
                    DrawText(gemini_msg, rx + GEMINI_PAD_X, ry + geminiRectH/2 - txtSize/2, txtSize, WHITE);
                }
            }
        }
        DrawTextureEx(sprGemini, (Vector2){geminiX, geminiY}, 0.0f, geminiScale, logoCol);
    }

    // Botão de continuar (ENTER) após typewriter acabar: visual animado
    if (podeAvancar) {
        float pulse = 0.07f * sinf(GetTime() * 5.0f);
        float btnScaleBase = 0.95f;
        float btnScale = btnScaleBase + pulse;
        float btnW = sprEnterButton.width * btnScale;
        float btnH = sprEnterButton.height * btnScale;
        float btnX = GetScreenWidth()/2 - btnW/2 + 120;
        float btnY = GetScreenHeight()/2 - 150;
        Color sombra = (Color){0, 0, 0, 90};
        DrawRectangleRounded((Rectangle){btnX + 8, btnY + 8, btnW, btnH}, 0.25f, 10, sombra);
        Color brilho = (pulse > 0.04f) ? (Color){255,255,255,75} : WHITE;
        DrawTextureEx(sprEnterButton, (Vector2){btnX, btnY}, 0.0f, btnScale, brilho);
    }
    if(fadeout) {
        float perc = fadeout_time / FADEOUT_DURACAO;
        if (perc > 1.0f) perc = 1.0f;
        int alpha = (int)(255 * perc);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0,0,0, alpha});
    }
    EndDrawing();
}
void UnloadFase6(void)
{
    UnloadTexture(fundo);
    UnloadTexture(pergunta_img);
    UnloadTexture(sprJoao);    UnloadTexture(sprJoao2);    UnloadTexture(sprJoao3);
    UnloadTexture(sprMateus);  UnloadTexture(sprMateus2);  UnloadTexture(sprMateus3);
    UnloadTexture(sprCarlos);  UnloadTexture(sprCarlos2);  UnloadTexture(sprCarlos3);
    UnloadTexture(sprMamede);  UnloadTexture(sprMamede2);  UnloadTexture(sprMamede3);
    UnloadTexture(sprEnterButton);
    UnloadTexture(sprGemini);
}