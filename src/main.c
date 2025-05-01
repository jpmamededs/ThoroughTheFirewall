#include "raylib.h"
#include "cutscenes.h"
#include "menu.h"
#include "intro.h"

typedef enum { APP_CUTSCENES, APP_MENU, APP_INTRO, APP_GAME } AppState;

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

    // Inicie o menu/intro APENAS quando mudar de estado!
    // (não aqui antes do loop)

    while (!WindowShouldClose())
    {
        // MANTÉM a música tocando contínua durante TODO o jogo!
        UpdateMusicStream(music);

        if (state == APP_CUTSCENES)
        {
            UpdateCutscenes();
            DrawCutscenes();
            if (CutscenesEnded())
            {
                UnloadCutscenes();
                InitMenu(); // Inicialize o menu AQUI!
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
                InitIntro(MenuSelectedCharacterName()); // Passa nome para intro!
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
                state = APP_GAME;
            }
        }
        else if (state == APP_GAME)
        {
            BeginDrawing();
            ClearBackground(GREEN);
            DrawText("Personagem escolhido:", 40, 40, 28, BLACK);
            DrawText(MenuSelectedCharacterName(), 80, 80, 40, DARKGRAY);
            EndDrawing();
        }
    }

    // Liberação segura dos recursos ao sair, dependendo do estado:
    if (state == APP_CUTSCENES) UnloadCutscenes();
    if (state == APP_MENU)      UnloadMenu();
    if (state == APP_INTRO)     UnloadIntro();

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}