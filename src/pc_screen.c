#include "pc_screen.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <direct.h> // Para _getcwd
#include <string.h> // Para strcat
#include <stdio.h>

static Texture2D wallpaper;
static Texture2D background;
static Texture2D terminalIcon;
static Texture2D geminiIcon;
static Sound bootSound;
static Font geminiFont;

static float fadeTimer = 0.0f;
static float fadeDuration = 3.0f;
static float fadePause = 1.0f;

static bool showBackground = false;
static bool bootSoundPlayed = false;
static bool terminalChamado = false;

static Vector2 geminiFinalPos;
static Vector2 geminiAnimPos;
static bool geminiAnimDone = false;
static bool geminiAnimStarted = false;
static float geminiAnimCooldown = 1.0f;
static float geminiAnimTimer = 0.0f;

static bool mostrarCaixaDialogo = false;
static float tempoPosAnimacao = 0.0f;
static float delayCaixaDialogo = 1.0f;

static int estadoCaixa = 0; // 0 = primeira mensagem, 1 = segunda
static float tempoCaixaDialogo = 0.0f;
static const float trocaMensagemDelay = 3.0f; // segundos

void InitPcScreen(void)
{
    wallpaper = LoadTexture("src/sprites/os/wallpaper.png");
    background = LoadTexture("src/sprites/os/background.jpg");
    terminalIcon = LoadTexture("src/sprites/os/terminal_icon.png");
    geminiIcon = LoadTexture("src/sprites/os/gemini.png");
    bootSound = LoadSound("src/music/boot.mp3");
    geminiFont = LoadFont("src/fonts/GoogleSansMono.ttf");

    fadeTimer = 0.0f;
    showBackground = false;
    bootSoundPlayed = false;
    terminalChamado = false;

    float geminiAnimScale = 1.0f / 13.5f;
    geminiFinalPos = (Vector2){
        GetScreenWidth() - geminiIcon.width * geminiAnimScale - 20,
        GetScreenHeight() - geminiIcon.height * geminiAnimScale - 20};

    geminiAnimPos = (Vector2){
        GetScreenWidth(),
        geminiFinalPos.y};

    geminiAnimDone = false;
    geminiAnimStarted = false;
    geminiAnimTimer = 0.0f;
    mostrarCaixaDialogo = false;
    tempoPosAnimacao = 0.0f;
    tempoCaixaDialogo = 0.0f;
    estadoCaixa = 0;
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
            char cwd[512];
            _getcwd(cwd, sizeof(cwd)); // pega o diretório do .exe

            char command[1024];
            snprintf(command, sizeof(command),
                     "start \"\" cmd /k \"cd /d \"%s\\JogoAED\" && hack.bat\"",
                     cwd);

            system(command);
            terminalChamado = true;
        }
    }

    if (bootSoundPlayed && !geminiAnimStarted)
    {
        geminiAnimTimer += dt;
        if (geminiAnimTimer >= geminiAnimCooldown)
        {
            geminiAnimStarted = true;
        }
    }

    if (geminiAnimStarted && !geminiAnimDone)
    {
        float speed = 600.0f * dt;
        if (geminiAnimPos.x > geminiFinalPos.x)
        {
            geminiAnimPos.x -= speed;
            if (geminiAnimPos.x < geminiFinalPos.x)
                geminiAnimPos.x = geminiFinalPos.x;
        }
        else
        {
            geminiAnimDone = true;
            tempoPosAnimacao = 0.0f;
        }
    }

    if (geminiAnimDone && !mostrarCaixaDialogo)
    {
        tempoPosAnimacao += dt;
        if (tempoPosAnimacao >= delayCaixaDialogo)
        {
            mostrarCaixaDialogo = true;
        }
    }

    if (mostrarCaixaDialogo && estadoCaixa == 0)
    {
        tempoCaixaDialogo += dt;
        if (tempoCaixaDialogo >= trocaMensagemDelay)
        {
            estadoCaixa = 1;
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

    // Barra lateral esquerda
    int barWidth = 80;
    DrawRectangle(0, 0, barWidth, GetScreenHeight(), (Color){0, 0, 0, 76});

    if (showBackground)
    {
        int iconMargin = 10;
        float terminalScale = 1.3f;
        float geminiSideScale = 0.06f;
        float geminiAnimScale = 1.0f / 13.5f;

        // Terminal
        Vector2 terminalPos = {iconMargin, iconMargin};
        DrawTextureEx(terminalIcon, terminalPos, 0.0f, terminalScale, WHITE);

        // Gemini barra lateral
        Vector2 geminiPos = {
            iconMargin + 1,
            iconMargin + terminalIcon.height * terminalScale + 8};
        DrawTextureEx(geminiIcon, geminiPos, 0.0f, geminiSideScale, WHITE);

        // Gemini animado
        if (geminiAnimStarted)
        {
            DrawTextureEx(geminiIcon, geminiAnimPos, 0.0f, geminiAnimScale, WHITE);
        }

        // Caixa de diálogo
        if (mostrarCaixaDialogo)
        {
            const char *texto = (estadoCaixa == 0)
                                    ? "Ola, sou a Gemini. Vou te instruir sempre que precisar."
                                    : "Para continuar sua tarefa. Clique no icone do terminal.";

            int padding = 20;
            int fontSize = 18;

            Vector2 textSize = MeasureTextEx(geminiFont, texto, fontSize, 1);
            int largura = (int)textSize.x + padding * 2;
            int altura = (int)textSize.y + padding * 2;

            int x = geminiFinalPos.x - largura - 20;
            int y = geminiFinalPos.y + 5 - altura / 2;

            DrawRectangleRounded((Rectangle){x, y, largura, altura}, 0.3f, 16, WHITE);
            DrawTextEx(geminiFont, texto, (Vector2){x + padding, y + padding}, fontSize, 1, DARKGRAY);
        }
    }

    EndDrawing();
}
