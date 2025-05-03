#include "menu.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#define FRAME_COUNT 15
#define MAX_COLUMNS 64
#define SPRITE_SRC_WIDTH 480
#define SPRITE_SRC_HEIGHT 960
#define SPRITE_SCALE 0.6f
#define SPRITE_BTN_WIDTH (SPRITE_SRC_WIDTH * SPRITE_SCALE)
#define SPRITE_BTN_HEIGHT (SPRITE_SRC_HEIGHT * SPRITE_SCALE)

typedef struct CharacterNode
{
    char name[32];
    struct CharacterNode *next, *prev;
    Sound sfx;
} CharacterNode;

typedef enum
{
    MENU_MAIN,
    MENU_SELECT_CHAR,
    MENU_FINISHED
} MenuScreen;

static MenuScreen currentScreen = MENU_MAIN;
static Texture2D backgroundMatrix;
static Texture2D logoTexture;
static Texture2D hacker1, hacker2;
static Texture2D menina1, menina2;
static Texture2D meninoPdavida1, meninoPdavida2;
static Texture2D deBone1, deBone2;
static Rectangle matrixFrames[FRAME_COUNT];
static int currentFrame = 0;
static float frameTime = 0.05f;
static float lastUpdate = 0.0f;
static float rainY[MAX_COLUMNS] = {0};
static bool matrixInitialized = false;

static CharacterNode *head = NULL, *selectedChar = NULL;
static int charCount = 0;
static Sound clickSound;
static Sound alertSound;
static bool wasHoveredLastFrame = false;
static bool isFadingOut = false;
static float fadeAlpha = 0.0f;

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
    Rectangle src = {0, 0, SPRITE_SRC_WIDTH, SPRITE_SRC_HEIGHT};
    Texture2D texture;
    if (strcmp(node->name, "Mateus") == 0)
        texture = (isHovered || isSelected) ? hacker2 : hacker1;
    else if (strcmp(node->name, "João") == 0)
        texture = (isHovered || isSelected) ? menina2 : menina1;
    else if (strcmp(node->name, "Mamede") == 0)
        texture = (isHovered || isSelected) ? meninoPdavida2 : meninoPdavida1;
    else if (strcmp(node->name, "Carlos") == 0)
        texture = (isHovered || isSelected) ? deBone2 : deBone1;
    else
    {
        int textW = MeasureText(node->name, 36);
        DrawText(node->name, btn.x + (btn.width - textW) / 2, btn.y + 20, 36, WHITE);
        return;
    }
    Rectangle dest = {btn.x, btn.y, SPRITE_BTN_WIDTH, SPRITE_BTN_HEIGHT};
    DrawTexturePro(texture, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void CreateCharacterList(void)
{
    if (head) return;
    const char *names[] = {"João", "Mateus", "Carlos", "Mamede"};
    CharacterNode *last = NULL;
    for (int i = 0; i < 4; i++)
    {
        CharacterNode *node = malloc(sizeof(CharacterNode));
        strcpy(node->name, names[i]);
        node->sfx = (Sound){0};
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
    hacker1 = LoadTexture("src/sprites/hacker1-unselected.png");
    hacker2 = LoadTexture("src/sprites/hacker2-selected.png");
    menina1 = LoadTexture("src/sprites/menina-unselected.png");
    menina2 = LoadTexture("src/sprites/menina-selected.png");
    meninoPdavida1 = LoadTexture("src/sprites/meninoPdavida-unselected.png");
    meninoPdavida2 = LoadTexture("src/sprites/meninoPdavida-selected.png");
    deBone1 = LoadTexture("src/sprites/deBone-unselected.png");
    deBone2 = LoadTexture("src/sprites/deBone-selected.png");
    clickSound = LoadSound("src/music/buttonPress.wav");
    alertSound = LoadSound("src/music/welcome-to-the-game-hacking-alert_sm4UxhuM.mp3");
    Rectangle frames[] = {
        {0, 512, 512, 512}, {0, 1024, 512, 512}, {0, 1536, 512, 512}, {512, 0, 512, 512}, {512, 512, 512, 512}, {512, 1024, 512, 512}, {512, 1536, 512, 512}, {1024, 0, 512, 512}, {1024, 512, 512, 512}, {1024, 1024, 512, 512}, {1024, 1536, 512, 512}, {1536, 0, 512, 512}, {1536, 512, 512, 512}, {1536, 1024, 512, 512}, {1536, 1536, 512, 512}
    };
    for (int i = 0; i < FRAME_COUNT; i++)
        matrixFrames[i] = frames[i];
    CreateCharacterList();
    matrixInitialized = false;
    wasHoveredLastFrame = false;
    isFadingOut = false;
    fadeAlpha = 0.0f;

    // Carregar sons dos personagens
    CharacterNode *node = head;
    do
    {
        if (strcmp(node->name, "João") == 0)
            node->sfx = LoadSound("src/music/menina-menu.wav");
        else if (strcmp(node->name, "Mateus") == 0)
            node->sfx = LoadSound("src/music/hacker-menu.wav");
        else if (strcmp(node->name, "Carlos") == 0)
            node->sfx = LoadSound("src/music/deBone-menu.wav");
        else if (strcmp(node->name, "Mamede") == 0)
            node->sfx = LoadSound("src/music/meninoPdavida-menu.wav");

        TraceLog(LOG_INFO, "Som carregado para: %s", node->name);
        node = node->next;
    } while (node != head);
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
    if (isFadingOut)
    {
        fadeAlpha += GetFrameTime();
        if (fadeAlpha >= 1.0f)
        {
            fadeAlpha = 1.0f;
            currentScreen = MENU_FINISHED;
        }
        return;
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
        int spacing = 30;
        int totalW = charCount * SPRITE_BTN_WIDTH + (charCount - 1) * spacing;
        int startX = (screenWidth - totalW) / 2;
        int y = screenHeight / 2 - SPRITE_BTN_HEIGHT / 2;
        CharacterNode *node = head;
        for (int i = 0; i < charCount; i++, node = node->next)
        {
            Rectangle btn = {startX + i * (SPRITE_BTN_WIDTH + spacing), y, SPRITE_BTN_WIDTH, SPRITE_BTN_HEIGHT};
            bool hovered = CheckCollisionPointRec(mouse, btn);
            if (hovered && node != selectedChar)
            {
                PlaySound(node->sfx);
                PlaySound(clickSound);
                selectedChar = node;
            }
            else if (hovered)
            {
                PlayHoverSound(btn, hovered);
            }
        }
        Rectangle confirmBtn = {screenWidth / 2 - 130, y + SPRITE_BTN_HEIGHT + 40, 260, 56};
        bool confirmHover = CheckCollisionPointRec(mouse, confirmBtn);
        PlayHoverSound(confirmBtn, confirmHover);
        if ((confirmHover && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))
        {
            PlaySound(clickSound);
            PlaySound(alertSound);
            isFadingOut = true;
        }
        if (IsKeyPressed(KEY_RIGHT))
        {
            selectedChar = selectedChar->next;
            PlaySound(selectedChar->sfx);
            PlaySound(clickSound);
        }
        else if (IsKeyPressed(KEY_LEFT))
        {
            selectedChar = selectedChar->prev;
            PlaySound(selectedChar->sfx);
            PlaySound(clickSound);
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
        int spacing = 30;
        int totalW = charCount * SPRITE_BTN_WIDTH + (charCount - 1) * spacing;
        int startX = (screenWidth - totalW) / 2;
        int y = screenHeight / 2 - SPRITE_BTN_HEIGHT / 2;
        CharacterNode *node = head;
        for (int i = 0; i < charCount; i++, node = node->next)
        {
            Rectangle btn = {startX + i * (SPRITE_BTN_WIDTH + spacing), y, SPRITE_BTN_WIDTH, SPRITE_BTN_HEIGHT};
            bool hovered = CheckCollisionPointRec(GetMousePosition(), btn);
            DrawCharacterButtonContent(node, btn, hovered, node == selectedChar);
        }
        DrawText("Escolha seu personagem", screenWidth / 2 - 180, y - 70, 28, RAYWHITE);
        Rectangle confirmBtn = {screenWidth / 2 - 130, y + SPRITE_BTN_HEIGHT + 40, 260, 56};
        Color confirmColor = CheckCollisionPointRec(GetMousePosition(), confirmBtn) ? DARKGREEN : GREEN;
        DrawRectangleRec(confirmBtn, confirmColor);
        DrawText("Confirmar", confirmBtn.x + 50, confirmBtn.y + 12, 32, WHITE);
    }
    if (isFadingOut)
    {
        DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, (unsigned char)(fadeAlpha * 255)});
    }
    EndDrawing();
}

bool MenuStartGame(void)
{
    return currentScreen == MENU_FINISHED;
}

int MenuSelectedCharacter(void)
{
    // Retorna o índice do personagem selecionado (0=João, 1=Mateus, 2=Carlos, 3=Mamede)
    // Caso selectedChar seja NULL, retorna 0 (João)
    if (!selectedChar) return 0;
    CharacterNode *node = head;
    int index = 0;
    while (node && node != selectedChar)
    {
        node = node->next;
        index++;
        if (node == head)
            break;
    }
    if (index < 0 || index > 3)
        index = 0; // fallback de segurança: João
    return index;
}

const char *MenuSelectedCharacterName(void)
{
    // Retorna o nome do personagem selecionado, padrão "João" se nenhum (ou inválido)
    if (!selectedChar || !selectedChar->name[0])
        return "João";
    return selectedChar->name;
}

void UnloadMenu(void)
{
    UnloadSound(clickSound);
    UnloadSound(alertSound);
    UnloadTexture(logoTexture);
    UnloadTexture(backgroundMatrix);
    UnloadTexture(hacker1);
    UnloadTexture(hacker2);
    UnloadTexture(menina1);
    UnloadTexture(menina2);
    UnloadTexture(meninoPdavida1);
    UnloadTexture(meninoPdavida2);
    UnloadTexture(deBone1);
    UnloadTexture(deBone2);
    CharacterNode *node = head;
    if (node)
    {
        do
        {
            UnloadSound(node->sfx);
            node = node->next;
        } while (node != head);
    }
    // Libere a lista!
    // (Para segurança, já que você usou malloc em CreateCharacterList)
    if (head)
    {
        CharacterNode* iter = head->next;
        while (iter != head)
        {
            CharacterNode* next = iter->next;
            free(iter);
            iter = next;
        }
        free(head);
        head = NULL;
    }
}