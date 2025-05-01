// generalFunctions.h
#pragma once

#include <stdbool.h>

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