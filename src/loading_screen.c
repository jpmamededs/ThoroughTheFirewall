#include "loading_screen.h"
#include "raylib.h"
#include <math.h>

// --- parâmetros ajustáveis -----------------------------------------------
#define FADE_TIME     1.0f           // seg. p/ fade-in e fade-out
#define RING_INNER    30.0f
#define RING_OUTER    40.0f
// --------------------------------------------------------------------------

static float totalDuration  = 0.0f;
static double startTime = 0.0;
static float elapsed        = 0.0f;
static float fadeAlpha      = 1.0f;
static float ringAngle      = 0.0f;
static bool  done           = false;

void Init_LoadingScreen(float durationSeconds)
{
    totalDuration = (durationSeconds < 2*FADE_TIME) ? 2*FADE_TIME : durationSeconds;
    startTime  = GetTime();
    elapsed       = 0.0f;
    fadeAlpha     = 1.0f;        // começa preto (fade-in)
    ringAngle     = 0.0f;
    done          = false;
}

void Update_LoadingScreen(void)
{
    float dt = GetFrameTime();
    ringAngle = fmodf(ringAngle + 360.0f * dt, 360.0f);

    elapsed = (float)(GetTime() - startTime);

    // ­­­­­­--- cálculo do fade ------------------------------------------------
    if (elapsed < FADE_TIME)
        fadeAlpha = 1.0f - (elapsed / FADE_TIME);                     // fade-in
    else if (elapsed > totalDuration - FADE_TIME)
        fadeAlpha = (elapsed - (totalDuration - FADE_TIME)) / FADE_TIME; // fade-out
    else
        fadeAlpha = 0.0f;

    if (elapsed >= totalDuration)
        done = true;
}

void Draw_LoadingScreen(void)
{
    BeginDrawing();

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    ClearBackground(BLACK);

    Vector2 center = { w * 0.5f, h * 0.5f };

    DrawRing(center, RING_INNER, RING_OUTER, 0, 360, 64, Fade(DARKGRAY, 0.4f));
    DrawRing(center, RING_INNER, RING_OUTER, ringAngle, ringAngle + 270, 64, RAYWHITE);

    if (fadeAlpha > 0.0f)
        DrawRectangle(0, 0, w, h, Fade(BLACK, fadeAlpha));

    EndDrawing();
}

void Unload_LoadingScreen(void)
{
    // nada a descarregar no momento
}

bool LoadingScreen_Done(void)
{
    return done;
}
