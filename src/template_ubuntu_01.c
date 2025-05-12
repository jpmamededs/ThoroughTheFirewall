#include "template_ubuntu_01.h"
#include "generalFunctions.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <direct.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

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

static int estadoCaixa = 0;
static float tempoCaixaDialogo = 0.0f;
static const float trocaMensagemDelay = 3.0f;

static bool iniciandoTransicao = false;
static float tempoFadeOut = 0.0f;
static float tempoAposFade = 0.0f;
static float tempoMensagemFinal = 0.0f;
static bool aguardandoMensagemFinal = false;

static const float esperaPreta = 2.0f;
static const float tempoMensagemFinalDelay = 2.0f;

static bool fase_concluida = false;

void Init_Template_Ubuntu_01(void)
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
    fase_concluida = false;

    float geminiAnimScale = 1.0f / 13.5f;
    geminiFinalPos = (Vector2){
        GetScreenWidth() - geminiIcon.width * geminiAnimScale - 20,
        GetScreenHeight() - geminiIcon.height * geminiAnimScale - 60
    };

    geminiAnimPos = (Vector2){GetScreenWidth(), geminiFinalPos.y};

    geminiAnimDone = false;
    geminiAnimStarted = false;
    geminiAnimTimer = 0.0f;
    mostrarCaixaDialogo = false;
    tempoPosAnimacao = 0.0f;
    tempoCaixaDialogo = 0.0f;
    estadoCaixa = 0;

    iniciandoTransicao = false;
    tempoFadeOut = 0.0f;
    tempoAposFade = 0.0f;
    tempoMensagemFinal = 0.0f;
    aguardandoMensagemFinal = false;
}

void Update_Template_Ubuntu_01(void)
{
    float dt = GetFrameTime();
    fadeTimer += dt;

    if (!showBackground && fadeTimer >= (fadeDuration + fadePause))
        showBackground = true;

    if (showBackground && !bootSoundPlayed)
    {
        PlaySound(bootSound);
        bootSoundPlayed = true;
    }

    if (showBackground && !terminalChamado && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();
        Rectangle terminalIconBounds = {10, 10, terminalIcon.width * 1.5f, terminalIcon.height * 2.0f};

        if (CheckCollisionPointRec(mouse, terminalIconBounds))
        {
            char cwd[512];
            if (_getcwd(cwd, sizeof(cwd)) != NULL)
            {
                char command[1024];
                snprintf(command, sizeof(command),
                         "start \"\" \"%s\\first_terminal.bat\"", cwd);
                system(command);
                terminalChamado = true;
            }
        }
    }

    DIR *d = opendir(".");
    struct dirent *dir;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (strcmp(dir->d_name, "dados.txt") == 0)
            {
                remove("dados.txt");

                estadoCaixa = 2;
                tempoMensagemFinal = 0.0f;
                aguardandoMensagemFinal = true;
                break;
            }
        }
        closedir(d);
    }

    if (aguardandoMensagemFinal)
    {
        tempoMensagemFinal += dt;
        if (tempoMensagemFinal >= tempoMensagemFinalDelay)
        {
            iniciandoTransicao = true;
            aguardandoMensagemFinal = false;
            tempoFadeOut = 0.0f;
            tempoAposFade = 0.0f;
        }
    }

    if (bootSoundPlayed && !geminiAnimStarted)
    {
        geminiAnimTimer += dt;
        if (geminiAnimTimer >= geminiAnimCooldown)
            geminiAnimStarted = true;
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
            mostrarCaixaDialogo = true;
    }

    if (mostrarCaixaDialogo && estadoCaixa == 0)
    {
        tempoCaixaDialogo += dt;
        if (tempoCaixaDialogo >= trocaMensagemDelay)
            estadoCaixa = 1;
    }

    if (iniciandoTransicao)
    {
        tempoFadeOut += dt;
        if (tempoFadeOut >= 1.0f)
        {
            tempoAposFade += dt;
            if (tempoAposFade >= esperaPreta)
            {
                // Isso define para Main que a fase se encerrou!
                // Caso queira mandar para outra fase, basta apenas trocar o (state) na main
                fase_concluida = true;
            }
        }
    }
}

void Draw_Template_Ubuntu_01(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    if (showBackground)
    {
        DrawTexturePro(background, (Rectangle){0, 0, background.width, background.height},
                       (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()}, (Vector2){0, 0}, 0.0f, WHITE);
    }
    else
    {
        float t = fmodf(fadeTimer, fadeDuration + fadePause);
        float alpha = (t < fadeDuration) ? fabsf(sinf(t / fadeDuration * PI)) : 0.0f;
        Vector2 pos = {(GetScreenWidth() - wallpaper.width * 0.3f) / 2, (GetScreenHeight() - wallpaper.height * 0.3f) / 2};
        DrawTextureEx(wallpaper, pos, 0.0f, 0.3f, (Color){255, 255, 255, (unsigned char)(alpha * 255)});
    }

    DrawRectangle(0, 0, 80, GetScreenHeight(), (Color){0, 0, 0, 76});

    if (showBackground)
    {
        int iconMargin = 10;
        float terminalScale = 1.3f;
        float geminiAnimScale = 1.0f / 13.5f;

        DrawTextureEx(terminalIcon, (Vector2){iconMargin, iconMargin}, 0.0f, terminalScale, WHITE);

        if (geminiAnimStarted)
            DrawTextureEx(geminiIcon, geminiAnimPos, 0.0f, geminiAnimScale, WHITE);

        if (mostrarCaixaDialogo)
        {
            const char *texto = (estadoCaixa == 0)
                                    ? "Nova tarefa detectada!"
                                : (estadoCaixa == 1)
                                    ? "Para continuar sua tarefa. Clique no icone do terminal."
                                    : "Parabens, voce terminou suas pendencias.";

            int padding = 20;
            int fontSize = 18;
            Vector2 textSize = MeasureTextEx(geminiFont, texto, fontSize, 1);
            int largura = (int)textSize.x + padding * 2;
            int altura = (int)textSize.y + padding * 2;

            int x = geminiFinalPos.x - largura - 20;
            int y = geminiFinalPos.y - ((altura - 110) / 8);

            DrawRectangleRounded((Rectangle){x, y, largura, altura}, 0.3f, 16, WHITE);
            DrawTextEx(geminiFont, texto, (Vector2){x + padding, y + padding}, fontSize, 1, DARKGRAY);
        }
    }

    if (iniciandoTransicao)
    {
        float alpha = (tempoFadeOut < 1.0f) ? tempoFadeOut / 1.0f : 1.0f;
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      (Color){0, 0, 0, (unsigned char)(alpha * 255)});
    }

    EndDrawing();
}

bool Fase_Template_Ubuntu_01_Concluida(void)
{
    return fase_concluida;
}

void Unload_Template_Ubuntu_01(void)
{
    UnloadTexture(wallpaper);
    UnloadTexture(background);
    UnloadTexture(terminalIcon);
    UnloadTexture(geminiIcon);
    UnloadSound(bootSound);
    UnloadFont(geminiFont);
}