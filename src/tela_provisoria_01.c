#include "tela_provisoria_01.h"
#include "raylib.h"
#include "generalFunctions.h"
#include <math.h>

// ESSA FASE É A CUTSCENE DA CAIXA

static bool fase_concluida = false;
static Texture2D cardboardBox;
static Texture2D unknown;
static Texture2D backgroundOutside;
static Texture2D closedCardboardBox; // Caixa fechada para o final
static Texture2D openCardboardBox;   // Caixa aberta para a troca
static Texture2D cdSprite;           // Sprite do CD
static Sound suspenseSound;
static Sound grabbingSound;          // Som ao abrir a caixa
static Sound surpriseSound;

// Variáveis de Animação
static float unknownPosX;
static float unknownPosY;
static float cardboardPosX;
static float cardboardPosY;
static float velocityUnknown;
static float velocityCardboard;
static bool slowingDownUnknown = false;

// Variáveis de Fade Out e Fade In
static float fadeTimer = 0.0f;
static bool fadeOutActive = false;
static float fadeAlpha = 1.0f;
static float fadeInAlpha = 0.0f;
static bool boxAppeared = false;

// Variáveis de Flutuação
static float floatOffset = 0.0f;
static float floatSpeed = 1.5f;

// Controle da Troca da Caixa
static bool boxOpened = false;
static float boxTimer = 0.0f;

static bool surprisePlayed = false;

void Init_Tela_01(void)
{
    // Carregar as texturas
    cardboardBox = LoadTexture("src/sprites/cardboard_box.png");
    unknown = LoadTexture("src/sprites/unknown.png");
    backgroundOutside = LoadTexture("src/sprites/background_outside.png");
    closedCardboardBox = LoadTexture("src/sprites/closed_cardboardBox.png");
    openCardboardBox = LoadTexture("src/sprites/open_cardboardBox.png");
    cdSprite = LoadTexture("src/sprites/cd.png");

    // Carregar os sons
    suspenseSound = LoadSound("src/music/suspenseBox.mp3");
    grabbingSound = LoadSound("src/music/grabbing.mp3");
    surpriseSound = LoadSound("src/music/surprise.mp3");  // Adicionado

    SetSoundVolume(surpriseSound, 1.0f);

    PlaySound(suspenseSound);

    // Inicializar a posição inicial dos sprites
    unknownPosX = GetScreenWidth();
    unknownPosY = 100;
    cardboardPosX = -cardboardBox.width;
    cardboardPosY = 200;

    // Velocidade inicial rápida
    velocityUnknown = 20.0f;
    velocityCardboard = 20.0f;

    // Resetar o timer do fade
    fadeTimer = 0.0f;
    fadeOutActive = false;
    fadeAlpha = 1.0f;
    fadeInAlpha = 0.0f;

    // Inicializar controle da caixa
    boxOpened = false;
    boxTimer = 0.0f;

    surprisePlayed = false;

    fase_concluida = false;
}

void Update_Tela_01(void)
{
    // Atualizar o timer para o fade out
    fadeTimer += GetFrameTime();

    // Ativar o fade out após 5 segundos
    if (fadeTimer >= 5.0f && !fadeOutActive)
    {
        fadeOutActive = true;
        fadeAlpha = 1.0f;
    }

    // Efeito de fade out sincronizado
    if (fadeOutActive)
    {
        fadeAlpha -= 0.01f;
        if (fadeAlpha <= 0.0f)
        {
            fadeAlpha = 0.0f;
            boxAppeared = true;
        }
    }

    if (boxAppeared && !surprisePlayed)
    {
        PlaySound(surpriseSound);
        surprisePlayed = true;
    }

    // Efeito de fade in para a caixa fechada e aberta
    if (boxAppeared)
    {
        fadeInAlpha += 0.01f;
        if (fadeInAlpha > 1.0f) fadeInAlpha = 1.0f;

        // Movimentação de flutuação
        floatOffset = sin(GetTime() * floatSpeed) * 5.0f;

        // Atualizar o timer para a troca de caixa
        boxTimer += GetFrameTime();
        if (boxTimer >= 4.0f && !boxOpened)
        {
            boxOpened = true;
            PlaySound(grabbingSound); // Tocar o som ao abrir a caixa
        }
    }

    // Movimento do personagem desconhecido
    if (!slowingDownUnknown)
    {
        unknownPosX -= velocityUnknown;
        if (unknownPosX <= GetScreenWidth() - (unknown.width * 1.1f) + 50)
        {
            slowingDownUnknown = true;
            velocityUnknown = 8.0f;
        }
    }
    else
    {
        unknownPosX -= velocityUnknown * GetFrameTime();
        if (velocityUnknown < 5.0f)
        {
            velocityUnknown = 5.0f;
        }
    }

    // Movimento da caixa de papelão
    float distanceToBordaCardboard = cardboardPosX;
    if (distanceToBordaCardboard < 0)
    {
        velocityCardboard = fmax(5.0f, 10.0f + (10.0f * (-distanceToBordaCardboard / GetScreenWidth())));
        cardboardPosX += velocityCardboard;
        if (cardboardPosX > 0) cardboardPosX = 0;
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        fase_concluida = true;
    }
}

void Draw_Tela_01(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    Color fadeColor = (Color){255, 255, 255, (unsigned char)(fadeAlpha * 255)};
    DrawTexturePro(backgroundOutside, 
        (Rectangle){0, 0, (float)backgroundOutside.width, (float)backgroundOutside.height},
        (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0, 0}, 0.0f, fadeColor);

    DrawTextureEx(cardboardBox, (Vector2){cardboardPosX, cardboardPosY}, 0.0f, 1.0f, fadeColor);
    DrawTextureEx(unknown, (Vector2){unknownPosX, unknownPosY}, 0.0f, 1.1f, fadeColor);

    if (boxAppeared)
    {
        Color fadeInColor = (Color){255, 255, 255, (unsigned char)(fadeInAlpha * 255)};
        float boxScale = 0.5f;
        float cdScale = 0.2f;

        float posX = (GetScreenWidth() - closedCardboardBox.width * boxScale) / 2;
        float posY = (GetScreenHeight() - closedCardboardBox.height * boxScale) / 2 + floatOffset + 30.0f;

        if (!boxOpened)
            DrawTextureEx(closedCardboardBox, (Vector2){posX, posY}, 0.0f, boxScale, fadeInColor);
        else
        {
            DrawTextureEx(openCardboardBox, (Vector2){posX, posY}, 0.0f, boxScale, fadeInColor);
            float cdPosX = posX + (openCardboardBox.width * boxScale / 2) - (cdSprite.width * cdScale / 2);
            float cdPosY = posY - (cdSprite.height * cdScale) + (1.0f - fadeInAlpha) * 50.0f;
            DrawTextureEx(cdSprite, (Vector2){cdPosX, cdPosY}, 0.0f, cdScale, fadeInColor);
        }
    }

    EndDrawing();
}

bool Fase_Tela01_Concluida(void)
{
    return fase_concluida;
}

void Unload_Tela_01(void)
{
    StopSound(suspenseSound);
    UnloadSound(suspenseSound);
    UnloadSound(grabbingSound);
    StopSound(surpriseSound);  // Adicionado
    UnloadSound(surpriseSound);  // Adicionado
    UnloadTexture(cardboardBox);
    UnloadTexture(unknown);
    UnloadTexture(backgroundOutside);
    UnloadTexture(closedCardboardBox);
    UnloadTexture(openCardboardBox);
    UnloadTexture(cdSprite);
}
