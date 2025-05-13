#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include <stdbool.h>

void  Init_LoadingScreen(float durationSeconds);
void  Update_LoadingScreen(void);
void  Draw_LoadingScreen(void);
void  Unload_LoadingScreen(void);
bool  LoadingScreen_Done(void);

#endif
