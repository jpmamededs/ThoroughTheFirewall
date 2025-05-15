#ifndef GENERALFUNCTIONS_H
#define GENERALFUNCTIONS_H

#include <stdbool.h>
#include "raylib.h"
#include <string.h>

typedef enum
{
    APP_CUTSCENES,
    APP_MENU,
    APP_LOADING_SCREEN,
    APP_INTRO,
    APP_LIGACAO_DESCONHECIDO,
    APP_FIREWALL,
    APP_PORTA_BATENDO,
    INTERROGATORIO,
    APP_TRANSICAO,
    APP_DESAFIO_01,
    APP_SERVIDOR_PROXY,
    APP_DESAFIO_02,
    APP_DESAFIO_03,
    APP_PENDRIVE,
    APP_BRUTEFORCE,
    APP_DESAFIO_04,
    APP_TEMPLATE_UBUNTU_01,
    APP_TEMPLATE_UBUNTU_02,
    APP_TEMPLATE_3D_01,
    APP_TEMPLATE_3D_02,
    APP_TEMPLATE_3D_03,
    APP_TELA_01,
    APP_TELA_02,
    APP_FASE4,
    APP_UBUNTU_PROVISORIO,
    APP_FASE6,
    APP_DEBUG
} AppState;

typedef struct {
    const char* text; int length; int drawnChars; float accum; float speed; bool done;
} TypeWriter;

typedef struct {
    const char* text; int length; int drawnChars; float accum; float speed; bool done;
} TypeEraser;

#define MAX_PERGUNTAS 4
#define MAX_PLAYER_NAME 26
extern int notasIA[MAX_PERGUNTAS];
extern char relatoriosIA[MAX_PERGUNTAS][512];
extern char relatorioGeralIA[1024];
extern char gPlayerName[MAX_PLAYER_NAME];
extern char gSelectedCharacterName[MAX_PLAYER_NAME];

void InitTypeWriter(TypeWriter* tw, const char* text, float speed);
void UpdateTypeWriter(TypeWriter* tw, float deltaTime, bool skip);
void SkipTypeWriter(TypeWriter* tw);
void InitTypeEraser(TypeEraser* te, const char* text, float speed);
void UpdateTypeEraser(TypeEraser* te, float deltaTime, bool skip);
void SkipTypeEraser(TypeEraser* te);
float UpdateFade(float dt, float duration, bool fadeIn);
void DrawChronometer(float timer_restante, float timer_total, int x, int y, int radius);
void DrawDica(float posX, float posY, const char *text);
int  SomarNotasIA(void);
void GerarRelatorioGeralIA(char *relatorioGeral, size_t tamanho);

#endif