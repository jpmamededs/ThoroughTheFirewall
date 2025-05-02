#include "raylib.h"
#include "cutscenes.h"
// Music NÃO deve ser usado aqui
static Sound welcomeSound;
static Texture2D sprite1, sprite2, menina, caraDeCostas, cabecaBranca;
static Texture2D russia, whiteHouse, hacker, court, hackerGuy;
static Texture2D escritorio; // <--- NOVO
static int screenWidth, screenHeight;
static float time = 0.0f;
static float startTime = 0.0f;
static bool welcomePlayed = false;
static bool ended = false;
void AnimateSpriteRightToLeft(Texture2D texture, float startAnimTime, float animTime, float duration, float scale, float offsetY)
{
    if (animTime < startAnimTime)
        return;
    float t = (animTime - startAnimTime) / duration;
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float x = w + texture.width * scale - (w + texture.width * scale) * t;
    Vector2 pos = {x, h / 2 - texture.height * scale / 2 + offsetY};
    DrawTextureEx(texture, pos, 0.0f, scale, WHITE);
}
void AnimateSpriteLeftToRight(Texture2D texture, float startAnimTime, float animTime, float duration, float scale, float offsetY)
{
    if (animTime < startAnimTime)
        return;
    float t = (animTime - startAnimTime) / duration;
    if (t > 1.0f)
        t = 1.0f;
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float x = -texture.width * scale + (w + texture.width * scale) * t;
    Vector2 pos = {x, h / 2 - texture.height * scale / 2 + offsetY};
    DrawTextureEx(texture, pos, 0.0f, scale, WHITE);
}
void InitCutscenes(void)
{
    screenWidth = GetMonitorWidth(0);
    screenHeight = GetMonitorHeight(0);
    // NÃO chame InitAudioDevice aqui!
    // NÃO carregue música aqui!
    welcomeSound = LoadSound("src/music/welcome-to-the-game-hacking-alert_sm4UxhuM.mp3");
    sprite1 = LoadTexture("src/sprites/cropped.png");
    sprite2 = LoadTexture("src/sprites/carinhaPdavida.png");
    menina = LoadTexture("src/sprites/menina.png");
    caraDeCostas = LoadTexture("src/sprites/caraDeCostas.png");
    cabecaBranca = LoadTexture("src/sprites/cabecaBranca.png");
    russia = LoadTexture("src/sprites/russia.png");
    whiteHouse = LoadTexture("src/sprites/White_House.png");
    hacker = LoadTexture("src/sprites/hackerscenery.jpg");
    court = LoadTexture("src/sprites/courtscenery.jpeg");
    hackerGuy = LoadTexture("src/sprites/hacker.png");
    escritorio = LoadTexture("src/sprites/escritorio.png"); // <--- NOVO
    time = 0.0f;
    startTime = GetTime();
    welcomePlayed = false;
    ended = false;
}
void UpdateCutscenes(void)
{
    if (ended)
        return;
    // NÃO chame UpdateMusicStream aqui!
    time = GetTime() - startTime;
    if (IsKeyPressed(KEY_SPACE) && time < 26.5f)
    {
        startTime = GetTime() - 26.5f;
        time = 26.5f;
    }
    if (time > 27.0f)
        ended = true;
}
void DrawCutscenes(void)
{
    if (ended)
        return;
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    BeginDrawing();
    // ClearBackground(RAYWHITE); // REMOVIDO

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
    // Aqui trocamos o fundo:
    else if (time < 26.0f)
    {
        // DESENHA O FUNDO DO ESCRITÓRIO ANTES DE TUDO
        float animStart = 6.3f;
        float animEnd = 28.5f; // animação prolongada
        float animDuration = animEnd - animStart;
        float animElapsed = time - animStart;
        float scale = 2.0f;

        if (animElapsed < 0.0f)
            animElapsed = 0.0f;
        if (animElapsed > animDuration)
            animElapsed = animDuration;

        float t = animElapsed / animDuration;

        // Largura da imagem escalada
        float texWidthScaled = escritorio.width * scale;

        // A posição inicial mostra o canto direito da imagem
        // A posição final mostra o canto esquerdo
        float startX = screenWidth - texWidthScaled;
        float endX = 0.0f;

        float x = startX + (endX - startX) * t;

        DrawTextureEx(escritorio, (Vector2){x, 0}, 0.0f, scale, WHITE);
        float animTime = time - 6.3f;
        AnimateSpriteRightToLeft(cabecaBranca, 13.5f, animTime, 10.0f, 1.0f, 30);
        AnimateSpriteRightToLeft(menina, 11.5f, animTime, 10.0f, 1.0f, 30);
        AnimateSpriteLeftToRight(caraDeCostas, 12.5f, animTime, 10.0f, 2.0f, 0);
        AnimateSpriteRightToLeft(sprite2, 2.0f, animTime, 10.0f, 3.0f, 30);
        AnimateSpriteLeftToRight(hackerGuy, 10.5f, animTime, 10.0f, 1.2f, 0);
        AnimateSpriteLeftToRight(sprite1, 0.0f, animTime, 10.0f, 2.0f, 240);
        DrawRectangle(0, 0, w, h / 4, BLACK);
        DrawRectangle(0, h - h / 4, w, h / 4, BLACK);
    }
    else if (time < 26.5f)
    {
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
    UnloadSound(welcomeSound);
    UnloadTexture(sprite1);
    UnloadTexture(sprite2);
    UnloadTexture(menina);
    UnloadTexture(caraDeCostas);
    UnloadTexture(cabecaBranca);
    UnloadTexture(russia);
    UnloadTexture(whiteHouse);
    UnloadTexture(hacker);
    UnloadTexture(court);
    UnloadTexture(hackerGuy);
    UnloadTexture(escritorio); // <--- NOVO
}