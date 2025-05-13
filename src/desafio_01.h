#ifndef FASE3_H
#define FASE3_H

#include <stdbool.h>

void Init_Desafio_01(void);
void Update_Desafio_01(void);
void Draw_Desafio_01(void);
void Unload_Desafio_01(void);
bool Fase_Desafio_01_Concluida(void);

#define NUM_SWITCHES 8
#define QUADRO_SHIFT_Y (-40)
#define MIN_Y_BASE 536
#define MAX_Y_BASE 700
#define RESPOSTA_MOSTRA_SEG 5.0f
#define LIMPEZA_TELA_RESULTADO 5.0f
#define GEMINI_RECT_PADRAO 550
#define GEMINI_PAD_X 36
#define FASE3_FADEOUT_DURACAO 0.8f
#define FALA_NORMAL "Preciso realocar os recursos para manter os sistemas críticos ativos. Não posso desligar os essenciais!"
#define FALA_ACERTO "Consegui! Os sistemas prioritários estão ativos e funcionando corretamente."
#define FALA_ERRO "Algo deu errado... Desativei um sistema essencial! Isso pode comprometer o funcionamento!"
#define FALA_JOAO_ACERTO "Tudo certo! Os recursos estão bem distribuídos e a operação está estável."
#define FALA_JOAO_ERRO "Droga! Desativei um sistema essencial! Isso pode atrapalhar o andamento da operação!"
#define FALA_CARLOS_ACERTO "Recursos alocados corretamente! A operação segue estável."
#define FALA_CARLOS_ERRO "Ih, desliguei um sistema importante! Isso vai prejudicar o funcionamento!"
#define FALA_MAMEDE_ACERTO "Boa! Mantive os recursos essenciais ativos e a operação estável."
#define FALA_MAMEDE_ERRO "Que problema! Desliguei algo crítico! A estabilidade ficou comprometida."
#define FALA_EXPLODIU "O que foi isso? Parece que algum sistema falhou! Preciso verificar os recursos novamente!"

#endif