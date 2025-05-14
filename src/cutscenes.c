#include "raylib.h"
#include "cutscenes.h"
#include <math.h>
// --- Variáveis globais de cutscene ---
static Texture2D empresa1, empresa2, empresa3, empresa4;
static Texture2D detetiveNaEmpresa4;
static Texture2D minhaTinta;
static Texture2D meuSelo;
static int screenWidth, screenHeight;
static float startTime = 0.0f;
static bool ended = false;

void InitCutscenes(void)
{
    screenWidth = GetMonitorWidth(0);
    screenHeight = GetMonitorHeight(0);
    empresa1        = LoadTexture("src/sprites/intro/empresa1.jpg");
    empresa2        = LoadTexture("src/sprites/intro/empresa2.jpg");
    empresa3        = LoadTexture("src/sprites/intro/empresa3.jpg");
    empresa4        = LoadTexture("src/sprites/intro/empresa4.jpg");
    detetiveNaEmpresa4 = LoadTexture("src/sprites/intro/detetiveNaEmpresa4.png");
    minhaTinta      = LoadTexture("src/sprites/intro/tinta.png");
    meuSelo         = LoadTexture("src/sprites/intro/selo.png");
    startTime = GetTime();
    ended = false;
}
void UpdateCutscenes(void)
{
    if (ended) return;

    if (IsKeyPressed(KEY_SPACE)) ended = true;

    float time = GetTime() - startTime;
    if (IsKeyPressed(KEY_SPACE) && time < 5.8f)
        startTime = GetTime() - 5.8f;
    if (time >= 16.3f && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)))
        ended = true;
}
void DrawEmpresa4DetetiveAnim(float animTime, int w, int h)
{
    const float animDuration = 1.2f;
    if (animTime < 0.0f) animTime = 0.0f;
    if (animTime > animDuration) animTime = animDuration;
    float t = animTime / animDuration;
    float easeT = t * t * (3.0f - 2.0f * t);
    float spriteW = detetiveNaEmpresa4.width;
    float spriteH = detetiveNaEmpresa4.height;
    float escala = (h/4.0f/spriteH) * 3.5f;
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xFinal = -15.0f;
    float xStart = xFinal - 15.0f;
    float xAtual = xStart + (xFinal - xStart) * easeT;
    float yFinal = h * 0.72f - dstH + 280.0f;
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (alpha > 0)
    {
        DrawTexturePro(
            detetiveNaEmpresa4,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xAtual, yFinal, dstW, dstH},
            (Vector2){0, 0},
            0.0f,
            (Color){255, 255, 255, alpha}
        );
    }
}
void DrawEmpresa4TintaAnimESelo(float showTime, int w, int h)
{
    const float tintaAnimDur = 1.1f;
    float tintaT = showTime / tintaAnimDur;
    if (tintaT > 1.0f) tintaT = 1.0f;
    if (tintaT < 0.0f) tintaT = 0.0f;
    float easeT = 1.0f - powf(1.0f-tintaT, 2.2f);
    float texW = minhaTinta.width;
    float texH = minhaTinta.height;
    float escala = (h/8.0f/texH)*11.0f;
    float dstW = texW * escala;
    float dstH = texH * escala;
    float xDest = w - dstW + 400.0f;
    float yDest = -410.0f;
    float revealedW = dstW * easeT;
    float revealedPixSrc = texW * easeT;
    unsigned char alpha = (unsigned char)(200 + 55 * easeT);
    if (tintaT > 0.0f)
    {
        DrawTexturePro(
            minhaTinta,
            (Rectangle){0, 0, revealedPixSrc, texH},
            (Rectangle){xDest, yDest, revealedW, dstH},
            (Vector2){0, 0},
            0.0f,
            (Color){255, 255, 255, alpha}
        );
    }
    if (tintaT >= 0.999f) {
        float selarAnimTime = showTime - tintaAnimDur;
        const float selarAnimDur = 0.42f;
        float carimboT = selarAnimTime / selarAnimDur;
        if (carimboT > 1.0f) carimboT = 1.0f;
        if (carimboT < 0.0f) carimboT = 0.0f;
        float easeA = carimboT * carimboT * (3.0f - 2.0f * carimboT);
        float spring = 1.05f - 0.10f * sinf(easeA * PI);
        float seloW = meuSelo.width;
        float seloH = meuSelo.height;
        float escalaSeloBase = (dstW < dstH ? dstW : dstH) * 0.28f / ((seloW > seloH) ? seloW : seloH);
        float escalaSelo = escalaSeloBase * (0.70f + 0.30f * easeA) * spring;
        if (escalaSelo > escalaSeloBase) escalaSelo = escalaSeloBase;
        float seloDstW = seloW * escalaSelo;
        float seloDstH = seloH * escalaSelo;
        float seloX = xDest + dstW/2.0f - seloDstW/2.0f;
        float seloY = yDest + dstH/2.0f - seloDstH/2.0f;
        unsigned char alphaSelo = (unsigned char)(255 * easeA);
        DrawTexturePro(
            meuSelo,
            (Rectangle){0,0,seloW,seloH},
            (Rectangle){seloX, seloY, seloDstW, seloDstH},
            (Vector2){0,0},
            0.0f,
            (Color){255,255,255,alphaSelo}
        );
    }
}
void DrawCutscenes(void)
{
    if (ended) return;
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float time = GetTime() - startTime;
    BeginDrawing();
    if (time < 2.0f)
    {
        ClearBackground(BLACK);
    }
    else if (time < 5.8f)
    {
        float panT = (time - 2.0f) / 3.8f;
        if (panT > 1.0f) panT = 1.0f;
        float panEase = panT * panT * (3.0f - 2.0f * panT);
        float panPixels = w * 0.07f;
        float destX = panPixels * panEase;
        DrawTexturePro(
            empresa1,
            (Rectangle){0, 0, empresa1.width, empresa1.height},
            (Rectangle){destX, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
        if (time < 3.4f)
        {
            float t = (time - 2.0f) / 1.4f;
            if (t > 1.0f) t = 1.0f;
            const int bars = 96;
            int barWidth = (w + bars - 1) / bars;
            for (int i = 0; i < bars; i++)
            {
                float barraInicio = (float)i / bars * 0.7f;
                float barraRandomOffset = (float)((i * 193) % 211) / 211.0f;
                barraInicio += barraRandomOffset * 0.09f;
                float barraT = (t - barraInicio) / 0.3f;
                if (barraT < 0) barraT = 0;
                if (barraT > 1) barraT = 1;
                barraT = barraT * barraT * (3.0f - 2.0f * barraT);
                int yReveal = (int)(h * barraT);
                DrawRectangle(i * barWidth, yReveal, barWidth, h - yReveal, BLACK);
            }
        }
    }
    else if (time < 5.93f)
    {
        ClearBackground(BLACK);
    }
    else if (time < 9.13f)
    {
        float showTime = time - 5.93f;
        float animDuration = 3.2f;
        float srcW = empresa2.width, srcH = empresa2.height;
        float zoomInicial = 2.3f;
        float centerYStart = 0.22f;
        float centerYEnd   = 0.5f;
        float centerX      = 0.5f;
        Rectangle srcRect;
        if (showTime < animDuration)
        {
            float t = showTime / animDuration;
            if (t > 1.0f) t = 1.0f;
            t = t * t * (3.0f - 2.0f * t);
            float currZoom = zoomInicial + (1.0f - zoomInicial) * t;
            float cropW = srcW / currZoom;
            float cropH = srcH / currZoom;
            float currCenterY = centerYStart + (centerYEnd - centerYStart) * t;
            float currCenterX = centerX;
            srcRect.x = srcW * currCenterX - cropW / 2.0f;
            srcRect.y = srcH * currCenterY - cropH / 2.0f;
            srcRect.width = cropW;
            srcRect.height = cropH;
            float fadeT = showTime / 0.29f;
            if (fadeT > 1.0f) fadeT = 1.0f;
            DrawTexturePro(
                empresa2,
                srcRect,
                (Rectangle){0, 0, w, h},
                (Vector2){0, 0},
                0.0f, WHITE
            );
            if (fadeT < 1.0f)
            {
                unsigned char fadeAlpha = (unsigned char)(255 * (1.0f - fadeT));
                DrawRectangle(0, 0, w, h, (Color){0, 0, 0, fadeAlpha});
            }
        }
        else
        {
            DrawTexturePro(
                empresa2,
                (Rectangle){0, 0, srcW, srcH},
                (Rectangle){0, 0, w, h},
                (Vector2){0, 0},
                0.0f, WHITE
            );
        }
    }
    // EMPRESA2 -> EMPRESA3: piscada preta simples
    else if (time < 9.31f)
    {
        ClearBackground(BLACK);
    }
    // PAN DA EMPRESA3 APÓS O FLASH
    else if (time < 14.13f)
    {
        float panT = (time - 9.31f) / 4.82f; // mantem proportional total pan (14.13-9.31)
        if (panT > 1.0f) panT = 1.0f;
        float panEase = panT * panT * (3.0f - 2.0f * panT);
        float srcW = empresa3.width;
        float srcH = empresa3.height;
        float dstW = (float)w;
        float dstH = (float)h;
        float scaleX = srcW / dstW;
        float scaleY = srcH / dstH;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        float cropW = dstW * scale;
        float cropH = dstH * scale;
        float cropXmin = 0;
        float cropXmax = srcW - cropW;
        float cropX = cropXmax * (1.0f - panEase) + cropXmin * (panEase);
        float cropY = (srcH - cropH) * 0.5f;
        DrawTexturePro(
            empresa3,
            (Rectangle){cropX, cropY, cropW, cropH},
            (Rectangle){0, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
    else if (time < 14.63f)
    {
        float srcW = empresa3.width;
        float srcH = empresa3.height;
        float dstW = (float)w;
        float dstH = (float)h;
        float scaleX = srcW / dstW;
        float scaleY = srcH / dstH;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        float cropW = dstW * scale;
        float cropH = dstH * scale;
        float cropX = 0;
        float cropY = (srcH - cropH) * 0.5f;
        DrawTexturePro(
            empresa3,
            (Rectangle){cropX, cropY, cropW, cropH},
            (Rectangle){0, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
    else if (time < 15.83f)
    {
        float glitchT = (time - 14.63f) / (15.83f - 14.63f);
        if (glitchT > 1.0f) glitchT = 1.0f;
        float srcW = empresa3.width;
        float srcH = empresa3.height;
        float dstW = (float)w;
        float dstH = (float)h;
        float scaleX = srcW / dstW;
        float scaleY = srcH / dstH;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        float cropW = dstW * scale;
        float cropH = dstH * scale;
        float cropX = 0;
        float cropY = (srcH - cropH) * 0.5f;
        DrawTexturePro(
            empresa3,
            (Rectangle){cropX, cropY, cropW, cropH},
            (Rectangle){0, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
        int glitches = 18 + (int)(28 * glitchT);
        for (int i = 0; i < glitches; i++)
        {
            int y = GetRandomValue(0, h-5);
            int hline = GetRandomValue(1, 10);
            DrawRectangle(0, y, w, hline, BLACK);
            if (glitchT > 0.3f) {
                int rx = GetRandomValue(0, w - 60);
                int rw = GetRandomValue(16, 90);
                int ry = GetRandomValue(0, h - 20);
                int rh = GetRandomValue(8, 30);
                DrawRectangle(rx, ry, rw, rh, BLACK);
            }
        }
        if (glitchT > 0.7f) {
            unsigned char fadeAlpha = (unsigned char)(255 * (glitchT-0.7f) / 0.3f);
            if (fadeAlpha > 255) fadeAlpha = 255;
            DrawRectangle(0,0,w,h,(Color){0,0,0,fadeAlpha});
        }
    }
    //--- EMPRESA4: Detetive/tinta só depois do fade concluído, sem flinque/piscada --
    else if (time < 16.3f)
    {
        float fadeInDuration = 0.47f;
        float timeSince = time - 15.83f;
        float fadeT = timeSince / fadeInDuration;
        if (fadeT > 1.0f) fadeT = 1.0f;
        if (fadeT < 0.0f) fadeT = 0.0f;
        float srcW = empresa4.width, srcH = empresa4.height;
        float scaleX = srcW / w;
        float scaleY = srcH / h;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        float dstW = w, dstH = h;
        float cropW = dstW * scale;
        float cropH = dstH * scale;
        float cropX = 0;
        float cropY = (srcH - cropH) * 0.5f;
        DrawTexturePro(
            empresa4,
            (Rectangle){cropX, cropY, cropW, cropH},
            (Rectangle){0, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
        unsigned char alpha = (unsigned char)(255 * (1.0f - fadeT));
        if (fadeT < 0.22f) {
            unsigned char flashAlpha = (unsigned char)(255 * (0.22f-fadeT)/0.22f);
            DrawRectangle(0,0,w,h, (Color){220,220,230,flashAlpha});
        } else {
            DrawRectangle(0,0,w,h,(Color){0,0,0,alpha});
        }
        float empresa4ShowTime = time - 15.83f;
        const float detetiveDur = 1.2f;
        const float tintaDelay = detetiveDur;
        // Detetive/tinta: só quando fade sumiu completamente!
        if (fadeT >= 1.0f)
        {
            float detetiveAnimTime = empresa4ShowTime;
            if (detetiveAnimTime < 0.0f) detetiveAnimTime = 0.0f;
            if (detetiveAnimTime > detetiveDur) detetiveAnimTime = detetiveDur;
            DrawEmpresa4DetetiveAnim(detetiveAnimTime, w, h);
            if (empresa4ShowTime >= tintaDelay)
                DrawEmpresa4TintaAnimESelo(empresa4ShowTime - tintaDelay, w, h);
        }
    }
    else
    {
        float empresa4ShowTime = time - 16.3f;
        const float detetiveDur = 1.2f;
        const float tintaDelay = detetiveDur;
        float srcW = empresa4.width;
        float srcH = empresa4.height;
        float dstW = (float)w;
        float dstH = (float)h;
        float scaleX = srcW / dstW;
        float scaleY = srcH / dstH;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        float cropW = dstW * scale;
        float cropH = dstH * scale;
        float cropX = 0;
        float cropY = (srcH - cropH) * 0.5f;
        DrawTexturePro(
            empresa4,
            (Rectangle){cropX, cropY, cropW, cropH},
            (Rectangle){0, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
        float detetiveAnimTime = empresa4ShowTime;
        if (detetiveAnimTime < 0.0f) detetiveAnimTime = 0.0f;
        if (detetiveAnimTime > detetiveDur) detetiveAnimTime = detetiveDur;
        DrawEmpresa4DetetiveAnim(detetiveAnimTime, w, h);
        if (empresa4ShowTime >= tintaDelay)
            DrawEmpresa4TintaAnimESelo(empresa4ShowTime - tintaDelay, w, h);
    }
    EndDrawing();
}
bool CutscenesEnded(void)
{
    return ended;
}
void UnloadCutscenes(void)
{
    UnloadTexture(empresa1);
    UnloadTexture(empresa2);
    UnloadTexture(empresa3);
    UnloadTexture(empresa4);
    UnloadTexture(detetiveNaEmpresa4);
    UnloadTexture(minhaTinta);
    UnloadTexture(meuSelo);
}