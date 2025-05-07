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

static void PlayCharacterSound(CharacterNode *node)
{
    if (!IsSoundPlaying(node->sfx))        // evita retrigger no mesmo frame
        PlaySound(node->sfx);
}

static void PlayHoverSound(Rectangle btn, bool hoveredNow)
{
    // Mantém o estado de hover de cada botão individualmente
    #define MAX_HOVER_BTNS 32
    typedef struct { Rectangle rect; bool wasHover; } HoverState;

    static HoverState states[MAX_HOVER_BTNS] = {0};
    static int count = 0;

    /* procura o botão ou o registra se ainda não existir */
    int id = -1;
    for (int i = 0; i < count; i++)
        if (memcmp(&states[i].rect, &btn, sizeof(Rectangle)) == 0)
        { id = i; break; }

    if (id < 0 && count < MAX_HOVER_BTNS)          // novo botão
    {
        id = count++;
        states[id].rect = btn;
        states[id].wasHover = false;
    }

    if (id >= 0)
    {
        if (hoveredNow && !states[id].wasHover)    // **ENTROU** no retângulo
            PlaySound(clickSound);

        states[id].wasHover = hoveredNow;          // guarda estado p/ próximo frame
    }
}

static void DrawCharacterButtonContent(CharacterNode *node, Rectangle btn, bool isHovered, bool isSelected)
{
    Rectangle src = { 0, 0, SPRITE_SRC_WIDTH, SPRITE_SRC_HEIGHT };
    Texture2D texture = { 0 };

    bool hasTexture = true;

    if (strcmp(node->name, "Dante") == 0)
        texture = (isHovered || isSelected) ? hacker2 : hacker1;
    else if (strcmp(node->name, "Alice")   == 0)
        texture = (isHovered || isSelected) ? menina2 : menina1;
    else if (strcmp(node->name, "Levi") == 0)
        texture = (isHovered || isSelected) ? meninoPdavida2 : meninoPdavida1;
    else if (strcmp(node->name, "Jade") == 0)
        texture = (isHovered || isSelected) ? deBone2 : deBone1;
    else
        hasTexture = false;

    if (hasTexture) {
        Rectangle dest = { btn.x, btn.y, SPRITE_BTN_WIDTH, SPRITE_BTN_HEIGHT };
        DrawTexturePro(texture, src, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
    }

    const int fontSize = 36;
    int textW = MeasureText(node->name, fontSize);
    int textX = btn.x + (btn.width - textW) / 2;
    int textY = btn.y + btn.height - fontSize - 16;

    if (isHovered && !isSelected) {
        DrawRectangleRec(btn, (Color){150, 150, 150, 100}); // Cinza translúcido por cima
    }

    DrawText(node->name, textX, textY, fontSize, BLACK);
}

void CreateCharacterList(void)
{
    if (head) return;
    const char *names[] = {"Alice", "Dante", "Jade", "Levi"};
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

    if (!IsAudioDeviceReady()) InitAudioDevice();

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

    for (int i = 0; i < FRAME_COUNT; i++) {
        matrixFrames[i] = frames[i];
    }

    CreateCharacterList();
    matrixInitialized = false;
    wasHoveredLastFrame = false;
    isFadingOut = false;
    fadeAlpha = 0.0f;

    CharacterNode *node = head;
    do
    {
        if (strcmp(node->name, "Alice") == 0) 
            node->sfx = LoadSound("src/music/menina-menu.mp3");
        else if (strcmp(node->name, "Dante") == 0)
            node->sfx = LoadSound("src/music/hacker-menu.mp3");
        else if (strcmp(node->name, "Jade") == 0)
            node->sfx = LoadSound("src/music/deBone-menu.mp3");
        else if (strcmp(node->name, "Levi") == 0)
            node->sfx = LoadSound("src/music/meninoPdavida-menu.mp3");

        SetSoundVolume(node->sfx, 3.5f);

        node = node->next;
    } while (node != head);
}

void UpdateMenu(void)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Vector2 mouse = GetMousePosition();

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

    /* ----------------------   TELA INICIAL   ---------------------------- */
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

    /* ----------------   SELEÇÃO DE PERSONAGENS   ------------------------ */
    else if (currentScreen == MENU_SELECT_CHAR)
    {
        int spacing = 30;
        int totalW = charCount * SPRITE_BTN_WIDTH + (charCount - 1) * spacing;
        int startX = (screenWidth - totalW) / 2;
        int y = screenHeight / 2 - SPRITE_BTN_HEIGHT / 2;
    
        static CharacterNode *lastHovered = NULL;
        CharacterNode *node = head;
        for (int i = 0; i < charCount; i++, node = node->next)
        {
            Rectangle btn = {startX + i * (SPRITE_BTN_WIDTH + spacing), y,
                             SPRITE_BTN_WIDTH, SPRITE_BTN_HEIGHT};
            bool hovered = CheckCollisionPointRec(mouse, btn);
    
            /* som ao entrar no hover */
            PlayHoverSound(btn, hovered);
            if (hovered && node != lastHovered)
            {
                PlayCharacterSound(node);
                lastHovered = node;
            }
            else if (!hovered && node == lastHovered)
            {
                lastHovered = NULL;
            }
    
            /* clique do mouse escolhe o personagem */
            if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON) &&
                node != selectedChar)
            {
                PlaySound(clickSound);
                selectedChar = node;
            }
        }
    
        /* navegação por setas também continua funcionando */
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        {
            selectedChar = selectedChar ? selectedChar->next : head;
            PlayCharacterSound(selectedChar);
            PlaySound(clickSound);

            // Calcula posição central logo abaixo do botão Confirmar
            Rectangle confirmBtn = {
                screenWidth/2 - 130,
                y + SPRITE_BTN_HEIGHT + 40,
                260, 56
            };

            int mx = confirmBtn.x + confirmBtn.width/2;
            int my = confirmBtn.y + confirmBtn.height + 8; // 8px abaixo do botão
            SetMousePosition(mx, my);
        }
        else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
        {
            selectedChar = selectedChar ? selectedChar->prev : head;
            PlayCharacterSound(selectedChar);      // aqui também!
            PlaySound(clickSound);

            // Calcula posição central logo abaixo do botão Confirmar
            Rectangle confirmBtn = {
                screenWidth/2 - 130,
                y + SPRITE_BTN_HEIGHT + 40,
                260, 56
            };
            
            int mx = confirmBtn.x + confirmBtn.width/2;
            int my = confirmBtn.y + confirmBtn.height + 8; // 8px abaixo do botão
            SetMousePosition(mx, my);
        }
    
        /* botão Confirmar – mouse ou ENTER */
        Rectangle confirmBtn = {screenWidth / 2 - 130,
                                y + SPRITE_BTN_HEIGHT + 40, 260, 56};
        bool confirmHover = CheckCollisionPointRec(mouse, confirmBtn);
        PlayHoverSound(confirmBtn, confirmHover);
        if ((confirmHover && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) ||
            IsKeyPressed(KEY_ENTER))
        {
            PlaySound(clickSound);
            PlaySound(alertSound);
            isFadingOut = true;
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

            Color normalColor = (Color){ 40, 40, 40, 200 };   // fundo escuro padrão
            Color hoverColor  = (Color){150, 150, 150, 100};
            DrawRectangleRec(btn, hovered ? hoverColor : normalColor);

            // Conteúdo do card (sprite + nome)
            DrawCharacterButtonContent(node, btn, hovered, node == selectedChar);

            // Destaque de borda se for o selecionado
            if (node == selectedChar) {
                DrawRectangleLinesEx(btn, 6, DARKGREEN);
            }
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
        index = 0;
    return index;
}

const char *MenuSelectedCharacterName(void)
{
    if (!selectedChar || !selectedChar->name[0])
        return "Alice";
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