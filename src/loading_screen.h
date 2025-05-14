#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include <stdbool.h>

void  Init_LoadingScreen(float durationSeconds);
void  Update_LoadingScreen(void);
void  Draw_LoadingScreen(void);
void  Unload_LoadingScreen(void);
bool  LoadingScreen_Done(void);

typedef struct {
    float x;
    float y;
    float speed;
} BinColumn;

#define FADE_TIME       1.0f
#define RING_INNER      30.0f
#define RING_OUTER      40.0f
#define BIN_FONT_SIZE   18       
#define BIN_SPEED_MIN   90.0f  
#define BIN_SPEED_MAX   160.0f
#define BIN_COL_SPACING 26    
#define LOGO_SCALE      0.6f
#define RING_GAP        40.0f 

#endif
