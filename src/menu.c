#include "menu.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include "generalFunctions.h"
#include "playerStats.h"
#include <math.h>
#define FRAME_COUNT 15
#define SPRITE_SRC_WIDTH 480
#define SPRITE_SRC_HEIGHT 960
#define SPRITE_SCALE 0.5f
#define SPRITE_BTN_WIDTH (SPRITE_SRC_WIDTH * SPRITE_SCALE)
#define SPRITE_BTN_HEIGHT (SPRITE_SRC_HEIGHT * SPRITE_SCALE)
#define MAX_HOVER_BTNS 32
#define MENU_Y_OFFSET (-60)
#define LOGO_BASE_SCALE     0.50f   // tamanho médio da logo
#define LOGO_PULSE_AMPL     0.05f   // quanto ela “respira”
#define LOGO_PULSE_SPEED    2.5f    // ciclos por segundo

typedef struct CharacterNode
{
    char name[32];
    struct CharacterNode *next, *prev;
    Sound sfx;
} CharacterNode;

typedef enum
{
    MENU_MAIN,
    MENU_INPUT_NAME, 
    MENU_SELECT_CHAR,
    MENU_FINISHED
} MenuScreen;

static MenuScreen currentScreen = MENU_MAIN;
static Texture2D logo_jogo;
static Texture2D alerta;
static Texture2D background;
static Texture2D hacker1, hacker2;
static Texture2D menina1, menina2;
static Texture2D meninoPdavida1, meninoPdavida2;
static Texture2D deBone1, deBone2;
static Texture2D btnInit, btnInitHover;
static Texture2D btnRanking, btnRankingHover;
static int currentFrame = 0;
static float frameTime = 0.05f;
static float lastUpdate = 0.0f;
static CharacterNode *head = NULL, *selectedChar = NULL;
static int charCount = 0;
static Sound clickSound;
static Sound alertSound;
static bool wasHoveredLastFrame = false;
static bool isFadingOut = false;
static float fadeAlpha = 0.0f;
static bool rankingRequested = false;

static void PlayCharacterSound(CharacterNode *node)
{
    if (!IsSoundPlaying(node->sfx))        // evita retrigger no mesmo frame
        PlaySound(node->sfx);
}
static void PlayHoverSound(Rectangle btn, bool hoveredNow)
{
    // Mantém o estado de hover de cada botão individualmente
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
    background = LoadTexture("src/sprites/intro/bgFinal.png");
    logo_jogo = LoadTexture("src/sprites/logo_jogo.png");
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
    btnInit       = LoadTexture("src/sprites/btnInit.png");
    btnRanking    = LoadTexture("src/sprites/btnRanking.png");
    btnInitHover    = LoadTexture("src/sprites/btnInitHover.png");
    btnRankingHover = LoadTexture("src/sprites/btnRankingHover.png");
    alerta = LoadTexture("src/sprites/alerta.png");
    CreateCharacterList();
    wasHoveredLastFrame = false;
    isFadingOut = false;
    fadeAlpha = 0.0f;
    rankingRequested = false;
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
        // ==== ALTERADO: ESCALA, POSIÇÃO, DISTÂNCIA ====
        float btnScale = 0.6f;  // ou o valor que preferir
        int btnW = btnInit.width  * btnScale;
        int btnH = btnInit.height * btnScale;
        int distancia = 200;
        int totalW = btnW * 2 + distancia;
        int baseX = screenWidth/2 - totalW/2;
        int baseY = (screenHeight - btnH)/2 + 84;
        Rectangle startBtn   = { baseX,                 baseY, btnW, btnH };
        Rectangle rankingBtn = { baseX + btnW + distancia, baseY, btnW, btnH };

        bool hoverStart = CheckCollisionPointRec(mouse, startBtn);
        PlayHoverSound(startBtn, hoverStart);
        if ((hoverStart && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) ||
            IsKeyPressed(KEY_ENTER))
        {
            PlaySound(clickSound);
            memset(gPlayerName, 0, sizeof(gPlayerName));
            currentScreen = MENU_INPUT_NAME;
        }
        bool hoverRank = CheckCollisionPointRec(mouse, rankingBtn);
        PlayHoverSound(rankingBtn, hoverRank);
        if (hoverRank && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clickSound);
            rankingRequested = true;
        }
        return;
    }
    /* ----------------------   TELA DIGITAR NOME   ----------------------- */
    else if (currentScreen == MENU_INPUT_NAME)
    {
        int key;
        while ((key = GetCharPressed()) > 0)
        {
            size_t len = strlen(gPlayerName);
            if (len < MAX_PLAYER_NAME - 1)
            {
                if (key >= 32 && key <= 125)
                {
                    gPlayerName[len] = (char)key;
                    gPlayerName[len + 1] = '\0';
                }
            }
        }
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            size_t len = strlen(gPlayerName);
            if (len) gPlayerName[len-1] = '\0';
        }
        if (strlen(gPlayerName) > 0 && IsKeyPressed(KEY_ENTER))
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
        int y = screenHeight / 2 - SPRITE_BTN_HEIGHT / 2 - 130;
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
            strncpy(gSelectedCharacterName, selectedChar->name, MAX_PLAYER_NAME);
            gSelectedCharacterName[MAX_PLAYER_NAME-1] = '\0';
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
    float imgW = (float)screenWidth;
    float imgH = (float)screenHeight;
    DrawTexturePro(background, 
        (Rectangle){0, 0, (float)background.width, (float)background.height}, 
        (Rectangle){0, 0, imgW, imgH}, 
        (Vector2){0, 0}, 
        0.0f, 
        WHITE
    );
    if (currentScreen == MENU_MAIN)
    {
        float imgScale = LOGO_BASE_SCALE + LOGO_PULSE_AMPL * sinf(GetTime() * LOGO_PULSE_SPEED);
        float imgW = logo_jogo.width  * imgScale;
        float imgH = logo_jogo.height * imgScale;
        float logoY = (screenHeight - imgH)/2 + (MENU_Y_OFFSET - 130);
        DrawTextureEx(logo_jogo, (Vector2){ (screenWidth - imgW)/2, logoY }, 0.0f, imgScale, WHITE);

        float btnScale = 0.4f;
        int btnW = btnInit.width  * btnScale;
        int btnH = btnInit.height * btnScale;
        int distancia = 200;
        int totalW = btnW * 2 + distancia;
        int baseX = screenWidth/2 - totalW/2;
        int baseY = (screenHeight - btnH)/2 + 84;
        Rectangle startBtn   = { baseX,                 baseY, btnW, btnH };
        Rectangle rankingBtn = { baseX + btnW + distancia, baseY, btnW, btnH };
        bool hoveringInit    = CheckCollisionPointRec(GetMousePosition(), startBtn);
        bool hoveringRanking = CheckCollisionPointRec(GetMousePosition(), rankingBtn);
        Texture2D texInit    = (hoveringInit    && btnInitHover.id > 0)    ? btnInitHover    : btnInit;
        Texture2D texRanking = (hoveringRanking && btnRankingHover.id > 0) ? btnRankingHover : btnRanking;
        DrawTexturePro(texInit,
            (Rectangle){0,0,texInit.width, texInit.height},
            startBtn, (Vector2){0,0}, 0, WHITE
        );
        DrawTexturePro(texRanking,
            (Rectangle){0,0,texRanking.width, texRanking.height},
            rankingBtn, (Vector2){0,0}, 0, WHITE
        );
    }
    else if (currentScreen == MENU_INPUT_NAME)
    {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        const char *title = "Digite seu nome:";
        int titleFontSize = 36;
        int titleWidth = MeasureText(title, titleFontSize);
        int titleX = (screenWidth - titleWidth) / 2;
        int titleY = screenHeight / 2 - 120-130;
        DrawText(title, titleX, titleY, titleFontSize, RAYWHITE);
        /* Caixa de texto */
        int boxW = 560, boxH = 70;
        Rectangle box = {screenWidth/2 - boxW/2, screenHeight/2 - boxH/2-130, boxW, boxH};
        DrawRectangleRec(box, DARKGRAY);
        DrawRectangleLinesEx(box, 4, GREEN);
        /* Texto digitado + cursor piscante */
        const int fontSize = 36;
        int textX = box.x + 20;
        int textY = box.y + (box.height - fontSize) / 2;
        DrawText(gPlayerName, textX, textY, fontSize, WHITE);
        /* cursor (“_”) piscando */
        if (((int)(GetTime()*2) & 1) == 0) {
            int cursorX = textX + MeasureText(gPlayerName, fontSize) + 4;
            DrawText("_", cursorX, textY, fontSize, WHITE);
        }
        /* Dica de confirmação */
        if (strlen(gPlayerName) > 0)
            DrawText("Pressione ENTER para confirmar", screenWidth/2 - 190, box.y + box.height + 20, 24, BLACK);

        // EXIBIÇÃO DO ALERTA QUANDO ULTRAPASSA O LIMITE DE CARACTERES
        if (strlen(gPlayerName) == MAX_PLAYER_NAME - 1)
        {
            int avisoY = 120;
            int avisoFont = 20;
            float alertaScale = 0.5f;
            int alertaW = alerta.width * alertaScale;
            int alertaH = alerta.height * alertaScale;
            int imgX = 50 + 16 + 320; // 100px mais para a direita
            int imgY = avisoY-80;
            // Desenha o PNG
            DrawTextureEx(alerta, (Vector2){ imgX, imgY }, 0.0f, alertaScale, WHITE);
            // Texto centralizado logo abaixo da imagem
            const char *msg = "Limite de caracteres atingido!";
            int textoW = MeasureText(msg, avisoFont);
            int textoX = (imgX + (alertaW - textoW)/2)+238;
            int textoY = imgY + alertaH - 102; // 2px abaixo do PNG, bem próximo
            DrawText(msg, textoX, textoY, avisoFont, RED);
        }
    }
    else if (currentScreen == MENU_SELECT_CHAR)
    {
        int spacing = 30;
        int totalW = charCount * SPRITE_BTN_WIDTH + (charCount - 1) * spacing;
        int startX = (screenWidth - totalW) / 2;
        int y = screenHeight / 2 - SPRITE_BTN_HEIGHT / 2 - 130;
        CharacterNode *node = head;
        for (int i = 0; i < charCount; i++, node = node->next)
        {
            Rectangle btn = {startX + i * (SPRITE_BTN_WIDTH + spacing), y, SPRITE_BTN_WIDTH, SPRITE_BTN_HEIGHT};
            bool hovered = CheckCollisionPointRec(GetMousePosition(), btn);
            Color normalColor = (Color){ 40, 40, 40, 200 };   // fundo escuro padrão
            Color hoverColor  = (Color){150, 150, 150, 100};
            DrawRectangleRec(btn, hovered ? hoverColor : normalColor);
            DrawCharacterButtonContent(node, btn, hovered, node == selectedChar);
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
        DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, (unsigned char)(fadeAlpha * 255)});
    EndDrawing();
}
bool MenuStartGame(void) { return currentScreen == MENU_FINISHED; }
bool MenuShowRanking(void) { return rankingRequested; }
void UnloadMenu(void)
{
    UnloadSound(clickSound);
    UnloadSound(alertSound);
    UnloadTexture(logo_jogo);
    UnloadTexture(background);
    UnloadTexture(hacker1);
    UnloadTexture(hacker2);
    UnloadTexture(menina1);
    UnloadTexture(menina2);
    UnloadTexture(meninoPdavida1);
    UnloadTexture(meninoPdavida2);
    UnloadTexture(deBone1);
    UnloadTexture(deBone2);
    UnloadTexture(btnInit);
    UnloadTexture(btnRanking);
    UnloadTexture(btnInitHover);
    UnloadTexture(btnRankingHover);
    UnloadTexture(alerta);
    CharacterNode *node = head;
    if (node)
    {
        do
        {
            UnloadSound(node->sfx);
            node = node->next;
        } while (node != head);
    }
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