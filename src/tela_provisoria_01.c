#include "tela_provisoria_01.h"
#include "raylib.h"
#include "generalFunctions.h"
#include <math.h>

// ESSA FASE É A CUTSCENE DA CAIXA

static bool fase_concluida = false;
static Texture2D cardboardBox;
static Texture2D unknown;
static Texture2D backgroundOutside;

// Variáveis de Animação
static float unknownPosX;
static float unknownPosY;
static float cardboardPosX;
static float cardboardPosY;
static float velocityUnknown;
static float velocityCardboard;
static bool slowingDownUnknown = false;

// Variáveis de Flash
static float flashTimer = 0.0f;
static bool flashActive = false;
static float flashAlpha = 1.0f;

void Init_Tela_01(void)
{
    // Carregar as texturas
    cardboardBox = LoadTexture("src/sprites/cardboard_box.png");
    unknown = LoadTexture("src/sprites/unknown.png");
    backgroundOutside = LoadTexture("src/sprites/background_outside.png");

    // Inicializar a posição inicial para que a borda direita do sprite fique fora da tela
    unknownPosX = GetScreenWidth();
    unknownPosY = 100;

    // Inicializar a posição na borda esquerda da tela para cardboardBox
    cardboardPosX = -cardboardBox.width;
    cardboardPosY = 200;

    // Velocidade inicial rápida
    velocityUnknown = 20.0f;
    velocityCardboard = 20.0f;

    // Resetar o timer do flash
    flashTimer = 0.0f;
    flashActive = false;
    flashAlpha = 1.0f;

    fase_concluida = false;
}

void Update_Tela_01(void)
{
    // Atualizar o timer para o flash
    flashTimer += GetFrameTime();

    // Ativar o flash após 3 segundos
    if (flashTimer >= 3.0f && !flashActive)
    {
        flashActive = true;
        flashAlpha = 1.0f;
    }

    // Efeito de fade out do flash
    if (flashActive)
    {
        flashAlpha -= 0.01f;
        if (flashAlpha <= 0.0f)
        {
            flashAlpha = 0.0f;
            flashActive = false; // Flash completo
            fase_concluida = true; // Fase encerrada após o efeito
        }
    }

    // Definir a velocidade mínima garantida
    float minVelocity = 5.0f;

    // Movimento rápido do unknown da direita para a esquerda
    if (!slowingDownUnknown)
    {
        unknownPosX -= velocityUnknown;

        // Verificar se está próximo da posição final para iniciar a desaceleração
        if (unknownPosX <= GetScreenWidth() - (unknown.width * 1.1f) + 50)
        {
            slowingDownUnknown = true;
            velocityUnknown = 8.0f;
        }
    }
    else
    {
        unknownPosX -= velocityUnknown * GetFrameTime();
        if (velocityUnknown < minVelocity)
        {
            velocityUnknown = minVelocity;
        }
    }

    // Movimento rápido do cardboardBox da esquerda para a direita
    float distanceToBordaCardboard = cardboardPosX;
    if (distanceToBordaCardboard < 0)
    {
        velocityCardboard = fmax(minVelocity, 10.0f + (10.0f * (-distanceToBordaCardboard / GetScreenWidth())));
        cardboardPosX += velocityCardboard;

        if (cardboardPosX > 0)
        {
            cardboardPosX = 0;
        }
    }
    else
    {
        cardboardPosX += minVelocity * GetFrameTime();
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        fase_concluida = true;
    }
}

void Draw_Tela_01(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Desenhar o fundo cobrindo toda a tela
    DrawTexturePro(backgroundOutside, 
        (Rectangle){0, 0, (float)backgroundOutside.width, (float)backgroundOutside.height},
        (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0, 0}, 0.0f, WHITE);

    // Desenhar os sprites na tela
    DrawTextureEx(cardboardBox, (Vector2){cardboardPosX, cardboardPosY}, 0.0f, 1.0f, WHITE);
    DrawTextureEx(unknown, (Vector2){unknownPosX, unknownPosY}, 0.0f, 1.1f, WHITE);

    // Desenhar o efeito de flash branco (se ativo)
    if (flashActive || flashTimer >= 3.0f)
    {
        Color flashColor = (Color){255, 255, 255, (unsigned char)(flashAlpha * 255)};
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), flashColor);
    }

    EndDrawing();
}

bool Fase_Tela01_Concluida(void)
{
    return fase_concluida;
}

void Unload_Tela_01(void)
{
    // Descarregar as texturas
    UnloadTexture(cardboardBox);
    UnloadTexture(unknown);
    UnloadTexture(backgroundOutside);
}
