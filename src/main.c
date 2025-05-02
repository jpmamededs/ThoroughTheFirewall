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

    // --- Música global, iniciada UMA VEZ ---
    InitAudioDevice();
    Music music = LoadMusicStream("src/music/EisenfunkPong-[AudioTrimmer.com] (1).mp3");
    PlayMusicStream(music);

    AppState state = APP_CUTSCENES;
    InitCutscenes();

    bool showCharacterName = false; // <-- ESSA VARIÁVEL CONTROLADORA

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
                state = APP_MENU;
            }
        }
        else if (state == APP_MENU)
        {
            UpdateMenu();
            DrawMenu();
            if (MenuStartGame())
            {
                UnloadMenu();
                InitIntro(MenuSelectedCharacterName());
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

            if (!showCharacterName) {
                DrawFase1();
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                    showCharacterName = true;
                }
            } else {
                BeginDrawing();
                ClearBackground(GREEN);
                DrawText("Personagem escolhido:", 40, 40, 28, BLACK);
                DrawText(MenuSelectedCharacterName(), 80, 80, 40, DARKGRAY);
                EndDrawing();

                // se quiser voltar para a fase
                // if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                //     showCharacterName = false;
                // }
                // se quiser mudar para próxima fase, pode mudar o "state" aqui!
            }
        }
    }

    if (state == APP_CUTSCENES) UnloadCutscenes();
    if (state == APP_MENU)      UnloadMenu();
    if (state == APP_INTRO)     UnloadIntro();
    if (state == APP_FASE1)     UnloadFase1();

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}