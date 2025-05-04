#include "fase2.h"
#include "raylib.h"
#include "menu.h"
#include "generalFunctions.h"
#include <string.h>
#include <stdio.h>

extern AppState state;

// Recursos da fase 2
static Texture2D fundo;
static Texture2D pergunta_img;

// Sprites dos personagens
static Texture2D sprJoao, sprMateus, sprCarlos, sprMamede;
static TypeWriter fase2Writer;
static bool typeStarted = false;
static bool podeAvancar = false;

// TIMER FASE2
static float fase2Timer = 30.0f;
static bool fase2TimerActive = true;
static bool fase2TimerFinished = false;

// NAVEGAÇÃO DAS OPÇÕES
static int selectedOption = 0; // Agora global para manter seleção nos frames

// Texto exibido
static const char *fase2_fala =
    "Tudo bem, agora que tenho acesso ao computador central o que eu devo fazer primeiro? Essa é uma decisão dificil,\n"
    "talvez eu precise de ajuda...";

void InitFase2(void)
{
    fundo = LoadTexture("src/sprites/empresa3.png");
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");
    // IMPORTANTE: Imagens devem ter fundo transparente!
    sprJoao    = LoadTexture("src/sprites/joaoSprite.png");          // João (a menina)
    sprMateus  = LoadTexture("src/sprites/mateusSprite.png");
    sprCarlos  = LoadTexture("src/sprites/carlosSprite.png");
    sprMamede  = LoadTexture("src/sprites/mamedeSprite.png");
    InitTypeWriter(&fase2Writer, fase2_fala, 18.5f);
    typeStarted = true;
    podeAvancar = false;

    fase2Timer = 30.0f;
    fase2TimerActive = true;
    fase2TimerFinished = false;

    selectedOption = 0;
}

void UpdateFase2(void)
{
    float delta = GetFrameTime();

    // TIMER FASE2
    if (fase2TimerActive && !fase2TimerFinished) {
        fase2Timer -= delta;
        if (fase2Timer <= 0) {
            fase2Timer = 0;
            fase2TimerActive = false;
            fase2TimerFinished = true;
            // Caso queira fazer algo ao acabar o tempo, coloque aqui (ex: state = APP_GAME_OVER;)
        }
    }

    // ==================== NAVEGAÇÃO PELAS OPÇÕES =========================
    int numRects = 5;

    if (IsKeyPressed(KEY_DOWN)) {
        selectedOption++;
        if (selectedOption >= numRects) selectedOption = 0;
    }
    if (IsKeyPressed(KEY_UP)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = numRects-1;
    }
    // =====================================================================

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

    // --- FUNDO
    DrawTextureEx(fundo, (Vector2){0,0}, 0.0f,
        (float)GetScreenWidth() / fundo.width, RAYWHITE);

    // Seleciona sprite conforme personagem
    const char* name = MenuSelectedCharacterName();
    Texture2D spr = sprJoao;
    if(strcmp(name, "Mateus") == 0)
        spr = sprMateus;
    else if(strcmp(name, "João") == 0 || name[0]=='\0')
        spr = sprJoao;
    else if(strcmp(name, "Mamede") == 0)
        spr = sprMamede;
    else if(strcmp(name, "Carlos") == 0)
        spr = sprCarlos;

    // --- Só aumenta a escala do Mateus ---
    float scale = 0.6f;
    if(strcmp(name, "Mateus") == 0)
        scale = 1.3f; // MAIOR SÓ PRA MATEUS!
    else if(strcmp(name, "Carlos") == 0)
        scale = 0.56f;
    else if(strcmp(name, "Mamede") == 0)
        scale = 1.0f;
    float tw2 = spr.width * scale;
    float th = spr.height * scale;

    int boxX = 60;
    int marginBottom = 220;
    int boxY = GetScreenHeight() - marginBottom;
    int boxWidth = GetScreenWidth() - 120;
    int boxHeight = 130;
    int imgW = 1000;
    int imgH = pergunta_img.height - 130;
    int imgX = boxX;
    int imgY = boxY - imgH;
    Vector2 pos;
    pos.x = imgX - 330 + (imgW - tw2)/2.0f;
    pos.y = imgY - th + 210;
    if (strcmp(name, "Carlos") == 0)
        pos.x += 100;
    DrawTextureEx(spr, pos, 0, scale, WHITE);

    // Caixa de fala e imagem da pergunta
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

    // ################# CRONÔMETRO #################
    int timerRadius = 55;
    int timerX = GetScreenWidth() - timerRadius - 25;
    int timerY = 25 + timerRadius;

    DrawCircle(timerX, timerY, timerRadius, (Color){25,25,30, 220});
    DrawCircleSectorLines((Vector2){timerX, timerY}, timerRadius, 0, 360, 32, (Color){180,180,180,220});
    float angle = (fase2Timer / 30.0f) * 360.0f;
    Color barColor = (fase2Timer <= 10.0f) ? (Color){230,30,30,150} : (Color){80, 230, 80, 130};
    DrawCircleSector((Vector2){timerX, timerY}, timerRadius-4, -90, -90+angle, 60, barColor);

    char timerText[8];
    int tempo = (int)fase2Timer;
    snprintf(timerText, sizeof(timerText), "%02d", tempo);
    int fonte = 49;
    Color timeColor = (fase2Timer <= 10.0f) ? RED : WHITE;
    if (fase2Timer <= 5.0f) { fonte = 58; }

    int tw = MeasureText(timerText, fonte);
    DrawText(timerText, timerX - tw/2, timerY - fonte/2, fonte, timeColor);
    DrawText("s", timerX + tw/2 + 2, timerY - fonte/3 + 10, 28, (Color){170,170,210,210});
    // #################################################################################

    // ============ 5 RETÂNGULOS DO LADO DIREITO EM "MEIA PIRÂMIDE" COM HOVER/NAVEGAÇÃO =========
    int numRects = 5;
    int baseWidth = 600;     // Bem compridos!
    int larguraStep = 85;    // Cada camada mais comprida
    int rectHeight = 78;     // Altos!
    int spacing = 28;
    int offsetY = 295;
    int offsetX = 35;

    Color rectGray      = (Color){56, 56, 56, 216};
    Color hoverGreen    = (Color){ 26, 110, 51, 235 };   // verde escuro (um pouco mais vivo para seleção)
    Color mouseGreen    = (Color){14, 77, 33, 235};      // verde escuro padrão hover mouse

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < numRects; i++)
    {
        int rectWidth = baseWidth + i * larguraStep;
        int y = offsetY + i * (rectHeight + spacing);
        int x = GetScreenWidth() - rectWidth - offsetX;

        Rectangle rec = {x, y, rectWidth, rectHeight};

        Color cor = rectGray;

        // Mouse hover sobrescreve a seleção de teclado!
        if (CheckCollisionPointRec(mouse, rec)) {
            cor = mouseGreen;
            selectedOption = i;
        } else if (selectedOption == i) {
            cor = hoverGreen;
        }

        DrawRectangleRounded(rec, 0.32f, 16, cor);

        // Texto dentro do retângulo centralizado
        char texto[32];
        snprintf(texto, sizeof(texto), "Opção %d", i+1);
        int txtWidth = MeasureText(texto, 32);
        DrawText(
            texto,
            x + rectWidth/2 - txtWidth/2,
            y + rectHeight/2 - 32/2,
            32, WHITE
        );
    }
    // ===========================================================================================

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