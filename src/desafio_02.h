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

#define BUTTON_SIZE        80
#define BUTTON_SPACING     18
#define BUTTON_ROW_SPACING 16
#define NUM_BUTTON_COLS    3
#define NUM_BUTTON_ROWS    3
#define MAX_TENTATIVAS 3
#define SEQ_TOTAL_PASSOS 4
#define FADEOUT_DURACAO 0.8f
#define GEMINI_RECT_PADRAO 550
#define GEMINI_PAD_X 36
#define FASE6_CHRONO_MAX 60.0f
#define FALA_NORMAL_02 "Essa senha está criptografada... Preciso voltar duas letras para decifrá-la. Não posso errar!"
#define FALA_VENCEU_02 "JOÃO! A senha está correta! Estou pronto para o próximo desafio."
#define FALA_PERDEU_02 "Não deu tempo... A criptografia me venceu. Hank vai me dispensar..."

#endif