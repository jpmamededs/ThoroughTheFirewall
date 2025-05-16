#include "cutscenes.h"
#include "menu.h"
#include "intro.h"
#include "ligacao_desconhecido.h"
#include "firewall.h"
#include "porta_batendo.h"
#include "interrogatorio.h"
#include "desafio_01.h"
#include "servidor_proxy.h"
#include "desafio_02.h"
#include "desafio_03.h"
#include "pendrive.h"
#include "brute-force.h"
#include "desafio_04.h"
#include "gemini.h"
#include "generalFunctions.h"
#include "debug.h"
#include "fase4.h"
#include "ubuntu_provisorio.h"
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "template_ubuntu_01.h"
#include "template_ubuntu_02.h"
#include "template_3D_01.h"
#include "template_3D_02.h"
#include "template_3D_03.h"
#include "tela_provisoria_01.h"
#include "tela_provisoria_02.h"
#include "loading_screen.h"
#include "transition_screen.h"
#include "playerStats.h"
#include "ranking.h"

AppState state = APP_CUTSCENES;
AppState PFP_Iterrogatorio;
AppState PFP_Loading;
AppState PFP_Trasicao;

PlayerStats playerStats;

typedef struct { const char *audio; const char *texto; } RoteiroHank;
static const RoteiroHank roteiros[] = {
    { 
        "src/music/fala_hank_desafio01.mp3",
        "Impressionante! Você conseguiu identificar as prioridades do sistema sob pressão e realocar os recursos de forma "
        "eficiente, mantendo a calma e controlando a situação da melhor forma possivel. Isso demonstra uma capacidade de "
        "tomada de decisão rápida e precisa e essa habilidade é fundamental para garantir a integridade dos sistemas em "
        "situações de crise. [ENTER]"
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
    SetTargetFPS(870);
    SetWindowPosition(0, 0); 

    InitAudioDevice();
    Music music = LoadMusicStream("src/music/musica.mp3");
    SetMusicVolume(music, 0.9f);
    PlayMusicStream(music);

    InitPlayerStats(&playerStats);

    InitCutscenes();

    static int perguntaAtual = -1;
    bool firewall_Initialized = false;
    bool bruteforce_Initialized = false;
    bool desafio_03_Initialized = false;
    bool ligacao_desconhecido_Initialized = false;
    static bool porta_batendo_Initialized = false;
    static bool desafio_01_Initialized = false;
    static bool fase4Initialized = false;
    static bool ubuntoProvisorio_Initialized = false;
    static bool desafio_04_Initialized = false;
    static bool desafio_02_Initialized = false;
    static bool interrogatorio_Initialized = false;
    static bool pendrive_Initialized = false;
    static bool servidorProxy_Initialized = false;
    static bool template_ubuntu_01_Initialized = false;
    static bool template_ubuntu_02_Initialized = false;
    static bool template_3D_01_Initialized = false;
    static bool template_3D_02_Initialized = false;
    static bool template_3D_03_Initialized = false;
    static bool tela01_Initialized = false;
    static bool tela02_Initialized = false;
    static bool loading_Initialized = false;
    static bool transicao_Initialized = false;
    static bool ranking_Initialized = false;
    static bool menu_Initialized = false;
    static bool debug_Initialized = false;

    extern bool interrogatorioFinalizado;

    // DEBUG DE SELEÇÃO DO NOME
    strncpy(gSelectedCharacterName, "Alice", MAX_PLAYER_NAME);
    gSelectedCharacterName[MAX_PLAYER_NAME-1] = '\0';

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
                ResumeMusicStream(music);
                state = APP_MENU;
            }
        }
        else if (state == APP_MENU)
        {
            if (!menu_Initialized)
            {
                InitMenu();
                menu_Initialized = true;
            }
            UpdateMenu();
            DrawMenu();
            if (MenuShowRanking())
            {
                UnloadMenu();
                ranking_Initialized = false;
                menu_Initialized = false;
                state = APP_RANKING;
            }
            if (MenuStartGame())
            {
                PauseMusicStream(music);
                UnloadMenu();
                float temposIntroShow[3] = {12.0f, 8.0f, 10.0f};
                float temposIntroEraser[3] = {1.5f, 1.5f, 1.5f};
                InitIntro(temposIntroShow, temposIntroEraser);
                state = APP_INTRO;
            }
            // DEBUG KEYS
            if (IsKeyPressed(KEY_ONE))
            {
                PauseMusicStream(music);
                UnloadMenu();
                desafio_01_Initialized = false;
                state = APP_DESAFIO_01;
            }
            if (IsKeyPressed(KEY_TWO))
            {
                PauseMusicStream(music);
                UnloadMenu();
                desafio_02_Initialized = false;
                state = APP_DESAFIO_02;
            }
            if (IsKeyPressed(KEY_THREE))
            {
                PauseMusicStream(music);
                UnloadMenu();
                desafio_03_Initialized = false;
                state = APP_DESAFIO_03;
            }
            if (IsKeyPressed(KEY_FOUR))
            {
                PauseMusicStream(music);
                UnloadMenu();
                desafio_04_Initialized = false;
                state = APP_DESAFIO_04;
            }
            if (IsKeyPressed(KEY_O))
            {
                PauseMusicStream(music);
                UnloadMenu();
                pendrive_Initialized = false;
                state = APP_PENDRIVE;
            }
            if (IsKeyPressed(KEY_K))
            {
                PauseMusicStream(music);
                UnloadMenu();
                fase4Initialized = false;
                state = APP_FASE4;
            }
            if (IsKeyPressed(KEY_L))
            {
                PauseMusicStream(music);
                UnloadMenu();
                ubuntoProvisorio_Initialized = false;
                state = APP_UBUNTU_PROVISORIO;
            }
            if (IsKeyPressed(KEY_Z))
            {
                PauseMusicStream(music);
                UnloadMenu();
                template_ubuntu_01_Initialized = false;
                state = APP_TEMPLATE_UBUNTU_01;
            }
            if (IsKeyPressed(KEY_X))
            {
                PauseMusicStream(music);
                UnloadMenu();
                template_ubuntu_02_Initialized = false;
                state = APP_TEMPLATE_UBUNTU_02;
            }
            if (IsKeyPressed(KEY_C))
            {
                PauseMusicStream(music);
                UnloadMenu();
                template_3D_01_Initialized = false;
                state = APP_TEMPLATE_3D_01;
            }
            if (IsKeyPressed(KEY_V))
            {
                PauseMusicStream(music);
                UnloadMenu();
                template_3D_02_Initialized = false;
                state = APP_TEMPLATE_3D_02;
            }
            if (IsKeyPressed(KEY_E))
            {
                PauseMusicStream(music);
                UnloadMenu();
                template_3D_03_Initialized = false;
                state = APP_TEMPLATE_3D_03;
            }
            if (IsKeyPressed(KEY_J))
            {
                PauseMusicStream(music);
                UnloadMenu();
                servidorProxy_Initialized = false;
                state = APP_SERVIDOR_PROXY;
            }
            if (IsKeyPressed(KEY_M))
            { 
                PauseMusicStream(music);
                UnloadMenu();
                InitDebug();
                state = APP_DEBUG;
            }
            if (IsKeyPressed(KEY_T))
            {
                PauseMusicStream(music);
                UnloadMenu();
                perguntaAtual++;
                Init_Interrogatorio(-1, NULL, NULL);
                interrogatorio_Initialized = true;
                PFP_Iterrogatorio = APP_DESAFIO_01;
                state = INTERROGATORIO;
            }
            if (IsKeyPressed(KEY_B))
            {
                PauseMusicStream(music);
                UnloadMenu();
                tela01_Initialized = false;
                state = APP_TELA_01;
            }
            if (IsKeyPressed(KEY_G))
            {
                PauseMusicStream(music);
                UnloadMenu();
                tela02_Initialized = false;
                state = APP_TELA_02;
            }
            if (IsKeyPressed(KEY_F))
            {
                PauseMusicStream(music);
                UnloadMenu();
                transicao_Initialized = false;
                state = APP_TRANSICAO;
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
                state = APP_LOADING_SCREEN;
                PFP_Loading = APP_LIGACAO_DESCONHECIDO;
            }
        }
        else if (state == APP_LOADING_SCREEN)
        {
            if (!loading_Initialized)
            {
                Init_LoadingScreen(5.0f);
                loading_Initialized = true;
            }
            Update_LoadingScreen();
            Draw_LoadingScreen();
            if (LoadingScreen_Done())
            {
                Unload_LoadingScreen();
                loading_Initialized = false;
                state = PFP_Loading;
            }
        }
        else if (state == APP_LIGACAO_DESCONHECIDO)
        {
            if (!ligacao_desconhecido_Initialized)
            {
                Init_Ligacao_Desconhecido();
                ligacao_desconhecido_Initialized = true;
            }
            Update_Ligacao_Desconhecido();
            Draw_Ligacao_Desconhecido();
            if (Fase_Ligacao_Desconhecido_Concluida())
            {
                Unload_Ligacao_Desconhecido();
                ligacao_desconhecido_Initialized = false;
                state = APP_FIREWALL;
            }
        }
        else if (state == APP_FIREWALL)
        {
            if (!firewall_Initialized)
            {
                Init_Firewall();
                firewall_Initialized = true;
            }
            Update_Firewall();
            Draw_Firewall();
            if (Fase_Firewall_Concluida())
            {
                Unload_Firewall();
                firewall_Initialized = false;
                state = APP_PORTA_BATENDO;
            }
        }
        else if (state == APP_PORTA_BATENDO)
        {
            if (!porta_batendo_Initialized)
            {
                Init_Porta_Batendo();
                porta_batendo_Initialized = true;
            }
            Update_Porta_Batendo();
            Draw_Porta_Batendo();
            if (Fase_Porta_Batendo_Concluida())
            {
                Unload_Porta_Batendo();
                porta_batendo_Initialized = false;

                Init_Interrogatorio(-1, NULL, NULL);
                interrogatorio_Initialized = true;
                state = INTERROGATORIO;

                PFP_Iterrogatorio = APP_TRANSICAO;
                PFP_Trasicao = APP_DESAFIO_01;
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
                state = PFP_Iterrogatorio;
            }
        }
        else if (state == APP_TRANSICAO)
        {
            if (!transicao_Initialized)
            {
                Init_TransitionScreen(1, "Redirecione os sistemas criticos");
                transicao_Initialized = true;
            }
            Update_TransitionScreen();
            Draw_TransitionScreen();
            if (TransitionScreenDone())
            {
                Unload_TransitionScreen();
                transicao_Initialized = false;
                state = PFP_Trasicao;
            }
        }
        else if (state == APP_DESAFIO_01)
        {
            if (!desafio_01_Initialized)
            {
                Init_Desafio_01();
                desafio_01_Initialized = true;
            }
            Update_Desafio_01();
            Draw_Desafio_01();
            if (Fase_Desafio_01_Concluida()) 
            {
                Unload_Desafio_01();
                desafio_01_Initialized = false;

                state = APP_LOADING_SCREEN;
                PFP_Loading = INTERROGATORIO;
                PFP_Iterrogatorio = APP_SERVIDOR_PROXY;
            }
        }
        else if (state == APP_SERVIDOR_PROXY)
        {
            if (!servidorProxy_Initialized)
            {
                Init_ServidorProxy();
                servidorProxy_Initialized = true;
            }
            Update_ServidorProxy();
            Draw_ServidorProxy();
            if (Fase_ServidorProxy_Concluida()) 
            {
                Unload_ServidorProxy();
                servidorProxy_Initialized = false;

                Init_TransitionScreen(2, "Cifra De Cesar");
                transicao_Initialized = true;

                state = APP_TRANSICAO;
                PFP_Trasicao = APP_DESAFIO_02;
            }
        }
        else if (state == APP_DESAFIO_02)
        {
            if (!desafio_02_Initialized)
            {
                Init_Desafio_02();
                desafio_02_Initialized = true;
            }
            Update_Desafio_02();
            Draw_Desafio_02();
            if (Fase_Desafio_02_Concluida()) 
            {
                Unload_Desafio_02();
                desafio_02_Initialized = false;

                state = APP_LOADING_SCREEN;
                PFP_Loading = INTERROGATORIO;
                PFP_Iterrogatorio = APP_DESAFIO_03; // KEYLOGGER!!!!!
            }
        }
        // KEYLOGGER!!!!!
        else if (state == APP_DESAFIO_03)
        {
            if (!desafio_03_Initialized)
            {
                Init_Desafio_03();
                desafio_03_Initialized = true;
            }
            Update_Desafio_03();
            Draw_Desafio_03();
            if (Fase_Desafio_03_Concluida()) 
            {
                Unload_Desafio_03();
                desafio_03_Initialized = false;

                state = APP_LOADING_SCREEN;
                PFP_Loading = INTERROGATORIO;
                PFP_Iterrogatorio = APP_PENDRIVE;
            }
        }
        else if (state == APP_PENDRIVE)
        {
            if (!pendrive_Initialized)
            {
                Init_Pendrive();
                pendrive_Initialized = true;
            }
            Update_Pendrive();
            Draw_Pendrive();
            if (Fase_Pendrive_Concluida())
            {
                Unload_Pendrive();
                pendrive_Initialized = false;
                state = APP_BRUTEFORCE;
            }
        }
        else if (state == APP_BRUTEFORCE)
        {
            if (!bruteforce_Initialized)
            {
                Init_BruteForce();
                bruteforce_Initialized = true;
            }
            Update_BruteForce();
            Draw_BruteForce();
            if (Fase_BruteForce_Concluida())
            {
                Unload_BruteForce();
                bruteforce_Initialized = false;

                Init_TransitionScreen(4, "Fruit Ninja");
                transicao_Initialized = true;

                state = APP_TRANSICAO;
                PFP_Trasicao = APP_DESAFIO_04;
            }
        }
        else if (state == APP_DESAFIO_04)
        {
            if (!desafio_04_Initialized)
            {
                Init_Desafio_04();
                desafio_04_Initialized = true;
            }
            Update_Desafio_04();
            Draw_Desafio_04();
            if (Fase_Desafio_04_Concluida()) 
            {
                Unload_Desafio_04();
                desafio_04_Initialized = false;

                state = APP_LOADING_SCREEN;
                PFP_Loading = INTERROGATORIO;
                PFP_Iterrogatorio = APP_DEBUG; // Futuramente SHELL REVERSO!
            }
        }
        // ===== FIM DO JOGO! =====
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
                PFP_Iterrogatorio = APP_UBUNTU_PROVISORIO;
                state = INTERROGATORIO;
            }
        }
        else if (state == APP_UBUNTU_PROVISORIO)
        {
            if (!ubuntoProvisorio_Initialized)
            {
                Init_Ubuntu_Provisorio();
                ubuntoProvisorio_Initialized = true;
            }
            Update_Ubuntu_Provisorio();
            Draw_Ubuntu_Provisorio();
            if (Fase_Ubuntu_Provisorio_Concluida()) {
                Unload_Ubuntu_Provisorio();
                ubuntoProvisorio_Initialized = false;
                state = APP_DEBUG;
            }
        }
        else if (state == APP_TEMPLATE_UBUNTU_01)
        {
            if (!template_ubuntu_01_Initialized)
            {
                Init_Template_Ubuntu_01();
                template_ubuntu_01_Initialized = true;
            }
            Update_Template_Ubuntu_01();
            Draw_Template_Ubuntu_01();
            if (Fase_Template_Ubuntu_01_Concluida()) 
            {
                Unload_Template_Ubuntu_01();
                template_ubuntu_01_Initialized = false;
                state = APP_DEBUG; // MUDAR O STATE AQUI!
            }
        }
        else if (state == APP_TEMPLATE_UBUNTU_02)
        {
            if (!template_ubuntu_02_Initialized)
            {
                Init_Template_Ubuntu_02();
                template_ubuntu_02_Initialized = true;
            }
            Update_Template_Ubuntu_02();
            Draw_Template_Ubuntu_02();
            if (Fase_Template_Ubuntu_02_Concluida()) 
            {
                Unload_Template_Ubuntu_02();
                template_ubuntu_02_Initialized = false;
                state = APP_DEBUG; // MUDAR O STATE AQUI!
            }
        }
        else if (state == APP_TEMPLATE_3D_01)
        {
            if (!template_3D_01_Initialized)
            {
                Init_Template_3D_01();
                template_3D_01_Initialized = true;
            }
            Update_Template_3D_01();
            Draw_Template_3D_01();
            if (Fase_Template_3D_01_Concluida()) 
            {
                Unload_Template_3D_01();
                template_3D_01_Initialized = false;
                state = APP_DEBUG;
            }
        }
        else if (state == APP_TEMPLATE_3D_02)
        {
            if (!template_3D_02_Initialized)
            {
                Init_Template_3D_02();
                template_3D_02_Initialized = true;
            }
            Update_Template_3D_02();
            Draw_Template_3D_02();
            if (Fase_Template_3D_02_Concluida()) 
            {
                Unload_Template_3D_02();
                template_3D_02_Initialized = false;
                state = APP_DEBUG;
            }
        }
        else if (state == APP_TEMPLATE_3D_03)
        {
            if (!template_3D_03_Initialized)
            {
                Init_Template_3D_03();
                template_3D_03_Initialized = true;
            }
            Update_Template_3D_03();
            Draw_Template_3D_03();
            if (Fase_Template_3D_03_Concluida())
            {
                Unload_Template_3D_03();
                template_3D_03_Initialized = false;
                state = APP_DEBUG;
            }
        }
        else if (state == APP_TELA_01)
        {
            if (!tela01_Initialized)
            {
                Init_Tela_01();
                tela01_Initialized = true;
            }
            Update_Tela_01();
            Draw_Tela_01();
            if (Fase_Tela01_Concluida()) {
                Unload_Tela_01();
                tela01_Initialized = false;
                state = APP_DEBUG;
            }
        }
        else if (state == APP_TELA_02)
        {
            if (!tela02_Initialized)
            {
                Init_Tela_02();
                tela02_Initialized = true;
            }
            Update_Tela_02();
            Draw_Tela_02();
            if (Fase_Tela02_Concluida()) {
                Unload_Tela_02();
                tela02_Initialized = false;
                state = APP_DEBUG;
            }
        }
        else if (state == APP_RANKING)
        {
            if (!ranking_Initialized)
            {
                Init_Ranking();
                ranking_Initialized = true;
            }
            Update_Ranking();
            Draw_Ranking();
            if (Ranking_Concluido()) {
                Unload_Ranking();
                ranking_Initialized = false;
                state = APP_MENU;
            }
        }
        else if (state == APP_DEBUG)
        {
            if (!debug_Initialized)
            {
                SetPlayerGeneralStats(&playerStats);
                debug_Initialized = true;
            }
            UpdateDebug();
            DrawDebug();
        }
    }

    // Limpeza final (ao sair do loop)
    if (state == APP_CUTSCENES)
        UnloadCutscenes();
    else if (state == APP_MENU)
        UnloadMenu();
    else if (state == APP_INTRO)
        UnloadIntro();
    else if (state == APP_LIGACAO_DESCONHECIDO)
        Unload_Ligacao_Desconhecido();
    else if (state == APP_DESAFIO_03)
        Unload_Desafio_03();
    else if (state == APP_PORTA_BATENDO)
        Unload_Porta_Batendo();
    else if (state == APP_DESAFIO_01)
        Unload_Desafio_01();
    else if (state == APP_FASE4)
        UnloadFase4();
    else if (state == APP_UBUNTU_PROVISORIO)
        Unload_Ubuntu_Provisorio();
    else if (state == APP_DESAFIO_04)
        Unload_Desafio_04();
    else if (state == APP_DESAFIO_02)
        Unload_Desafio_02();
    else if (state == INTERROGATORIO)
        Unload_Interrogatorio();
    else if (state == APP_PENDRIVE)
        Unload_Pendrive();
    else if (state == APP_FIREWALL)
        Unload_Firewall();
    else if (state == APP_BRUTEFORCE)
        Unload_BruteForce();
    else if (state == APP_SERVIDOR_PROXY)
        Unload_ServidorProxy();
    else if (state == APP_DEBUG)
        UnloadDebug();

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}