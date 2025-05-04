#include "fase1_3.h"
#include "raylib.h"
#include "generalFunctions.h"
#include <string.h>

extern AppState state;

static Texture2D background;
static Music interrogationMusic;

static Texture2D spriteNome;
static Texture2D spriteBustup;
static Texture2D spriteConfiante;
static Sound somSurpresa;
static Sound somFalaDetetive;

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

static float fadeWhiteAlpha = 0.0f;
static bool fadeWhiteIn = false;
static bool fadeWhiteOut = false;

static bool mostrarConfiante = false;
static bool somFalaTocado = false;

static TypeWriter writer;
static const char* dialogoTexto =
    "Então, basicamente, você e... mais 3 pessoas estão sendo investigadas por crimes cibernéticos... "
    "Eu sou o investigador desse caso, e vou te fazer algumas perguntas, ok?";

void InitFase1_3(void)
{
    background = LoadTexture("src/sprites/background_outside.png");
    interrogationMusic = LoadMusicStream("src/music/interrogationThemeA.mp3");
    SetMusicVolume(interrogationMusic, 1.0f);
    PlayMusicStream(interrogationMusic);

    spriteNome = LoadTexture("src/sprites/detetive-hank-text.png");
    spriteBustup = LoadTexture("src/sprites/Gumshoe_OA (1).png");
    spriteConfiante = LoadTexture("src/sprites/detective_confident.png");

    somSurpresa = LoadSound("src/music/surprise.mp3");
    somFalaDetetive = LoadSound("src/music/detectiveSpeaking.mp3");

    posNome = (Vector2){(float)GetScreenWidth(), GetScreenHeight() - spriteNome.height - 50};
    posBustup = (Vector2){-spriteBustup.width, (float)GetScreenHeight() - spriteBustup.height};

    targetNome = (Vector2){GetScreenWidth() - spriteNome.width, posNome.y};
    targetBustup = (Vector2){0.0f, posBustup.y};

    animacaoConcluida = false;
    somTocado = false;
    spritesVisiveis = true;
    bustupChegou = false;
    tempoAposAnimacao = 0.0f;

    fadeWhiteAlpha = 0.0f;
    fadeWhiteIn = false;
    fadeWhiteOut = false;

    mostrarConfiante = false;
    somFalaTocado = false;

    InitTypeWriter(&writer, dialogoTexto, strlen(dialogoTexto) / 10.0f); // 10 segundos
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

        if (posNome.x > targetNome.x)
        {
            posNome.x -= speed * delta;
            if (posNome.x < targetNome.x)
                posNome.x = targetNome.x;
        }

        // Movimento de entrada rápida do bustup
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
        // Após entrada, movimento lento do bustup
        posBustup.x += slowSpeed * delta;

        tempoAposAnimacao += delta;

        if (tempoAposAnimacao >= 2.0f && !fadeWhiteIn && !fadeWhiteOut)
        {
            fadeWhiteIn = true;
        }

        if (fadeWhiteIn)
        {
            fadeWhiteAlpha += delta * 1.5f;
            if (fadeWhiteAlpha >= 1.0f)
            {
                fadeWhiteAlpha = 1.0f;
                fadeWhiteIn = false;
                fadeWhiteOut = true;
                spritesVisiveis = false;
                mostrarConfiante = true;
            }
        }
    }

    if (fadeWhiteOut)
    {
        fadeWhiteAlpha -= delta * 1.5f;
        if (fadeWhiteAlpha <= 0.0f)
        {
            fadeWhiteAlpha = 0.0f;
            fadeWhiteOut = false;
        }
    }

    if (mostrarConfiante)
    {
        if (!somFalaTocado)
        {
            PlaySound(somFalaDetetive);
            somFalaTocado = true;
        }

        UpdateTypeWriter(&writer, delta, IsKeyPressed(KEY_ENTER)); // ENTER = pular
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

    if (mostrarConfiante)
    {
        float scale = 1.5f;
        float finalWidth = spriteConfiante.width * scale;
        float finalHeight = spriteConfiante.height * scale;

        Vector2 pos = {
            (GetScreenWidth() - finalWidth) / 2.0f,
            GetScreenHeight() - finalHeight};

        Rectangle src = {0, 0, (float)spriteConfiante.width, (float)spriteConfiante.height};
        Rectangle dst = {pos.x, pos.y, finalWidth, finalHeight};
        DrawTexturePro(spriteConfiante, src, dst, (Vector2){0, 0}, 0.0f, WHITE);

        float dialogHeight = 120.0f;
        DrawRectangle(50, GetScreenHeight() - dialogHeight - 30,
                      GetScreenWidth() - 100, dialogHeight, (Color){0, 0, 0, 200});
        DrawRectangleLines(50, GetScreenHeight() - dialogHeight - 30,
                           GetScreenWidth() - 100, dialogHeight, WHITE);

        char buffer[512] = {0};
        strncpy(buffer, writer.text, writer.drawnChars);

        int fontSize = 24;
        int maxWidth = GetScreenWidth() - 140;
        int x = 70;
        int y = GetScreenHeight() - dialogHeight - 10;

        const char *lineStart = buffer;
        int lineHeight = fontSize + 6;

        while (*lineStart)
        {
            const char *lineEnd = lineStart;
            int lineWidth = 0;
            int lastSpace = -1;
            int charCount = 0;

            while (*lineEnd && lineWidth < maxWidth)
            {
                if (*lineEnd == ' ')
                    lastSpace = charCount;

                lineEnd++;
                charCount++;

                char temp[256] = {0};
                strncpy(temp, lineStart, charCount);
                lineWidth = MeasureText(temp, fontSize);
            }

            if (lineWidth >= maxWidth && lastSpace >= 0)
            {
                charCount = lastSpace;
            }

            char tempLine[256] = {0};
            strncpy(tempLine, lineStart, charCount);
            DrawText(tempLine, x, y, fontSize, WHITE);

            lineStart += charCount;
            while (*lineStart == ' ') lineStart++;
            y += lineHeight;
        }
    }

    if (fadeWhiteAlpha > 0.0f)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      (Color){255, 255, 255, (unsigned char)(fadeWhiteAlpha * 255)});
    }

    EndDrawing();
}

void UnloadFase1_3(void)
{
    UnloadTexture(background);
    UnloadMusicStream(interrogationMusic);
    UnloadTexture(spriteNome);
    UnloadTexture(spriteBustup);
    UnloadTexture(spriteConfiante);
    UnloadSound(somSurpresa);
    UnloadSound(somFalaDetetive);
}
