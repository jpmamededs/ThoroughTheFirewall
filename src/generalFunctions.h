#ifndef GENERALFUNCTIONS_H
#define GENERALFUNCTIONS_H

#include <stdbool.h>
#include "raylib.h"
#include <string.h>

// ========================
// ENUM DE ESTADOS GERAIS
// =====================
typedef enum
{
    APP_CUTSCENES,
    APP_MENU,
    APP_INTRO,
    APP_LIGACAO_DESCONHECIDO,
    APP_FIREWALL,
    APP_PORTA_BATENDO,
    INTERROGATORIO,
    APP_DESAFIO_01,
    APP_SERVIDOR_PROXY,
    APP_DESAFIO_02,
    // Ta certo só a parte de cima (ordem das fases)
    APP_PROVISORIO,
    APP_TEMPLATE_UBUNTU_01,
    APP_TEMPLATE_UBUNTU_02,
    APP_FASE4,
    APP_UBUNTU_PROVISORIO,
    APP_FASE6,
    APP_FASE7,
    APP_FASEFINAL,
    APP_PC_SCREEN_FINAL,
    APP_DEBUG
} AppState;

#define MAX_PERGUNTAS 4
#define MAX_PLAYER_NAME 26
extern int notasIA[MAX_PERGUNTAS];
extern char relatoriosIA[MAX_PERGUNTAS][512];
extern char gPlayerName[MAX_PLAYER_NAME];

// =====================
// TYPEWRITER EFFECT (TEXTO DIGITANDO)
// =====================
typedef struct {
    const char* text;   // Texto completo
    int length;         // strlen(text)
    int drawnChars;     // Quantos caracteres já foram "lidos"
    float accum;        // Acumulador de tempo para animar
    float speed;        // Letras por segundo
    bool done;          // Texto terminou?
} TypeWriter;

typedef struct {
    const char* text;
    int length;
    int drawnChars;
    float accum;
    float speed;
    bool done;
} TypeEraser;

void InitTypeWriter(TypeWriter* tw, const char* text, float speed);
void UpdateTypeWriter(TypeWriter* tw, float deltaTime, bool skip);
void SkipTypeWriter(TypeWriter* tw);
float UpdateFade(float dt, float duration, bool fadeIn);

void InitTypeEraser(TypeEraser* te, const char* text, float speed);
void UpdateTypeEraser(TypeEraser* te, float deltaTime, bool skip);
void SkipTypeEraser(TypeEraser* te);

// =====================
// DIALOGUE EQUIPE/QUIZ (OPÇÕES)
// =====================
typedef struct {
    const char* texto;
    bool correta;
    bool desabilitada; // Para controle de disables
} DialogueOption;

typedef struct {
    const char* pergunta_txt;
    DialogueOption* opcoes;
    int num_opcoes;
    int opcao_selecionada_usuario; // -1 se nenhuma
    bool respondeu;
    // Timer
    float timer_restante;  // Em segundos
    float timer_total;
    bool timer_ativo;
    bool timer_explodiu; // Cronômetro zerou?
} DialogueQuestion;

// Helpers Gerais
void InitDialogueQuestion(DialogueQuestion* dq, const char* pergunta_txt, DialogueOption* opcoes, int num_opcoes, float timer_total);
void UpdateDialogueQuestion(DialogueQuestion* dq, float deltaTime);
// Navegação via tecla: direction = +1 (baixo), -1 (cima)
void DialogueNavigateOptions(const DialogueQuestion* dq, int* selectedIndex, int direction);
// Clique: retorna índice da opção clicada ou -1 (use mesmo offsetX, offsetY etc do Draw)
int GetDialogueOptionClick(const DialogueQuestion* dq,
    int offsetX, int offsetY, int baseWidth, int rectHeight, int spacing, int larguraStep);
// Desenho visual: opções e timer
void DrawChronometer(float timer_restante, float timer_total, int x, int y, int radius);
// Desenha uma opção individual
void DrawDialogueOption(const DialogueOption* opt, Rectangle rec, bool selected, bool disabled, bool blink, Color base, Color txtCor);
// Desenha todas as opções:
// blinkWrong: índice da opção errada marcada (pisca vermelho), -1 se nenhuma
// blink: se true, ativa o efeito piscar
// blinkCorrect: índice da opção correta (pisca verde), -1 se nenhuma
void DrawAllDialogueOptions(const DialogueQuestion* dq, int selected,
    int offsetX, int offsetY,
    int baseWidth, int rectHeight, int spacing, int larguraStep,
    int blinkWrong, bool blink, int blinkCorrect);

void InitPlayerName(void);

#endif // GENERALFUNCTIONS_H