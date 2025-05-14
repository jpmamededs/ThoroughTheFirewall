#include "loading_screen.h"
#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

static float totalDuration  = 0.0f;
static double startTime = 0.0;
static float elapsed = 0.0f;
static float fadeAlpha = 1.0f;
static float ringAngle = 0.0f;
static bool done = false;
static BinColumn *columns = NULL;
static int columnCount = 0;
static Texture2D logo;

static void InitBinaryRain(void)
{
    int w = GetScreenWidth();
    columnCount = w / BIN_COL_SPACING + 1;
    columns = (BinColumn*)MemAlloc(columnCount * sizeof(BinColumn));

    for (int i = 0; i < columnCount; ++i)
    {
        columns[i].x     = i * BIN_COL_SPACING;
        columns[i].y     = -GetRandomValue(0, GetScreenHeight());
        columns[i].speed = GetRandomValue(BIN_SPEED_MIN, BIN_SPEED_MAX);
    }
}

static void UpdateBinaryRain(float dt)
{
    int h = GetScreenHeight();

    for (int i = 0; i < columnCount; ++i)
    {
        columns[i].y += columns[i].speed * dt;

        if (columns[i].y > h + BIN_FONT_SIZE)
        {
            columns[i].y     = -GetRandomValue(0, h);
            columns[i].speed = GetRandomValue(BIN_SPEED_MIN, BIN_SPEED_MAX);
        }
    }
}

static void DrawBinaryRain(void)
{
    int h = GetScreenHeight();
    int rowsPerCol = h / BIN_FONT_SIZE + 2;

    for (int i = 0; i < columnCount; ++i)
    {
        float yHead = columns[i].y;

        for (int r = 0; r < rowsPerCol; ++r)
        {
            float y = yHead - r * BIN_FONT_SIZE;
            if (y < -BIN_FONT_SIZE) break;

            char bit = ((i + r + (int)elapsed) & 1) ? '1' : '0';
            Color bitColor = (r == 0) ? Fade(GREEN, 0.5f) : Fade(DARKGREEN, 0.5f);
            DrawTextEx(
                GetFontDefault(),
                (char[2]){bit, 0},
                (Vector2){columns[i].x, y},
                (float)BIN_FONT_SIZE, 1.0f, bitColor
            );
        }
    }
}

void Init_LoadingScreen(float durationSeconds)
{
    totalDuration = (durationSeconds < 2*FADE_TIME) ? 2*FADE_TIME : durationSeconds;
    startTime  = GetTime();
    elapsed       = 0.0f;
    fadeAlpha     = 1.0f;
    ringAngle     = 0.0f;
    done          = false;

    srand((unsigned int)time(NULL));
    InitBinaryRain();

    logo = LoadTexture("src/sprites/logo_jogo.png");
}

void Update_LoadingScreen(void)
{
    float dt = GetFrameTime();
    ringAngle = fmodf(ringAngle + 360.0f * dt, 360.0f);

    UpdateBinaryRain(dt);

    elapsed = (float)(GetTime() - startTime);

    if (elapsed < FADE_TIME)
        fadeAlpha = 1.0f - (elapsed / FADE_TIME);
    else if (elapsed > totalDuration - FADE_TIME)
        fadeAlpha = (elapsed - (totalDuration - FADE_TIME)) / FADE_TIME;
    else
        fadeAlpha = 0.0f;

    if (elapsed >= totalDuration) done = true;
}

void Draw_LoadingScreen(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    // CHUVA DE BINARIOS
    DrawBinaryRain();

    // LOGO
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float logoW = logo.width  * LOGO_SCALE;
    float logoH = logo.height * LOGO_SCALE;
    Vector2 logoPos = { (w - logoW) * 0.5f, (h - logoH) * 0.5f - logoH*0.25f };
    DrawTextureEx(logo, logoPos, 0.0f, LOGO_SCALE, WHITE);

    // ANEL
    Vector2 ringCenter = { w * 0.5f, logoPos.y + logoH + RING_GAP + RING_OUTER };
    DrawRing(ringCenter, RING_INNER, RING_OUTER, 0, 360, 64, Fade(DARKGRAY, 0.4f));
    DrawRing(ringCenter, RING_INNER, RING_OUTER, ringAngle, ringAngle + 270, 64, RAYWHITE);

    if (fadeAlpha > 0.0f) DrawRectangle(0, 0, w, h, Fade(BLACK, fadeAlpha));

    EndDrawing();
}

void Unload_LoadingScreen(void)
{
    if (columns) { MemFree(columns); columns = NULL; }
    UnloadTexture(logo);
}

bool LoadingScreen_Done(void)
{
    return done;
}
