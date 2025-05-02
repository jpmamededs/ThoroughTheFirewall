#include "menu.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#define FRAME_COUNT 15
#define MAX_COLUMNS 64

typedef struct CharacterNode
{
    char name[32];
    struct CharacterNode *next, *prev;
} CharacterNode;

typedef enum
{
    MENU_MAIN,
    MENU_SELECT_CHAR,
    MENU_FINISHED
} MenuScreen;

static MenuScreen currentScreen = MENU_MAIN;

// Recursos
static Texture2D backgroundMatrix;
static Texture2D logoTexture;
static Texture2D cabecaBranca;
static Texture2D meninaSheet;
static Texture2D cabeloCastanhoSheet;
static Texture2D hacker1;
static Texture2D hacker2;
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

static void PlayHoverSound(Rectangle btn, bool hoveredNow)
{
    if (hoveredNow && !wasHoveredLastFrame)
    {
        PlaySound(clickSound);
    }
    wasHoveredLastFrame = hoveredNow;
}

static void DrawCharacterButtonContent(CharacterNode *node, Rectangle btn, bool isHovered, bool isSelected)
{
    Rectangle src = {0, 0, 528, 528}; // padrão
    Vector2 pos;
    Texture2D texture;

    if (strcmp(node->name, "Mateus") == 0)
    {
        src = (isHovered || isSelected) ? (Rectangle){550, 7, 530, 525} : (Rectangle){7, 7, 525, 525};
        texture = cabecaBranca;
    }
    else if (strcmp(node->name, "João") == 0)
    {
        src = (isHovered || isSelected) ? (Rectangle){550, 7, 530, 525} : (Rectangle){7, 7, 525, 525};
        texture = meninaSheet;
    }
    else if (strcmp(node->name, "Mamede") == 0)
    {
        src = (isHovered || isSelected) ? (Rectangle){840, 4, 840, 540.4f} : (Rectangle){4, 4, 840, 540.4f};
        texture = cabeloCastanhoSheet;
    }
    else if (strcmp(node->name, "Carlos") == 0)
    {
        texture = (isHovered || isSelected) ? hacker2 : hacker1;
        src = (Rectangle){0, 0, texture.width, texture.height};
    }
    else
    {
        int textW = MeasureText(node->name, 36);
        DrawText(node->name, btn.x + (btn.width - textW) / 2, btn.y + 20, 36, WHITE);
        return;
    }

    // Escala proporcional com base na altura do botão
    float scale = btn.height * 0.85f / src.height;
    float spriteW = src.width * scale;
    float spriteH = src.height * scale;

    pos = (Vector2){
        btn.x + (btn.width - spriteW) / 2,
        btn.y + (btn.height - spriteH) / 2};

    DrawTexturePro(texture, src,
                   (Rectangle){pos.x, pos.y, spriteW, spriteH},
                   (Vector2){0, 0}, 0.0f, WHITE);
}

void CreateCharacterList(void)
{
    if (head)
        return;
    const char *names[] = {"João", "Mateus", "Carlos", "Mamede"};

    CharacterNode *last = NULL;
    for (int i = 0; i < 4; i++)
    {
        CharacterNode *node = malloc(sizeof(CharacterNode));
        strcpy(node->name, names[i]);

        if (!head)
        {
            head = node;
            node->next = node->prev = node;
        }
        else
        {
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

void InitMenu(void)
{
    currentScreen = MENU_MAIN;
    backgroundMatrix = LoadTexture("src/sprites/Matrix.png");
    logoTexture = LoadTexture("src/sprites/jogo.png");
    cabecaBranca = LoadTexture("src/sprites/cabecaBrancaSheet.png");
    meninaSheet = LoadTexture("src/sprites/meninaSheet.png");
    cabeloCastanhoSheet = LoadTexture("src/sprites/cabeloCastanhoSheet.png");
    hacker1 = LoadTexture("src/sprites/hacker1.png");
    hacker2 = LoadTexture("src/sprites/hacker2.png");
    clickSound = LoadSound("src/music/buttonPress.wav");

    Rectangle frames[] = {
        {0, 512, 512, 512}, {0, 1024, 512, 512}, {0, 1536, 512, 512}, {512, 0, 512, 512}, {512, 512, 512, 512}, {512, 1024, 512, 512}, {512, 1536, 512, 512}, {1024, 0, 512, 512}, {1024, 512, 512, 512}, {1024, 1024, 512, 512}, {1024, 1536, 512, 512}, {1536, 0, 512, 512}, {1536, 512, 512, 512}, {1536, 1024, 512, 512}, {1536, 1536, 512, 512}};
    for (int i = 0; i < FRAME_COUNT; i++)
        matrixFrames[i] = frames[i];

    CreateCharacterList();
    matrixInitialized = false;
    wasHoveredLastFrame = false;
}

void UpdateMenu(void)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float now = GetTime();

    if (now - lastUpdate >= frameTime)
    {
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        lastUpdate = now;
    }

    Vector2 mouse = GetMousePosition();

    if (currentScreen == MENU_MAIN)
    {
        Rectangle startBtn = {screenWidth / 2 - 150, screenHeight - 180, 300, 80};
        bool hovered = CheckCollisionPointRec(mouse, startBtn);
        PlayHoverSound(startBtn, hovered);

        if ((hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))
        {
            PlaySound(clickSound);
            currentScreen = MENU_SELECT_CHAR;
        }
    }
    else if (currentScreen == MENU_SELECT_CHAR)
    {
        int btnW = 240, btnH = 240, spacing = 30;
        int totalW = charCount * btnW + (charCount - 1) * spacing;
        int startX = (screenWidth - totalW) / 2;
        int y = screenHeight / 2 - btnH / 2;

        CharacterNode *node = head;
        for (int i = 0; i < charCount; i++, node = node->next)
        {
            Rectangle btn = {startX + i * (btnW + spacing), y, btnW, btnH};
            bool hovered = CheckCollisionPointRec(mouse, btn);
            PlayHoverSound(btn, hovered);

            if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                PlaySound(clickSound);
                selectedChar = node;
            }
        }

        if (IsKeyPressed(KEY_RIGHT))
        {
            PlaySound(clickSound);
            selectedChar = selectedChar->next;
        }
        else if (IsKeyPressed(KEY_LEFT))
        {
            PlaySound(clickSound);
            selectedChar = selectedChar->prev;
        }

        Rectangle confirmBtn = {screenWidth / 2 - 130, y + btnH + 50, 260, 56};
        bool confirmHover = CheckCollisionPointRec(mouse, confirmBtn);
        PlayHoverSound(confirmBtn, confirmHover);

        if ((confirmHover && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))
        {
            PlaySound(clickSound);
            currentScreen = MENU_FINISHED;
        }
    }
}

void DrawMenu(void)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    BeginDrawing();
    ClearBackground(BLACK);

    float scale = 0.3f;
    int spriteW = 512 * scale;
    int spriteH = 512 * scale;
    int columns = screenWidth / spriteW + 2;

    if (!matrixInitialized)
    {
        for (int i = 0; i < MAX_COLUMNS; i++)
            rainY[i] = GetRandomValue(-spriteH, screenHeight);
        matrixInitialized = true;
    }

    float speed = 100.0f * GetFrameTime();
    Color green = (Color){0, 255, 0, 60};

    for (int i = 0; i < columns && i < MAX_COLUMNS; i++)
    {
        rainY[i] += speed;
        if (rainY[i] > screenHeight)
            rainY[i] = -spriteH;

        DrawTexturePro(backgroundMatrix, matrixFrames[currentFrame],
                       (Rectangle){i * spriteW, rainY[i], spriteW, spriteH}, (Vector2){0, 0}, 0.0f, green);
    }

    if (currentScreen == MENU_MAIN)
    {
        float imgScale = 0.2f;
        float imgW = logoTexture.width * imgScale;
        float imgH = logoTexture.height * imgScale;
        DrawTextureEx(logoTexture, (Vector2){(screenWidth - imgW) / 2, (screenHeight - imgH) / 2}, 0.0f, imgScale, WHITE);

        Rectangle startBtn = {screenWidth / 2 - 150, screenHeight - 180, 300, 80};
        Color btnColor = CheckCollisionPointRec(GetMousePosition(), startBtn) ? DARKGREEN : GREEN;
        DrawRectangleRec(startBtn, btnColor);
        DrawText("Iniciar Jogo", startBtn.x + 50, startBtn.y + 20, 36, WHITE);
    }
    else if (currentScreen == MENU_SELECT_CHAR)
    {
        int spacing = 30, btnW = 240, btnH = 240;
        int totalW = charCount * btnW + (charCount - 1) * spacing;
        int startX = (screenWidth - totalW) / 2;
        int y = screenHeight / 2 - btnH / 2;

        CharacterNode *node = head;
        for (int i = 0; i < charCount; i++, node = node->next)
        {
            Rectangle btn = {startX + i * (btnW + spacing), y, btnW, btnH};
            bool isHovered = CheckCollisionPointRec(GetMousePosition(), btn);
            bool isSelected = (node == selectedChar);

            Color bgColor = isSelected ? DARKGREEN : BLACK;
            if (isHovered)
                bgColor = isSelected ? GREEN : DARKGRAY;
            DrawRectangleRec(btn, bgColor);
            DrawRectangleLinesEx(btn, 3, isSelected ? RAYWHITE : DARKGRAY);

            DrawCharacterButtonContent(node, btn, isHovered, isSelected);
        }

        DrawText("Escolha seu personagem", screenWidth / 2 - 180, y - 70, 28, RAYWHITE);
        Rectangle confirmBtn = {screenWidth / 2 - 130, y + btnH + 50, 260, 56};
        Color confirmColor = CheckCollisionPointRec(GetMousePosition(), confirmBtn) ? DARKGREEN : GREEN;
        DrawRectangleRec(confirmBtn, confirmColor);
        DrawText("Confirmar", confirmBtn.x + 50, confirmBtn.y + 12, 32, WHITE);
    }

    EndDrawing();
}

bool MenuStartGame(void)
{
    return currentScreen == MENU_FINISHED;
}

int MenuSelectedCharacter(void)
{
    CharacterNode *node = head;
    int index = 0;
    while (node && node != selectedChar)
    {
        node = node->next;
        index++;
        if (node == head)
            break;
    }
    return index;
}

const char *MenuSelectedCharacterName(void)
{
    return selectedChar ? selectedChar->name : "";
}

void UnloadMenu(void)
{
    UnloadSound(clickSound);
    UnloadTexture(logoTexture);
    UnloadTexture(backgroundMatrix);
    UnloadTexture(cabecaBranca);
    UnloadTexture(meninaSheet);
    UnloadTexture(cabeloCastanhoSheet);
    UnloadTexture(hacker1);
    UnloadTexture(hacker2);
}
