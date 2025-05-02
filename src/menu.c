#include "menu.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#define FRAME_COUNT 15
#define MAX_COLUMNS 64

typedef struct CharacterNode {
    char name[32];
    struct CharacterNode *next, *prev;
} CharacterNode;

typedef enum {
    MENU_MAIN,
    MENU_SELECT_CHAR,
    MENU_FINISHED
} MenuScreen;

static MenuScreen currentScreen = MENU_MAIN;

// Recursos
static Texture2D backgroundMatrix;
static Texture2D logoTexture;
static Rectangle matrixFrames[FRAME_COUNT];
static int currentFrame = 0;
static float frameTime = 0.05f;
static float lastUpdate = 0.0f;
static float rainY[MAX_COLUMNS] = {0};
static bool matrixInitialized = false;

// Personagens
static CharacterNode *head = NULL, *selectedChar = NULL;
static int charCount = 0;

// Som
static Sound clickSound;
static bool wasHoveredLastFrame = false;

void PlayHoverSound(Rectangle btn, bool hoveredNow) {
    if (hoveredNow && !wasHoveredLastFrame) {
        PlaySound(clickSound);
    }
    wasHoveredLastFrame = hoveredNow;
}

void CreateCharacterList(void) {
    if (head) return;
    const char *names[] = {"João", "Mateus", "Carlos", "Mamede"};

    CharacterNode *last = NULL;
    for (int i = 0; i < 4; i++) {
        CharacterNode *node = malloc(sizeof(CharacterNode));
        strcpy(node->name, names[i]);

        if (!head) {
            head = node;
            node->next = node->prev = node;
        } else {
            node->prev = last;
            node->next = head;
            last->next = node;
            head->prev = node;
        }
        last = node;
    }

    selectedChar = head;
    charCount = 4;
}

void InitMenu(void) {
    currentScreen = MENU_MAIN;
    backgroundMatrix = LoadTexture("src/sprites/Matrix.png");
    logoTexture = LoadTexture("src/sprites/jogo.png");
    clickSound = LoadSound("src/music/buttonPress.mp3");

    Rectangle frames[] = {
        {0, 512, 512, 512}, {0, 1024, 512, 512}, {0, 1536, 512, 512},
        {512, 0, 512, 512}, {512, 512, 512, 512}, {512, 1024, 512, 512},
        {512, 1536, 512, 512}, {1024, 0, 512, 512}, {1024, 512, 512, 512},
        {1024, 1024, 512, 512}, {1024, 1536, 512, 512}, {1536, 0, 512, 512},
        {1536, 512, 512, 512}, {1536, 1024, 512, 512}, {1536, 1536, 512, 512}
    };
    for (int i = 0; i < FRAME_COUNT; i++) matrixFrames[i] = frames[i];

    CreateCharacterList();
    matrixInitialized = false;
    wasHoveredLastFrame = false;
}

void UpdateMenu(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float now = GetTime();

    if (now - lastUpdate >= frameTime) {
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        lastUpdate = now;
    }

    Vector2 mouse = GetMousePosition();

    if (currentScreen == MENU_MAIN) {
        Rectangle startBtn = {screenWidth / 2 - 150, screenHeight - 180, 300, 80};
        bool hovered = CheckCollisionPointRec(mouse, startBtn);
        PlayHoverSound(startBtn, hovered);

        if ((hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER)) {
            PlaySound(clickSound);
            currentScreen = MENU_SELECT_CHAR;
        }

    } else if (currentScreen == MENU_SELECT_CHAR) {
        int btnW = 160, btnH = 80, spacing = 20;
        int totalW = charCount * btnW + (charCount - 1) * spacing;
        int startX = (screenWidth - totalW) / 2;
        int y = screenHeight / 2 - btnH / 2;

        CharacterNode *node = head;
        for (int i = 0; i < charCount; i++, node = node->next) {
            Rectangle btn = {startX + i * (btnW + spacing), y, btnW, btnH};
            bool hovered = CheckCollisionPointRec(mouse, btn);
            PlayHoverSound(btn, hovered);

            if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                PlaySound(clickSound);
                selectedChar = node;
            }
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            PlaySound(clickSound);
            selectedChar = selectedChar->next;
        } else if (IsKeyPressed(KEY_LEFT)) {
            PlaySound(clickSound);
            selectedChar = selectedChar->prev;
        }

        Rectangle confirmBtn = {screenWidth / 2 - 130, y + btnH + 50, 260, 56};
        bool confirmHover = CheckCollisionPointRec(mouse, confirmBtn);
        PlayHoverSound(confirmBtn, confirmHover);

        if ((confirmHover && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER)) {
            PlaySound(clickSound);
            currentScreen = MENU_FINISHED;
        }
    }
}

void DrawMenu(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    BeginDrawing();
    ClearBackground(BLACK);

    float scale = 0.3f;
    int spriteW = 512 * scale;
    int spriteH = 512 * scale;
    int columns = screenWidth / spriteW + 2;

    if (!matrixInitialized) {
        for (int i = 0; i < MAX_COLUMNS; i++)
            rainY[i] = GetRandomValue(-spriteH, screenHeight);
        matrixInitialized = true;
    }

    float speed = 100.0f * GetFrameTime();
    Color green = (Color){0, 255, 0, 60};

    for (int i = 0; i < columns && i < MAX_COLUMNS; i++) {
        rainY[i] += speed;
        if (rainY[i] > screenHeight) rainY[i] = -spriteH;

        DrawTexturePro(backgroundMatrix, matrixFrames[currentFrame],
            (Rectangle){i * spriteW, rainY[i], spriteW, spriteH}, (Vector2){0, 0}, 0.0f, green);
    }

    if (currentScreen == MENU_MAIN) {
        float imgScale = 0.2f;
        float imgW = logoTexture.width * imgScale;
        float imgH = logoTexture.height * imgScale;
        DrawTextureEx(logoTexture, (Vector2){(screenWidth - imgW) / 2, (screenHeight - imgH) / 2}, 0.0f, imgScale, WHITE);

        Rectangle startBtn = {screenWidth / 2 - 150, screenHeight - 180, 300, 80};
        Color btnColor = CheckCollisionPointRec(GetMousePosition(), startBtn) ? DARKGREEN : GREEN;
        DrawRectangleRec(startBtn, btnColor);
        DrawText("Iniciar Jogo", startBtn.x + 50, startBtn.y + 20, 36, WHITE);
    }

    else if (currentScreen == MENU_SELECT_CHAR) {
        int spacing = 20, btnW = 160, btnH = 80;
        int totalW = charCount * btnW + (charCount - 1) * spacing;
        int startX = (screenWidth - totalW) / 2;
        int y = screenHeight / 2 - btnH / 2;

        CharacterNode *node = head;
        for (int i = 0; i < charCount; i++, node = node->next) {
            Rectangle btn = {startX + i * (btnW + spacing), y, btnW, btnH};
            Color color = (node == selectedChar) ? DARKGREEN : BLACK;
            if (CheckCollisionPointRec(GetMousePosition(), btn))
                color = (node == selectedChar) ? GREEN : DARKGRAY;

            DrawRectangleRec(btn, color);
            int textW = MeasureText(node->name, 36);
            DrawText(node->name, btn.x + (btnW - textW) / 2, btn.y + 20, 36, WHITE);
            DrawRectangleLinesEx(btn, 3, (node == selectedChar) ? RAYWHITE : DARKGRAY);
        }

        DrawText("Escolha seu personagem", screenWidth / 2 - 180, y - 70, 28, RAYWHITE);
        Rectangle confirmBtn = {screenWidth / 2 - 130, y + btnH + 50, 260, 56};
        Color confirmColor = CheckCollisionPointRec(GetMousePosition(), confirmBtn) ? DARKGREEN : GREEN;
        DrawRectangleRec(confirmBtn, confirmColor);
        DrawText("Confirmar", confirmBtn.x + 50, confirmBtn.y + 12, 32, WHITE);
    }

    EndDrawing();
}

bool MenuStartGame(void) {
    return currentScreen == MENU_FINISHED;
}

int MenuSelectedCharacter(void) {
    CharacterNode *node = head;
    int index = 0;
    while (node && node != selectedChar) {
        node = node->next;
        index++;
        if (node == head) break;
    }
    return index;
}

const char* MenuSelectedCharacterName(void) {
    return selectedChar ? selectedChar->name : "";
}

void UnloadMenu(void) {
    UnloadSound(clickSound);
    UnloadTexture(logoTexture);
    UnloadTexture(backgroundMatrix);
}
