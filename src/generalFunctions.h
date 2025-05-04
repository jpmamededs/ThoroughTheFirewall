// generalFunctions.h
#pragma once

#include <stdbool.h>

typedef enum
{
    APP_CUTSCENES,
    APP_MENU,
    APP_INTRO,
    APP_FASE1,
    APP_FASE1_2,
    APP_PC_SCREEN,
    APP_FASE2,
    INTERROGATORIO,
    APP_DEBUG
} AppState;

typedef struct {
    const char* text;   // Texto completo (não gerencia o buffer, só referencia)
    int length;         // strlen(text)
    int drawnChars;     // Quantos caracteres já foram "lidos"
    float accum;        // Acumulador de tempo
    float speed;        // Leitura: letras por segundo
    bool done;          // Terminou?
} TypeWriter;

void InitTypeWriter(TypeWriter* tw, const char* text, float speed);
void UpdateTypeWriter(TypeWriter* tw, float deltaTime, bool skip);
void SkipTypeWriter(TypeWriter* tw);