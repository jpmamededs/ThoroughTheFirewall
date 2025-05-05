#ifndef GENERALFUNCTIONS_H
#define GENERALFUNCTIONS_H

#include <stdbool.h>
#include "raylib.h" // Necessário para Tipos Color, Rectangle, etc

// =====================
// ENUM DE ESTADOS GERAIS
// =====================
typedef enum
{
    APP_CUTSCENES,
    APP_MENU,
    APP_INTRO,
    APP_FASE1,
    APP_FASE1_2,
    APP_PC_SCREEN,
    APP_FASE2,
    APP_FASE3,
    APP_FASEFINAL,
    INTERROGATORIO,
    APP_DEBUG
} AppState;

#define MAX_PERGUNTAS 4
extern int notasIA[MAX_PERGUNTAS];
extern char relatoriosIA[MAX_PERGUNTAS][512];

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

void InitTypeWriter(TypeWriter* tw, const char* text, float speed);
void UpdateTypeWriter(TypeWriter* tw, float deltaTime, bool skip);
void SkipTypeWriter(TypeWriter* tw);
float UpdateFade(float dt, float duration, bool fadeIn);

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

#endif // GENERALFUNCTIONS_H