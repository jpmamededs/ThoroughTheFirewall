#ifndef MENU_H
#define MENU_H

#include <stdbool.h>

// Inicializa o menu e carrega os recursos
void InitMenu(void);

// Atualiza a lógica do menu (entrada do usuário, animações, etc.)
void UpdateMenu(void);

// Desenha o menu na tela
void DrawMenu(void);

// Libera os recursos usados pelo menu
void UnloadMenu(void);

// Verifica se o jogador confirmou o menu e o jogo pode começar
bool MenuStartGame(void);

// Retorna o índice do personagem selecionado
int MenuSelectedCharacter(void);

// Retorna o nome do personagem selecionado
const char* MenuSelectedCharacterName(void);

#endif
