#ifndef TRANSITION_SCREEN_H
#define TRANSITION_SCREEN_H

#include <stdbool.h>

void Init_TransitionScreen(int desafioNumero, const char *descricao);
void Update_TransitionScreen(void);
void Draw_TransitionScreen(void);
void Unload_TransitionScreen(void);
bool TransitionScreenDone(void);

#endif
