#include "raylib.h"
#include "cutscenes.h"
#include "menu.h"
#include "intro.h"
#include "fase1.h"
#include "gemini.h"
#include "pc_screen.h" // <- incluímos a nova tela

typedef enum
{
    APP_CUTSCENES,
    APP_MENU,
    APP_INTRO,
    APP_FASE1,
    APP_PC_SCREEN
} AppState;

AppState state = APP_CUTSCENES; // <- agora global

int main(void)
{
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    InitWindow(screenWidth, screenHeight, "Sprite andando");
    SetWindowPosition(0, 0);

    InitAudioDevice();
    Music music = LoadMusicStream("src/music/EisenfunkPong-[AudioTrimmer.com] (1).mp3");
    PlayMusicStream(music);

    InitCutscenes();

    bool showCharacterName = false;
    bool pcScreenInitialized = false;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music); // Atualiza música de fundo onde aplicável

        if (state == APP_CUTSCENES)
        {
            UpdateCutscenes();
            DrawCutscenes();
            if (CutscenesEnded())
            {
                UnloadCutscenes();
                InitMenu();
                ResumeMusicStream(music); // volta a música
                state = APP_MENU;
            }
        }
        else if (state == APP_MENU)
        {
            UpdateMenu();
            DrawMenu();
            if (MenuStartGame())
            {
                PauseMusicStream(music); // para música
                UnloadMenu();

                float temposIntro[4] = {9.0f, 11.0f, 13.5f, 7.3f};
                InitIntro(MenuSelectedCharacterName(), temposIntro);

                state = APP_INTRO;
            }
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
                showCharacterName = false;
                pcScreenInitialized = false; // caso volte do PC
            }
        }
        else if (state == APP_FASE1)
        {
            UpdateFase1();

            if (!showCharacterName)
            {
                DrawFase1();
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
                {
                    showCharacterName = true;
                }
            }
            else
            {
                BeginDrawing();
                ClearBackground(GREEN);
                DrawText("Personagem escolhido:", 40, 40, 28, BLACK);
                DrawText(MenuSelectedCharacterName(), 80, 80, 40, DARKGRAY);
                EndDrawing();
            }
        }
        else if (state == APP_PC_SCREEN)
        {
            if (!pcScreenInitialized)
            {
                InitPcScreen();
                pcScreenInitialized = true;
            }
            UpdatePcScreen();
            DrawPcScreen();
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
    // PC_SCREEN não tem sons ou alocação pesada aqui

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
