#ifndef MENU_H
#define MENU_H
#include <stdbool.h>

void InitMenu(void);
void UpdateMenu(void);
void DrawMenu(void);
bool MenuStartGame(void);
void UnloadMenu(void);

// Função para obter o índice do personagem selecionado após a confirmação
int MenuSelectedCharacter(void);

// NOVO: função para obter o NOME do personagem selecionado após a confirmação
const char* MenuSelectedCharacterName(void);

#endif