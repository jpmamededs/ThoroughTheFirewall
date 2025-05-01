#ifndef INTRO_H
#define INTRO_H

#include <stdbool.h>

// Inicializa a tela de introdução, usando o nome do personagem selecionado
void InitIntro(const char* nomePersonagem);

// Atualiza a lógica de apresentação da introdução (efeito de texto, input, etc)
void UpdateIntro(void);

// Desenha a tela de introdução (caixa, texto gradativo)
void DrawIntro(void);

// Retorna true se o usuário terminou e confirmou para seguir adiante
bool IntroEnded(void);

// Libera recursos alocados pela introdução
void UnloadIntro(void);

#endif // INTRO_H