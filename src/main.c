#include "raylib.h"
#include "cutscenes.h"
#include "menu.h"
#include "intro.h"
#include "fase1.h"
#include "gemini.h"

typedef enum {
    APP_CUTSCENES,
    APP_MENU,
    APP_INTRO,
    APP_FASE1
} AppState;

int main(void)
{
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    InitWindow(screenWidth, screenHeight, "Sprite andando");
    SetWindowPosition(0, 0);

    InitAudioDevice();
    Music music = LoadMusicStream("src/music/EisenfunkPong-[AudioTrimmer.com] (1).mp3");
    PlayMusicStream(music);

    AppState state = APP_CUTSCENES;
    InitCutscenes();

    bool showCharacterName = false;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music); // Sempre atualiza, mas será pausado onde necessário

        if (state == APP_CUTSCENES)
        {
            UpdateCutscenes();
            DrawCutscenes();
            if (CutscenesEnded())
            {
                UnloadCutscenes();
                InitMenu();
                ResumeMusicStream(music); // <- Música volta ao entrar no menu
                state = APP_MENU;
            }
        }
        else if (state == APP_MENU)
        {
            UpdateMenu();
            DrawMenu();
            if (MenuStartGame())
            {
                PauseMusicStream(music); // <- Música para ao sair do menu
                UnloadMenu();

                float temposIntro[4] = {9.0f, 11.0f, 13.5f, 7.3f}; // segundos por página
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

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
