#include "pc_screen.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

static Texture2D wallpaper;
static Texture2D background;
static Texture2D terminalIcon;
static Sound bootSound;

static float fadeTimer = 0.0f;
static float fadeDuration = 3.0f;
static float fadePause = 1.0f;

static bool showBackground = false;
static bool bootSoundPlayed = false;
static bool terminalChamado = false;

void InitPcScreen(void)
{
    wallpaper = LoadTexture("src/sprites/os/wallpaper.png");
    background = LoadTexture("src/sprites/os/background.jpg");
    terminalIcon = LoadTexture("src/sprites/os/terminal_icon.png");
    bootSound = LoadSound("src/music/boot.mp3");

    fadeTimer = 0.0f;
    showBackground = false;
    bootSoundPlayed = false;
    terminalChamado = false;
}

void UpdatePcScreen(void)
{
    float dt = GetFrameTime();
    fadeTimer += dt;

    if (!showBackground && fadeTimer >= (fadeDuration + fadePause))
    {
        showBackground = true;
    }

    if (showBackground && !bootSoundPlayed)
    {
        PlaySound(bootSound);
        bootSoundPlayed = true;
    }

    if (showBackground && !terminalChamado && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();
        Rectangle terminalIconBounds = {
            10, 10,
            terminalIcon.width * 1.5f,
            terminalIcon.height * 2.0f};

        if (CheckCollisionPointRec(mouse, terminalIconBounds))
        {
            system("start cmd /k echo hello world");
            terminalChamado = true;
        }
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
        float cycleTime = fadeDuration;
        float t = fmodf(fadeTimer, cycleTime + fadePause);

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

    int barWidth = 80;
    DrawRectangle(0, 0, barWidth, GetScreenHeight(), (Color){0, 0, 0, 76});

    if (showBackground)
    {
        int iconMargin = 10;
        DrawTextureEx(terminalIcon, (Vector2){iconMargin, iconMargin}, 0.0f, 2.0f, WHITE);
    }

    EndDrawing();
}
