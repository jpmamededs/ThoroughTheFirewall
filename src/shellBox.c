#include "shellBox.h"
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
static Sound grabbingSound; // Som ao abrir a caixa
static Sound surpriseSound;
static Sound telefoneSound;
static Sound shellPhoneCallSound;
static Sound sinalDesligadoSound;

static float soundTimer = 0.0f;
static bool telefonePlayed = false;
static bool shellPhoneCallPlayed = false;
static bool sinalDesligadoPlayed = false;

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

// Controle da Animação do CD
static bool cdAnimationStarted = false;
static float cdAnimationTimer = 0.0f;
static float cdScale = 0.2f;
static float cdPosX;
static float cdPosY;
static bool cdMovingLeft = false;
static float cdMoveTimer = 0.0f;
static float fadeOutBoxAlpha = 1.0f;

static bool surprisePlayed = false;

void Init_ShellBox(void)
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
    surpriseSound = LoadSound("src/music/surprise.mp3"); // Adicionado
    telefoneSound = LoadSound("src/music/telefone.mp3");
    shellPhoneCallSound = LoadSound("src/music/shellPhoneCall.wav");
    sinalDesligadoSound = LoadSound("src/music/som_telefone_sinal_desligado_ou_ocupado_caio_audio.mp3");

    SetSoundVolume(suspenseSound, 0.5f);

    SetSoundVolume(telefoneSound, 1.0f);
    SetSoundVolume(shellPhoneCallSound, 1.0f);
    SetSoundVolume(sinalDesligadoSound, 1.0f);

    SetSoundVolume(surpriseSound, 1.0f);
    PlaySound(surpriseSound);
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

    cdAnimationStarted = false;
    cdAnimationTimer = 0.0f;
    cdScale = 0.2f;
    cdMovingLeft = false;
    cdMoveTimer = 0.0f;
    fadeOutBoxAlpha = 1.0f;

    surprisePlayed = false;

    fase_concluida = false;
}

void Update_ShellBox(void)
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

    // Efeito de fade in para a caixa fechada e aberta
    if (boxAppeared)
    {
        fadeInAlpha += 0.01f;
        if (fadeInAlpha > 1.0f)
            fadeInAlpha = 1.0f;

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
        if (cardboardPosX > 0)
            cardboardPosX = 0;
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        fase_concluida = true;
    }

    soundTimer += GetFrameTime();

    if (!telefonePlayed && soundTimer >= 1.5f)
    {
        PlaySound(telefoneSound);
        telefonePlayed = true;
    }

    if (!shellPhoneCallPlayed && telefonePlayed && !IsSoundPlaying(telefoneSound))
    {
        PlaySound(shellPhoneCallSound);
        shellPhoneCallPlayed = true;
    }

    if (!sinalDesligadoPlayed && shellPhoneCallPlayed && !IsSoundPlaying(shellPhoneCallSound))
    {
        PlaySound(sinalDesligadoSound);
        sinalDesligadoPlayed = true;
    }
    if (boxAppeared && boxOpened)
    {
        cdAnimationTimer += GetFrameTime();

        // Iniciar a animação após 1.5 segundos
        if (!cdAnimationStarted && cdAnimationTimer >= 1.5f)
        {
            cdAnimationStarted = true;
        }

        // Realizar a animação de aumento e centralização
        if (cdAnimationStarted)
        {
            float targetScale = 1.0f;
            float targetPosX = (GetScreenWidth() - cdSprite.width * targetScale) / 2;
            float targetPosY = (GetScreenHeight() - cdSprite.height * targetScale) / 2;

            // Interpolação suave para aumentar o tamanho e mover para o centro
            cdScale += (targetScale - cdScale) * 0.05f;
            cdPosX += (targetPosX - cdPosX) * 0.05f;
            cdPosY += (targetPosY - cdPosY) * 0.05f;

            // Fade out da caixa durante o aumento do CD
            fadeOutBoxAlpha -= 0.02f;
            if (fadeOutBoxAlpha < 0.0f)
                fadeOutBoxAlpha = 0.0f;

            // Iniciar movimento lateral 2 segundos após o CD centralizar
            cdMoveTimer += GetFrameTime();
            if (cdMoveTimer >= 2.0f && !cdMovingLeft)
            {
                cdMovingLeft = true;
            }

            // Movimentação suave para a esquerda após a centralização
            if (cdMovingLeft)
            {
                float targetPosX = -cdSprite.width * cdScale; // Saindo da tela pela esquerda
                cdPosX += (targetPosX - cdPosX) * 0.05f;
            }
        }
        else
        {
            // Posição inicial do CD (flutuando dentro da caixa)
            float boxScale = 0.5f;
            cdPosX = (GetScreenWidth() - openCardboardBox.width * boxScale) / 2 +
                     (openCardboardBox.width * boxScale / 2) - (cdSprite.width * cdScale / 2);
            cdPosY = (GetScreenHeight() - openCardboardBox.height * boxScale) / 2 + floatOffset + 30.0f -
                     (cdSprite.height * cdScale);
        }
    }
}

void Draw_ShellBox(void)
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
        float posX = (GetScreenWidth() - closedCardboardBox.width * boxScale) / 2;
        float posY = (GetScreenHeight() - closedCardboardBox.height * boxScale) / 2 + floatOffset + 30.0f;

        if (!boxOpened)
            DrawTextureEx(closedCardboardBox, (Vector2){posX, posY}, 0.0f, boxScale, fadeInColor);
        else
        {
            // Caixa aberta com fade out
            Color fadeOutColor = (Color){255, 255, 255, (unsigned char)(fadeOutBoxAlpha * 255)};
            DrawTextureEx(openCardboardBox, (Vector2){posX, posY}, 0.0f, boxScale, fadeOutColor);

            // Desenho do CD animado (flutuando ou centralizado)
            DrawTextureEx(cdSprite, (Vector2){cdPosX, cdPosY}, 0.0f, cdScale, WHITE);
        }
    }

    EndDrawing();
}

bool Fase_ShellBox_Concluida(void)
{
    return fase_concluida;
}

void Unload_ShellBox(void)
{
    StopSound(suspenseSound);
    UnloadSound(suspenseSound);
    UnloadSound(grabbingSound);
    StopSound(surpriseSound);   // Adicionado
    UnloadSound(surpriseSound); // Adicionado
    UnloadSound(telefoneSound);
    UnloadSound(shellPhoneCallSound);
    UnloadSound(sinalDesligadoSound);
    UnloadTexture(cardboardBox);
    UnloadTexture(unknown);
    UnloadTexture(backgroundOutside);
    UnloadTexture(closedCardboardBox);
    UnloadTexture(openCardboardBox);
    UnloadTexture(cdSprite);
}