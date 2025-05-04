#include "raylib.h"
#include "cutscenes.h"
#include "menu.h"
#include "intro.h"
#include "fase1.h"
#include "fase1_2.h"
#include "fase1_3.h" // <--- ADICIONADO
#include "gemini.h"
#include "pc_screen.h"
#include "fase2.h"
#include "generalFunctions.h"

AppState state = APP_CUTSCENES;

int main(void)
{
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    InitWindow(screenWidth, screenHeight, "Jogo AED");
    SetWindowPosition(0, 0);
    InitAudioDevice();
    Music music = LoadMusicStream("src/music/EisenfunkPong-[AudioTrimmer.com] (1).mp3");
    PlayMusicStream(music);
    InitCutscenes();
    bool pcScreenInitialized = false;
    bool fase2Initialized = false;
    bool fase1_3Initialized = false;

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
                float temposIntro[4] = {9.0f, 11.0f, 13.5f, 7.3f};
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
            static bool fase1_2Initialized = false;

            if (!fase1_2Initialized)
            {
                InitFase1_2();
                fase1_2Initialized = true;
            }

            UpdateFase1_2();
            DrawFase1_2();
        }
        else if (state == APP_FASE1_3)
        {
            if (!fase1_3Initialized)
            {
                InitFase1_3();
                fase1_3Initialized = true;
            }

            UpdateFase1_3();
            DrawFase1_3();
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
    if (state == APP_FASE1_3)
        UnloadFase1_3();

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
