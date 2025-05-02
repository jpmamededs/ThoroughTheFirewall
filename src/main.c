#include "raylib.h"
#include "cutscenes.h"
#include "menu.h"
#include "intro.h"
#include "fase1.h"    // <-- Adicionado

typedef enum {
    APP_CUTSCENES,
    APP_MENU,
    APP_INTRO,
    APP_FASE1      // <-- Novo estado para fase1
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
                InitFase1();      // <-- Inicialize a Fase 1 aqui!
                state = APP_FASE1;
            }
        }
        else if (state == APP_FASE1)
        {
            UpdateFase1();
            DrawFase1();
            // Se quiser, adicione IF para avançar para próxima fase/estado aqui
        }
    }

    // Liberação segura dos recursos ao sair, dependendo do estado:
    if (state == APP_CUTSCENES) UnloadCutscenes();
    if (state == APP_MENU)      UnloadMenu();
    if (state == APP_INTRO)     UnloadIntro();
    if (state == APP_FASE1)     UnloadFase1();

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}