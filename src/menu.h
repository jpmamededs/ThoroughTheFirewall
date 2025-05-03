#ifndef MENU_H
#define MENU_H

#include <stdbool.h>

// Inicializa o menu e carrega todos os recursos
void InitMenu(void);

// Atualiza a lógica do menu (entrada do usuário, animações, etc.)
void UpdateMenu(void);

// Desenha o menu na tela
void DrawMenu(void);

// Libera os recursos usados pelo menu
void UnloadMenu(void);

// Verifica se o jogador confirmou o menu e o jogo pode começar
bool MenuStartGame(void);

/**
 * Retorna o índice do personagem selecionado:
 * 0  -> João (default)
 * 1  -> Mateus
 * 2  -> Carlos
 * 3  -> Mamede
 * Caso nada tenha sido selecionado, retorna 0.
 */
int MenuSelectedCharacter(void);

/**
 * Retorna o nome do personagem selecionado:
 * “João”, “Mateus”, “Carlos”, “Mamede”
 * Caso nada tenha sido selecionado, retorna “João”.
 */
const char* MenuSelectedCharacterName(void);

#endif // MENU_H