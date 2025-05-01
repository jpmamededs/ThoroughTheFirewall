#include "raylib.h"
#include "cutscenes.h"
static Music music;
static Sound welcomeSound;
static Texture2D sprite1, sprite2, russia, whiteHouse, hacker;
static Texture2D court, hackerGuy;
static int screenWidth, screenHeight;
static float time = 0.0f;
static float startTime = 0.0f;
static bool firstSpriteDone = false;
static bool welcomePlayed = false;
static bool ended = false;

void InitCutscenes(void)
{
    screenWidth = GetMonitorWidth(0);
    screenHeight = GetMonitorHeight(0);
    InitAudioDevice();
    music = LoadMusicStream("src/music/EisenfunkPong-[AudioTrimmer.com] (1).mp3");
    PlayMusicStream(music);
    welcomeSound = LoadSound("src/music/welcome-to-the-game-hacking-alert_sm4UxhuM.mp3");
    sprite1 = LoadTexture("src/sprites/cropped.png");
    sprite2 = LoadTexture("src/sprites/carinhaPdavida.png");
    russia = LoadTexture("src/sprites/russia.png");
    whiteHouse = LoadTexture("src/sprites/White_House.png");
    hacker = LoadTexture("src/sprites/hackerscenery.jpg");
    court = LoadTexture("src/sprites/courtscenery.jpeg");
    hackerGuy = LoadTexture("src/sprites/hacker.png");
    // matrix, jogoTexture e matrixFrames foram REMOVIDOS
    time = 0.0f;
    startTime = GetTime();
    firstSpriteDone = false;
    welcomePlayed = false;
    ended = false;
}

void UpdateCutscenes(void)
{
    if (ended) return;
    UpdateMusicStream(music);
    time = GetTime() - startTime;
    if (IsKeyPressed(KEY_SPACE) && time < 26.5f)
    {
        startTime = GetTime() - 26.5f;
        time = 26.5f;
    }
    if (time > 27.0f) ended = true;
}

void DrawCutscenes(void)
{
    if (ended) return;
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (time < 1.2f)
    {
        DrawTexturePro(russia, (Rectangle){0, 0, russia.width, russia.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, WHITE);
    }
    else if (time < 1.7f)
    {
        float alpha = (time - 1.2f) / 0.5f;
        Color fadeOut = (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))};
        Color fadeIn = (Color){255, 255, 255, (unsigned char)(255 * alpha)};
        DrawTexturePro(russia, (Rectangle){0, 0, russia.width, russia.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeOut);
        DrawTexturePro(whiteHouse, (Rectangle){0, 0, whiteHouse.width, whiteHouse.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeIn);
        DrawRectangle(0, 0, w, h, (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))});
    }
    else if (time < 2.9f)
    {
        DrawTexturePro(whiteHouse, (Rectangle){0, 0, whiteHouse.width, whiteHouse.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, WHITE);
    }
    else if (time < 3.4f)
    {
        float alpha = (time - 2.9f) / 0.5f;
        Color fadeOut = (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))};
        Color fadeIn = (Color){255, 255, 255, (unsigned char)(255 * alpha)};
        DrawTexturePro(whiteHouse, (Rectangle){0, 0, whiteHouse.width, whiteHouse.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeOut);
        DrawTexturePro(hacker, (Rectangle){0, 0, hacker.width, hacker.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeIn);
        DrawRectangle(0, 0, w, h, (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))});
    }
    else if (time < 4.6f)
    {
        DrawTexturePro(hacker, (Rectangle){0, 0, hacker.width, hacker.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, WHITE);
    }
    else if (time < 5.1f)
    {
        float alpha = (time - 4.6f) / 0.5f;
        Color fadeOut = (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))};
        Color fadeIn = (Color){255, 255, 255, (unsigned char)(255 * alpha)};
        DrawTexturePro(hacker, (Rectangle){0, 0, hacker.width, hacker.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeOut);
        DrawTexturePro(court, (Rectangle){0, 0, court.width, court.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeIn);
        DrawRectangle(0, 0, w, h, (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))});
    }
    else if (time < 6.3f)
    {
        DrawTexturePro(court, (Rectangle){0, 0, court.width, court.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, WHITE);
    }
    else if (time < 26.0f)
    {
        float animTime = time - 6.3f;
        Vector2 scale = {2.0f, 2.0f};
        float sprite1X = -sprite1.width * scale.x + (w + sprite1.width * scale.x) * (animTime / 10.0f);
        if (animTime >= 10.0f && !firstSpriteDone)
        {
            firstSpriteDone = true;
        }
        float sprite2X = w + sprite2.width * 3 - (w + sprite2.width * 3) * ((animTime - 2.0f) / 10.0f);
        Vector2 scale2 = {3.0f, 3.0f};
        Vector2 pos2 = {
            sprite2X,
            h / 2 - sprite2.height * scale2.y / 2 + 30};
        float hackerX = -hackerGuy.width * 1.5f + (w + hackerGuy.width * 1.5f) * ((animTime - 10.5f) / 10.0f);
        Vector2 scale3 = {1.2f, 1.2f};
        Vector2 pos3 = {
            hackerX,
            h / 2 - hackerGuy.height * scale3.y / 2};
        if (animTime >= 4.5f)
            DrawTextureEx(hackerGuy, pos3, 0.0f, scale3.x, WHITE);
        if (animTime >= 2.0f)
            DrawTextureEx(sprite2, pos2, 0.0f, scale2.x, WHITE);
        if (!firstSpriteDone)
            DrawTextureEx(sprite1, (Vector2){(int)sprite1X, (int)(h / 2 - sprite1.height / 2)}, 0.0f, scale.x, WHITE);
        DrawRectangle(0, 0, w, h / 4, BLACK);
        DrawRectangle(0, h - h / 4, w, h / 4, BLACK);
    }
    else if (time < 26.5f)
    {
        // Antes era Matrix + fade. Agora, só fade para preto.
        if (!welcomePlayed)
        {
            PlaySound(welcomeSound);
            welcomePlayed = true;
        }
        float alpha = (time - 26.0f) / 0.5f;
        if (alpha > 1.0f)
            alpha = 1.0f;
        DrawRectangle(0, 0, w, h, (Color){0, 0, 0, (unsigned char)(255 * alpha)});
    }
    else
    {
        // Só fundo preto
        ClearBackground(BLACK);
    }
    EndDrawing();
}

bool CutscenesEnded(void)
{
    return ended;
}

void UnloadCutscenes(void)
{
    UnloadMusicStream(music);
    UnloadSound(welcomeSound);
    CloseAudioDevice();
    UnloadTexture(sprite1);
    UnloadTexture(sprite2);
    UnloadTexture(russia);
    UnloadTexture(whiteHouse);
    UnloadTexture(hacker);
    UnloadTexture(court);
    UnloadTexture(hackerGuy);
}