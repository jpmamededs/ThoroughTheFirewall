#include "menu.h"
#include "raylib.h"
#include <math.h> // Para GetRandomValue

#define FRAME_COUNT 15
#define MAX_COLS 64

static bool startedGame = false;

// Matrix effect stuff
static Texture2D matrixSprite;
static Texture2D jogoTexture;
static Rectangle matrixFrames[FRAME_COUNT];
static int currentFrame = 0;
static float frameDuration = 0.05f;
static float lastFrameTime = 0.0f;
static float spriteY[MAX_COLS] = {0};
static bool matrixInited = false;

void InitMenu(void)
{
    startedGame = false;
    matrixSprite = LoadTexture("src/sprites/Matrix.png");
    jogoTexture = LoadTexture("src/sprites/jogo.png");
    Rectangle frames[FRAME_COUNT] = {
        {0, 512, 512, 512}, {0, 1024, 512, 512}, {0, 1536, 512, 512},
        {512, 0, 512, 512}, {512, 512, 512, 512}, {512, 1024, 512, 512},
        {512, 1536, 512, 512}, {1024, 0, 512, 512}, {1024, 512, 512, 512},
        {1024, 1024, 512, 512}, {1024, 1536, 512, 512}, {1536, 0, 512, 512},
        {1536, 512, 512, 512}, {1536, 1024, 512, 512}, {1536, 1536, 512, 512}
    };
    for (int i = 0; i < FRAME_COUNT; i++) matrixFrames[i] = frames[i];
    currentFrame = 0;
    frameDuration = 0.05f;
    lastFrameTime = 0.0f;
    matrixInited = false;
}

void UpdateMenu(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    int btnW = 300;
    int btnH = 80;
    int btnX = w / 2 - btnW / 2;
    int btnY = h - btnH - 100;
    Rectangle btnRect = {btnX, btnY, btnW, btnH};
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, btnRect);

    // Animação Matrix
    float now = GetTime();
    if (now - lastFrameTime >= frameDuration)
    {
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        lastFrameTime = now;
    }

    if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        startedGame = true;
    }
}

void DrawMenu(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    BeginDrawing();
    ClearBackground(BLACK);

    // MATRIX: efeito de chuva
    float scale = 0.3f;
    int spriteW = 512 * scale;
    int spriteH = 512 * scale;
    int numCols = w / spriteW + 2;
    if (!matrixInited) {
        for (int i = 0; i < MAX_COLS; i++)
            spriteY[i] = GetRandomValue(-spriteH, h);
        matrixInited = true;
    }
    float speed = 100.0f * GetFrameTime();
    for (int i = 0; i < numCols && i < MAX_COLS; i++)
    {
        spriteY[i] += speed;
        if (spriteY[i] > h)
            spriteY[i] = -spriteH;
        float x = i * spriteW;
        float y = spriteY[i];
        DrawTexturePro(matrixSprite, matrixFrames[currentFrame],
                       (Rectangle){x, y, spriteW, spriteH},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
    // Sprite central
    float imgScale = 0.2f;
    float imgWidth = jogoTexture.width * imgScale;
    float imgHeight = jogoTexture.height * imgScale;
    float posX = (w - imgWidth) / 2;
    float posY = (h - imgHeight) / 2;
    DrawTextureEx(jogoTexture, (Vector2){posX, posY}, 0.0f, imgScale, WHITE);

    // Botão iniciar
    int btnW = 300;
    int btnH = 80;
    int btnX = w / 2 - btnW / 2;
    int btnY = h - btnH - 100;
    Rectangle btnRect = {btnX, btnY, btnW, btnH};
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, btnRect);
    Color btnColor = (hovered ? (Color){0, 200, 0, 255} : (Color){0, 180, 0, 255});
    DrawRectangleRec(btnRect, btnColor);

    const char *text = "Iniciar jogo";
    int fontSize = 36;
    int textW = MeasureText(text, fontSize);
    int textH = fontSize;
    int textX = btnX + (btnW - textW)/2;
    int textY = btnY + (btnH - textH)/2;
    DrawText(text, textX, textY, fontSize, WHITE);

    EndDrawing();
}

bool MenuStartGame(void)
{
    return startedGame;
}

void UnloadMenu(void)
{
    UnloadTexture(jogoTexture);
    UnloadTexture(matrixSprite);
    // Nada mais a liberar
}