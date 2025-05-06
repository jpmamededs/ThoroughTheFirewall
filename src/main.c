#include "raylib.h"
#include "cutscenes.h"
#include "menu.h"
#include "intro.h"
#include "fase1.h"
#include "fase1_2.h"
#include "interrogatorio.h"
#include "gemini.h"
#include "faseFinal.h"
#include "pc_screen.h"
#include "fase2.h"
#include "generalFunctions.h"
#include "fase3.h"
#include "fase4.h"
#include "debug.h" 
#include <stdlib.h>
#include <time.h>

AppState state = APP_CUTSCENES;

int main(void)
{
    srand(time(NULL));
    SelecionarPerguntasAleatorias();

    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    InitWindow(screenWidth, screenHeight, "Blindspot Undercovered");
    SetWindowPosition(0, 0);
    InitAudioDevice();
    Music music = LoadMusicStream("src/music/EisenfunkPong-[AudioTrimmer.com] (1).mp3");
    PlayMusicStream(music);
    InitCutscenes();

    static int perguntaAtual = 0;
    bool pcScreenInitialized = false;
    bool fase2Initialized = false;
    static bool fase3Initialized = false;
    static bool fase4Initialized = false;
    static bool interrogatorio_Initialized = false;
    static bool faseFinalInitialized = false;
    static bool fase1_2Initialized = false;
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
                float temposIntro[5] = {8.4f, 12.1f, 9.1f, 8.6f, 7.5f};
                InitIntro(MenuSelectedCharacterName(), temposIntro);
                state = APP_INTRO;
            }
            // APENAS PARA DEBUG
            if (IsKeyPressed(KEY_P))
            {
                UnloadMenu();
                fase2Initialized = false;
                state = APP_FASE2;
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
                state = APP_FASE3;
            }
            if (IsKeyPressed(KEY_M)) {
                UnloadMenu();
                InitDebug();
                state = APP_DEBUG;
            }
            if (IsKeyPressed(KEY_T)) {
                UnloadMenu();
                InitInterrogatorio(perguntaAtual);
                interrogatorio_Initialized = true;
                state = INTERROGATORIO;
            }
            if (IsKeyPressed(KEY_K)) {
                UnloadMenu();
                fase4Initialized = false;
                state = APP_FASE4;
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
                InitFase1();
                state = APP_FASE1;
                pcScreenInitialized = false;
            }
        }
        else if (state == APP_FASE1)
        {
            UpdateFase1();
            DrawFase1();
        }
        else if (state == APP_PC_SCREEN)
        {
            if (!pcScreenInitialized)
            {
                InitPcScreen();
                pcScreenInitialized = true;
            }
            AppState previousState = state;
            UpdatePcScreen();
            DrawPcScreen();
            if (previousState != state)
            {
                UnloadPcScreen();
                pcScreenInitialized = false;
            }
            if (IsKeyPressed(KEY_P))
            {
                UnloadPcScreen();
                pcScreenInitialized = false;
                fase2Initialized = false;
                state = APP_FASE1_2;
            }
        }
        else if (state == APP_FASE1_2)
        {
            if (!fase1_2Initialized)
            {
                InitFase1_2();
                fase1_2Initialized = true;
            }
            UpdateFase1_2();
            DrawFase1_2();
        }
        else if (state == INTERROGATORIO)
        {
            if (!interrogatorio_Initialized)
            {
                InitInterrogatorio(perguntaAtual);
                interrogatorio_Initialized = true;
            }
            UpdateInterrogatorio();
            DrawInterrogatorio();
            
            if (interrogatorioFinalizado) {
                UnloadInterrogatorio();
                interrogatorio_Initialized = false;
                perguntaAtual++;
                // retoma o fluxo do jogo
                InitMenu();
                state = APP_MENU;
            }
        }
        else if (state == APP_FASE3)
        {
            if (!fase3Initialized)
            {
                InitFase3();
                fase3Initialized = true;
            }
            UpdateFase3();
            DrawFase3();
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
        }   
        else if (state == APP_FASE2)
        {
            if (!fase2Initialized)
            {
                InitFase2();
                fase2Initialized = true;
            }
            UpdateFase2();
            DrawFase2();
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
        else if (state == APP_DEBUG)
        {
            UpdateDebug();
            DrawDebug();
        }
    }

    // Limpeza
    if (state == APP_CUTSCENES)
        UnloadCutscenes();
    if (state == APP_MENU)
        UnloadMenu();
    if (state == APP_INTRO)
        UnloadIntro();
    if (state == APP_FASE1)
        UnloadFase1();
    if (state == APP_FASE2)
        UnloadFase2();
    if (state == APP_FASE1_2)
        UnloadFase1_2();
    if (state == APP_FASE3)
        UnloadFase3();
    if (state == APP_FASE4)
        UnloadFase4(); 
    if (state == INTERROGATORIO)
        UnloadInterrogatorio();
    if (state == APP_FASEFINAL)
        UnloadFaseFinal();

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}