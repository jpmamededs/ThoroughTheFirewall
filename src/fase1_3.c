#include "fase1_3.h"
#include "raylib.h"
#include "generalFunctions.h"

extern AppState state;

static Texture2D background;
static Music interrogationMusic;

static Texture2D spriteNome;
static Texture2D spriteBustup;
static Sound somSurpresa;

static Vector2 posNome;
static Vector2 posBustup;

static Vector2 targetNome;
static Vector2 targetBustup;

static float speed = 2500.0f;
static float slowSpeed = 10.0f;

static bool animacaoConcluida = false;
static bool somTocado = false;
static bool spritesVisiveis = true;
static bool bustupChegou = false;

static float tempoAposAnimacao = 0.0f;
static float fadeAlpha = 0.0f;

void InitFase1_3(void)
{
    background = LoadTexture("src/sprites/background_outside.png");
    interrogationMusic = LoadMusicStream("src/music/interrogationThemeA.mp3");
    SetMusicVolume(interrogationMusic, 1.0f);
    PlayMusicStream(interrogationMusic);

    spriteNome = LoadTexture("src/sprites/detetive-hank-text.png");
    spriteBustup = LoadTexture("src/sprites/Gumshoe_OA (1).png");
    somSurpresa = LoadSound("src/music/surprise.mp3");

    posNome = (Vector2){(float)GetScreenWidth(), GetScreenHeight() - spriteNome.height - 50};
    posBustup = (Vector2){-spriteBustup.width, (float)GetScreenHeight() - spriteBustup.height};

    targetNome = (Vector2){GetScreenWidth() - spriteNome.width, posNome.y};
    targetBustup = (Vector2){0.0f, posBustup.y};

    animacaoConcluida = false;
    somTocado = false;
    spritesVisiveis = true;
    bustupChegou = false;
    tempoAposAnimacao = 0.0f;
    fadeAlpha = 0.0f;
}

void UpdateFase1_3(void)
{
    UpdateMusicStream(interrogationMusic);
    float delta = GetFrameTime();

    if (!animacaoConcluida)
    {
        if (!somTocado)
        {
            PlaySound(somSurpresa);
            somTocado = true;
        }

        // Movimento rápido dos sprites
        if (posNome.x > targetNome.x)
        {
            posNome.x -= speed * delta;
            if (posNome.x < targetNome.x)
                posNome.x = targetNome.x;
        }

        if (!bustupChegou)
        {
            posBustup.x += speed * delta;
            if (posBustup.x >= targetBustup.x)
            {
                posBustup.x = targetBustup.x;
                bustupChegou = true;
            }
        }

        if (posNome.x == targetNome.x && bustupChegou)
        {
            animacaoConcluida = true;
        }
    }
    else if (spritesVisiveis)
    {
        // Movimento lento contínuo do bustup
        posBustup.x += slowSpeed * delta;

        tempoAposAnimacao += delta;
        if (tempoAposAnimacao >= 2.0f)
        {
            fadeAlpha += delta * 1.5f;
            if (fadeAlpha >= 1.0f)
            {
                fadeAlpha = 1.0f;
                spritesVisiveis = false;
            }
        }
    }
}

void DrawFase1_3(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    Rectangle source = {0, 0, (float)background.width, (float)background.height};
    Rectangle dest = {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};
    DrawTexturePro(background, source, dest, (Vector2){0, 0}, 0.0f, WHITE);

    if (spritesVisiveis)
    {
        DrawTexture(spriteNome, posNome.x, posNome.y, WHITE);
        DrawTexture(spriteBustup, posBustup.x, posBustup.y, WHITE);
    }

    if (fadeAlpha > 0.0f)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){255, 255, 255, (unsigned char)(fadeAlpha * 255)});
    }

    EndDrawing();
}

void UnloadFase1_3(void)
{
    UnloadTexture(background);
    UnloadMusicStream(interrogationMusic);
    UnloadTexture(spriteNome);
    UnloadTexture(spriteBustup);
    UnloadSound(somSurpresa);
}
