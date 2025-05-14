#ifndef FASE6_H
#define FASE6_H

#include <stdbool.h>
#include "generalFunctions.h"
#include "menu.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

void Init_Desafio_02(void);
void Update_Desafio_02(void);
void Draw_Desafio_02(void);
void Unload_Desafio_02(void);
bool Fase_Desafio_02_Concluida(void);

typedef enum { TECLA1, TECLA2, TECLA3, TECLA4, TECLA5, TECLA6, TECLA7, TECLA8, TECLA9} TeclasQuadrado;
typedef enum { LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8 } LedQuadrado;
typedef enum { LED_ESTADO_APAGADO, LED_ESTADO_VERDE, LED_ESTADO_VERMELHO } LedEstadoCor;
typedef enum { SPRITE_NORMAL, SPRITE_VITORIA, SPRITE_DERROTA } SpriteStatus;

#endif