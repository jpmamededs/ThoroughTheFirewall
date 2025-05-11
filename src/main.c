#include "raylib.h"
#include "cutscenes.h"
#include "menu.h"
#include "intro.h"
#include "ligacao_desconhecido.h"
#include "porta_batendo.h"
#include "interrogatorio.h"
#include "gemini.h"
#include "faseFinal.h"
#include "firewall.h"
#include "pc_screenFinal.h"
#include "fase2.h"
#include "generalFunctions.h"
#include "debug.h"
#include "fase3.h"
#include "fase4.h"
#include "fase5.h"
#include "fase6.h"
#include "fasePcServer.h"
#include <stdlib.h>
#include <time.h>

AppState state = APP_CUTSCENES;
AppState proxFasePosInterrogatorio;

typedef struct { const char *audio; const char *texto; } RoteiroHank;
static const RoteiroHank roteiros[] = {
    { 
        "src/music/surprise.mp3",
        "Bom trabalho na criptografia… Agora responda:"
    },
    { 
        "src/music/surprise.mp3",
        "Sua varredura de portas foi rápida, mas percebeu os falsos-positivos? Então me diga:"
    },
    {
        "src/music/surprise.mp3",
        "Interessante… Explorou bem aquele exploit zero-day. Agora responda:" 
    },
    { 
        "src/music/surprise.mp3",
        "Análise de logs impecável. Mas preciso saber:" 
    }
};

int main(void)
{
    srand(time(NULL));
    SelecionarPerguntasAleatorias();
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);

    InitWindow(screenWidth, screenHeight, "Blindspot Undercovered");
    SetWindowPosition(0, 0);

    InitAudioDevice();
    Music music = LoadMusicStream("src/music/musica_tema.mp3");
    SetMusicVolume(music, 0.9f);
    PlayMusicStream(music);

    InitCutscenes();

    static int perguntaAtual = -1;
    bool firewall_Initialized = false;
    bool pcScreenFinalInitialized = false;
    bool fase2Initialized = false;
    static bool fase_porta_batendo_Initialized = false;
    static bool fase3Initialized = false;
    static bool fase4Initialized = false;
    static bool fase5Initialized = false;
    static bool fase6Initialized = false;
    static bool interrogatorio_Initialized = false;
    static bool faseFinalInitialized = false;
    static bool fasePCServerInitialized = false; // <--- Flag ESPECÍFICA da PCServer

    extern bool interrogatorioFinalizado;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        if (state == APP_CUTSCENES)
        {
            UpdateCutscenes();
            DrawCutscenes();
            if (CutscenesEnded())
            {
                UnloadCutscenes();
                InitMenu();
                ResumeMusicStream(music);
                state = APP_MENU;
            }
        }
        else if (state == APP_MENU)
        {
            UpdateMenu();
            DrawMenu();
            if (MenuStartGame())
            {
                PauseMusicStream(music);
                UnloadMenu();
                float temposIntroShow[5] = {5.0f, 6.0f, 7.0f, 5.5f, 5.5f};
                float temposIntroEraser[5] = {1.2f, 1.2f, 1.2f, 1.2f, 1.2f};
                InitIntro(MenuSelectedCharacterName(), temposIntroShow, temposIntroEraser);
                state = APP_INTRO;
            }
            // DEBUG KEYS
            if (IsKeyPressed(KEY_P))
            {
                UnloadMenu();
                fase2Initialized = false;
                state = APP_PROVISORIO;
            }
            if (IsKeyPressed(KEY_O))
            {
                UnloadMenu();
                faseFinalInitialized = false;
                state = APP_FASEFINAL;
            }
            if (IsKeyPressed(KEY_I))
            {
                UnloadMenu();
                fase3Initialized = false;
                state = APP_DESAFIO_01;
            }
            if (IsKeyPressed(KEY_K))
            {
                UnloadMenu();
                fase4Initialized = false;
                state = APP_FASE4;
            }
            if (IsKeyPressed(KEY_L))
            {
                UnloadMenu();
                fase5Initialized = false;
                state = APP_FASE5;
            }
            if (IsKeyPressed(KEY_J))
            {
                UnloadMenu();
                fasePCServerInitialized = false;
                state = APP_SERVIDOR_PROXY;
            }
            if (IsKeyPressed(KEY_M))
            {
                UnloadMenu();
                //InitDebug();
                state = APP_FIREWALL;
            }
            if (IsKeyPressed(KEY_T))
            {
                UnloadMenu();
                Init_Interrogatorio(-1, NULL, NULL);
                interrogatorio_Initialized = true;
                proxFasePosInterrogatorio = APP_DESAFIO_01;
                state = INTERROGATORIO;
            }
            if (IsKeyPressed(KEY_H))
            {
                UnloadMenu();
                fase6Initialized = false;
                state = APP_DESAFIO_02;
            }
            // FIM DEBUG
        }
        else if (state == APP_INTRO)
        {
            UpdateIntro();
            DrawIntro();
            if (IntroEnded())
            {
                UnloadIntro();
                Init_Ligacao_Desconhecido();
                state = APP_LIGACAO_DESCONHECIDO;
                firewall_Initialized = false;
            }
        }
        else if (state == APP_LIGACAO_DESCONHECIDO)
        {
            Update_Ligacao_Desconhecido();
            Draw_Ligacao_Desconhecido(MenuSelectedCharacterName());
        }
        else if (state == APP_FIREWALL)
        {
            if (!firewall_Initialized)
            {
                Init_Firewall();
                firewall_Initialized = true;
            }
            AppState previousState = state;
            Update_Firewall();
            Draw_Firewall();
            if (previousState != state)
            {
                Unload_Firewall();
                firewall_Initialized = false;
            }
        }
        else if (state == APP_PORTA_BATENDO)
        {
            if (!fase_porta_batendo_Initialized)
            {
                Init_PORTA_BATENDO(MenuSelectedCharacterName());
                fase_porta_batendo_Initialized = true;
            }
            Update_PORTA_BATENDO();
            Draw_PORTA_BATENDO();
            if (Fase_PORTA_BATENDO_Concluida())
            {
                Init_Interrogatorio(-1, NULL, NULL);
                interrogatorio_Initialized = true;
                proxFasePosInterrogatorio = APP_DESAFIO_01;
                state = INTERROGATORIO;
            }
        }
        else if (state == INTERROGATORIO)
        {
            if (!interrogatorio_Initialized)
            {
                Init_Interrogatorio(perguntaAtual, roteiros[perguntaAtual].audio, roteiros[perguntaAtual].texto);
                interrogatorio_Initialized = true;
            }
            Update_Interrogatorio();
            Draw_Interrogatorio();
            if (interrogatorioFinalizado)
            {
                Unload_Interrogatorio();
                interrogatorio_Initialized = false;
                perguntaAtual++;
                state = proxFasePosInterrogatorio;
            }
        }
        else if (state == APP_PROVISORIO)
        {
            if (!fase2Initialized)
            {
                InitFase2();
                fase2Initialized = true;
            }
            UpdateFase2();
            DrawFase2();
            if (Fase2Concluida()) {
                UnloadFase2();
                fase2Initialized = false;
                proxFasePosInterrogatorio = APP_DESAFIO_01;
                state = INTERROGATORIO;
            }
        }
        else if (state == APP_DESAFIO_01)
        {
            if (!fase3Initialized)
            {
                InitFase3();
                fase3Initialized = true;
            }
            UpdateFase3();
            DrawFase3();
            if (Fase3Concluida()) {
                UnloadFase3();
                fase3Initialized = false;
                proxFasePosInterrogatorio = APP_SERVIDOR_PROXY;
                state = INTERROGATORIO;
            }
        }
        else if (state == APP_FASE4)
        {
            if (!fase4Initialized)
            {
                InitFase4();
                fase4Initialized = true;
            }
            UpdateFase4();
            DrawFase4();
            if (Fase4Concluida()) {
                UnloadFase4();
                fase4Initialized = false;
                proxFasePosInterrogatorio = APP_FASE5;
                state = INTERROGATORIO;
            }
        }
        else if (state == APP_FASE5)
        {
            if (!fase5Initialized)
            {
                InitFase5();
                fase5Initialized = true;
            }
            UpdateFase5();
            DrawFase5();
            if (Fase5Concluida()) {
                UnloadFase5();
                fase5Initialized = false;
                proxFasePosInterrogatorio = APP_DESAFIO_02;
                state = INTERROGATORIO;
            }
        }
        else if (state == APP_DESAFIO_02)
        {
            if (!fase6Initialized)
            {
                InitFase6();
                fase6Initialized = true;
            }
            UpdateFase6();
            DrawFase6();
            if (Fase6Concluida()) {
                UnloadFase6();
                fase6Initialized = false;
                proxFasePosInterrogatorio = APP_PROVISORIO;
                state = INTERROGATORIO;
            }
        }
        else if (state == APP_SERVIDOR_PROXY)
        {
            if (!fasePCServerInitialized)
            {
                InitFasePCServer();
                fasePCServerInitialized = true;
            }
            UpdateFasePCServer();
            DrawFasePCServer();
            if (FasePcServerConcluida()) {
                UnloadFasePCServer();
                fasePCServerInitialized = false;
                state = APP_DESAFIO_02; // o fluxo perde o sentido aqui, mas, na frente ele volta ao normal
            }
        }
        else if (state == APP_FASEFINAL)
        {
            if (!faseFinalInitialized)
            {
                InitFaseFinal();
                faseFinalInitialized = true;
            }
            UpdateFaseFinal();
            DrawFaseFinal();
        }
        else if (state == APP_PC_SCREEN_FINAL)
        {
            if (!pcScreenFinalInitialized)
            {
                InitPcScreenFinal();
                pcScreenFinalInitialized = true;
            }
            UpdatePcScreenFinal();
            DrawPcScreenFinal();
            if (IsKeyPressed(KEY_ESCAPE))
            {
                UnloadPcScreenFinal();
                pcScreenFinalInitialized = false;
                state = APP_MENU;
            }
        }
        else if (state == APP_DEBUG)
        {
            UpdateDebug();
            DrawDebug();
        }
    } // fechamento do while

    // Limpeza final (ao sair do loop)
    if (state == APP_CUTSCENES)
        UnloadCutscenes();
    else if (state == APP_MENU)
        UnloadMenu();
    else if (state == APP_INTRO)
        UnloadIntro();
    else if (state == APP_LIGACAO_DESCONHECIDO)
        Unload_Ligacao_Desconhecido();
    else if (state == APP_PROVISORIO)
        UnloadFase2();
    else if (state == APP_PORTA_BATENDO)
        Unload_PORTA_BATENDO();
    else if (state == APP_DESAFIO_01)
        UnloadFase3();
    else if (state == APP_FASE4)
        UnloadFase4();
    else if (state == APP_FASE5)
        UnloadFase5();
    else if (state == APP_DESAFIO_02)
        UnloadFase6();
    else if (state == INTERROGATORIO)
        Unload_Interrogatorio();
    else if (state == APP_FASEFINAL)
        UnloadFaseFinal();
    else if (state == APP_FIREWALL)
        Unload_Firewall();
    else if (state == APP_PC_SCREEN_FINAL)
        UnloadPcScreenFinal();
    else if (state == APP_SERVIDOR_PROXY)
        UnloadFasePCServer();
    else if (state == APP_DEBUG)
        UnloadDebug();

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}