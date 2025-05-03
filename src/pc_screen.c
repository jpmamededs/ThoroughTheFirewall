#include "pc_screen.h"
#include "raylib.h"
#include <math.h>

static Texture2D wallpaper;
static Texture2D background;
static Sound bootSound;
static float fadeTimer = 0.0f;
static float fadeDuration = 3.0f;
static float fadePause = 1.0f;
static bool showBackground = false;
static bool bootSoundPlayed = false;

void InitPcScreen(void)
{
    wallpaper = LoadTexture("src/sprites/os/wallpaper.png");
    background = LoadTexture("src/sprites/os/background.jpg");
    bootSound = LoadSound("src/music/boot.mp3");

    fadeTimer = 0.0f;
    showBackground = false;
    bootSoundPlayed = false;
}

void UpdatePcScreen(void)
{
    float dt = GetFrameTime();
    fadeTimer += dt;

    // Após o tempo total (incluindo pausa), exibe o background
    if (!showBackground && fadeTimer >= (fadeDuration + fadePause))
    {
        showBackground = true;
    }

    // Toca som quando o background aparece
    if (showBackground && !bootSoundPlayed)
    {
        PlaySound(bootSound);
        bootSoundPlayed = true;
    }
}

void DrawPcScreen(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    if (showBackground)
    {
        DrawTexturePro(
            background,
            (Rectangle){0, 0, background.width, background.height},
            (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
            (Vector2){0, 0},
            0.0f,
            WHITE);
    }
    else
    {
        float cycleTime = fadeDuration; // 3 segundos de animação
        float t = fmodf(fadeTimer, cycleTime + fadePause);

        // intervalo de pausa entre loops
        float alpha = 0.0f;
        if (t < cycleTime)
        {
            alpha = sinf(t / cycleTime * PI);
            if (alpha < 0)
                alpha = -alpha;
        }

        Vector2 wallpaperPos = {
            (GetScreenWidth() - wallpaper.width * 0.3f) / 2,
            (GetScreenHeight() - wallpaper.height * 0.3f) / 2};

        DrawTextureEx(wallpaper, wallpaperPos, 0.0f, 0.3f,
                      (Color){255, 255, 255, (unsigned char)(alpha * 255)});
    }

    EndDrawing();
}
