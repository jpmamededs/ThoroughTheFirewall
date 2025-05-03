#include "raylib.h"
#include "cutscenes.h"
#include "menu.h"
#include "intro.h"
#include "fase1.h"
#include "gemini.h"
#include "pc_screen.h"
#include "fase2.h"   // Inclua a header da fase2
#include "generalFunctions.h"

AppState state = APP_CUTSCENES; // Global como antes

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
    bool fase2Initialized = false; // Controle para fase2

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
            // NOVO: Se pressionar "P", vai direto para fase2
            if (IsKeyPressed(KEY_P))
            {
                UnloadMenu();
                fase2Initialized = false; // força o InitFase2 depois
                state = APP_FASE2;
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
            // Se apertar "P" vai para fase2
            if (IsKeyPressed(KEY_P))
            {
                // Poderia descarregar recursos do PC se precisar...
                pcScreenInitialized = false;
                fase2Initialized = false; // Força init da fase2
                state = APP_FASE2;
            }
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
            // Outros controles de mudança de estado em fase2,
            // se/quando quiser sair dela, tipo apertar ESC ou outro botão.
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
    if (state == APP_FASE2)
        UnloadFase2();
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}