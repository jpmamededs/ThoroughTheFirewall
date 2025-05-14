#include "transition_screen.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define TOTAL_DURATION         6.5f 
#define FADE_TIME              1.0f 
#define TITLE_START_TIME       0.3f 
#define TITLE_DURATION         1.5f 
#define DESC_START_GAP         0.2f 
#define DESC_DURATION          1.5f 
#define CURSOR_FREQ            0.6f
#define GLITCH_CHANCE_PCT      12 

static int desafio = 1;
static char descText[128] = "";
static float elapsed = 0.0f;
static bool done = false;
static Font font; 
static Sound whoosh;

static const int fontSize = 60;
static const int fontSizeDesc  = 32;

static float GetFadeAlpha(float t)
{
    if (t < FADE_TIME)
        return 1.0f - t / FADE_TIME; 
    if (t > TOTAL_DURATION - FADE_TIME)    
        return (t - (TOTAL_DURATION - FADE_TIME)) / FADE_TIME;
    return 0.0f;
}

static void DrawGlitch(int screenW, int screenH)
{
    if (GetRandomValue(0,99) >= GLITCH_CHANCE_PCT) return;

    int lines = GetRandomValue(2, 6);
    for (int i = 0; i < lines; ++i)
    {
        int y  = GetRandomValue(0, screenH);
        int lx = GetRandomValue(0, screenW);
        int lw = GetRandomValue(screenW / 8, screenW / 2);
        DrawRectangle(lx, y, lw, 2, (Color){0,255,0,80});
    }
}

void Init_TransitionScreen(int desafioNumero, const char *descricao)
{
    desafio  = desafioNumero;
    strncpy(descText, descricao ? descricao : "", sizeof(descText)-1);
    elapsed  = 0.0f;
    done     = false;

    font = LoadFont("src/fonts/SpecialElite-Regular.ttf");
    whoosh = LoadSound("src/music/som_transicao.mp3"); 
    PlaySound(whoosh);
}

void Update_TransitionScreen(void)
{
    elapsed += GetFrameTime();
    if (elapsed >= TOTAL_DURATION) done = true; 
}

void Draw_TransitionScreen(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    const int w = GetScreenWidth();
    const int h = GetScreenHeight();

    char fullTitle[32];
    snprintf(fullTitle, sizeof(fullTitle), "DESAFIO %d", desafio);
    size_t titleLen   = strlen(fullTitle);
    size_t titleChars = 0;

    if (elapsed > TITLE_START_TIME)
    {
        float t = (elapsed - TITLE_START_TIME) / TITLE_DURATION;
        if (t > 1.0f) t = 1.0f;
        titleChars = (size_t)floorf(t * titleLen);
    }

    char titleBuf[32] = {0};
    strncpy(titleBuf, fullTitle, titleChars);

    bool cursorOnTitle = (elapsed < TITLE_START_TIME + TITLE_DURATION) &&
                            (fmodf(elapsed, CURSOR_FREQ) < CURSOR_FREQ/2.0f);

    if (cursorOnTitle && titleChars < titleLen)
        snprintf(titleBuf + strlen(titleBuf), sizeof(titleBuf) - strlen(titleBuf), "_");

    Vector2 sizeTitle = MeasureTextEx(font, titleBuf, fontSize, 0);
    Vector2 posTitle  = { (w - sizeTitle.x)*0.5f, (h*0.5f) - sizeTitle.y };

    DrawTextEx(font, titleBuf, posTitle, fontSize, 0, GREEN);

    size_t descLen   = strlen(descText);
    size_t descChars = 0;

    float descStart = TITLE_START_TIME + TITLE_DURATION + DESC_START_GAP;
    if (elapsed > descStart)
    {
        float t = (elapsed - descStart) / DESC_DURATION;
        if (t > 1.0f) t = 1.0f;
        descChars = (size_t)floorf(t * descLen);
    }

    char descBuf[128] = {0};
    strncpy(descBuf, descText, descChars);

    bool cursorOnDesc = (elapsed > descStart) &&
                        (elapsed < descStart + DESC_DURATION) &&
                        (fmodf(elapsed, CURSOR_FREQ) < CURSOR_FREQ/2.0f);

    if (cursorOnDesc && descChars < descLen)
        snprintf(titleBuf + strlen(titleBuf), sizeof(titleBuf) - strlen(titleBuf), "_");

    Vector2 sizeDesc = MeasureTextEx(font, descBuf, fontSizeDesc, 0);
    Vector2 posDesc  = { (w - sizeDesc.x)*0.5f, posTitle.y + sizeTitle.y + 20 };

    DrawTextEx(font, descBuf, posDesc, fontSizeDesc, 0, GREEN);

    DrawGlitch(w, h);

    float alpha = GetFadeAlpha(elapsed);
    DrawRectangle(0, 0, w, h, (Color){0,0,0,(unsigned char)(alpha*255)});
    EndDrawing();
}

bool TransitionScreenDone(void)
{
    return done;
}

void Unload_TransitionScreen(void)
{
    UnloadFont(font);
    UnloadSound(whoosh);
}
