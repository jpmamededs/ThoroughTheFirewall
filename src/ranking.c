#include "ranking.h"
#include "raylib.h"
#include "playerStats.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define MAX_RANKING_PLAYERS 200
#define ITEMS_PER_PAGE 10
#define BASE_H 64
#define ROW_HEIGHT (BASE_H + 10)
#define AV_RADIUS 23 
#define MENU_BOX_RECT (Rectangle){20, 20, 160, 40}
#define FILTER_BTN_RECT (Rectangle){ GetScreenWidth()-180, 20, 160, 40 }
#define BG_VERT_SPACING  42
#define BG_CIRC_NODES    18 

static PlayerStatsDTO RankingPlayers[MAX_RANKING_PLAYERS];
static int PlayerCont = 0;
static int currentPage = 0;
static bool fase_concluida = false;
static bool showReprovados = false; 
static Font cyberFont;
static Sound clickSound;
static Texture2D avatarTex[4];

typedef struct { Vector2 a, b; } Segment;
static Segment circuitSegs[BG_CIRC_NODES];
static int circuitCount = 0;

static bool MatchesFilter(const PlayerStatsDTO *p)
{
    return showReprovados ? !p->isPassouSelecao : p->isPassouSelecao;
}

static int GetFilteredCount(void)
{
    int c = 0;
    for (int i = 0; i < PlayerCont; ++i)
        if (MatchesFilter(&RankingPlayers[i])) ++c;
    return c;
}

static int GetFilteredIndex(int filteredIdx)
{
    for (int i = 0, k = 0; i < PlayerCont; ++i)
        if (MatchesFilter(&RankingPlayers[i]))
            if (k++ == filteredIdx) return i;
    return -1;
}

static void DrawGlowText(Font f, const char *txt, Vector2 pos, float size, float spacing, Color core, Color glow, int passes)
{
    for (int i = passes; i > 0; --i)
        DrawTextEx(f, txt, (Vector2){ pos.x - i, pos.y - i }, size, spacing, Fade(glow, 0.18f));
    DrawTextEx(f, txt, pos, size, spacing, core);
}

static int AvatarFor(const char *name)
{
    if      (strcmp(name, "Alice")   == 0) return 0;
    else if (strcmp(name, "Dante") == 0) return 1;
    else if (strcmp(name, "Jade") == 0) return 2;
    else if (strcmp(name, "Levi") == 0) return 3;
    return 0;
}

static Rectangle GetRankingBox(void)
{
    int sw = GetScreenWidth();
    const float BOX_RATIO = 0.65f;

    int w = (int)roundf(sw * BOX_RATIO);
    int x = (sw - w) / 2;          /* margem esquerda = margem direita */

    return (Rectangle){ x, 120, w, (ITEMS_PER_PAGE * ROW_HEIGHT) + 40 };
}

static void DrawNeonButton(Rectangle r, const char *label, Color core, Color glow, bool hover)
{
    float t = GetTime();
    float pulse = 0.6f + 0.4f * sinf(t*3.0f);           /* 0.2 ↔ 1.0 */
    Color halo = Fade(glow, hover ? 0.50f : 0.30f * pulse);

    /* 1) halo externo (duas passadas finas) */
    for (int g = 4; g >= 2; --g)
        DrawRectangleRoundedLines(r, 0.25f, 6,  Fade(halo, 0.12f*g));

    /* 2) fundo (gradiente vertical) */
    Color top = Fade(core, hover ? 0.25f : 0.15f);
    Color bot = Fade(BLACK, 0.75f);
    DrawRectangleGradientEx(r, top, top, bot, bot);

    /* 3) contorno principal */
    DrawRectangleRoundedLines(r, 0.25f, 6, core);

    /* 4) texto centralizado com glow leve */
    Vector2 txtSz = MeasureTextEx(cyberFont, label, 24, 2);
    Vector2 pos   = { r.x + (r.width-txtSz.x)/2,
                      r.y + (r.height-txtSz.y)/2 };

    DrawTextEx(cyberFont, label, (Vector2){pos.x-1,pos.y-1}, 24, 2,
               Fade(halo, 0.6f));
    DrawTextEx(cyberFont, label, pos, 24, 2, WHITE);
}

static void LoadRankingFromFile(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp)
        return;

    char line[256];
    while (fgets(line, sizeof(line), fp) && PlayerCont < MAX_RANKING_PLAYERS)
    {
        char playerName[MAX_PLAYER_NAME] = {0};
        char characterName[MAX_PLAYER_NAME] = {0};
        float pontuacao = 0;
        int passouInt = 0;

        if (sscanf(line, "%25[^,], %25[^,], %f, %d", playerName, characterName, &pontuacao, &passouInt) == 4)
        {
            bool isPassou = (passouInt != 0);

            strncpy(RankingPlayers[PlayerCont].playerName, playerName, MAX_PLAYER_NAME);
            strncpy(RankingPlayers[PlayerCont].characterName, characterName, MAX_PLAYER_NAME);
            RankingPlayers[PlayerCont].notalGeral = pontuacao;
            RankingPlayers[PlayerCont].isPassouSelecao = isPassou;

            PlayerCont++;
        }
    }

    fclose(fp);
}

static void BubbleSort(void)
{
    for (int i = 1; i <= PlayerCont; i++)
    {
        for (int j = 0; j < PlayerCont - 1; j++)
        {
            if (RankingPlayers[j].notalGeral < RankingPlayers[j + 1].notalGeral)
            {
                PlayerStatsDTO tmp = RankingPlayers[j];
                RankingPlayers[j] = RankingPlayers[j + 1];
                RankingPlayers[j + 1] = tmp;
            }
        }
    }
}

void AppendPlayerToRankingFile(PlayerStats *ps, char *file)
{
    if (!ps || !file) return;

    FILE *fp = fopen(file, "a");

    if (!fp) return;

    fprintf(fp, "%s, %s, %.2f, %d\n",
            ps->playerName,
            ps->characterName,
            ps->notalGeral, 
            ps->isPassouSelecao ? 1 : 0
    );

    fclose(fp);
}

static Rectangle GetMenuBtnRect(void)
{
    return (Rectangle){ 20, 20, 160, 40 };
}

static Rectangle GetFilterBtnRect(void)
{
    Rectangle m = GetMenuBtnRect();
    return (Rectangle){ m.x, m.y + m.height + 20, 160, 40 };
}

void Init_Ranking(void)
{
    avatarTex[0] = LoadTexture("src/sprites/avatar/alice_avatar.png");
    avatarTex[1] = LoadTexture("src/sprites/avatar/dante_avatar.png");
    avatarTex[2] = LoadTexture("src/sprites/avatar/jade_avatar.png");
    avatarTex[3] = LoadTexture("src/sprites/avatar/levi_avatar.png");
    clickSound   = LoadSound("src/music/buttonPress.wav");
    cyberFont    = GetFontDefault();

    fase_concluida = false;
    PlayerCont = 0;
    currentPage = 0;
    showReprovados = false;
    
    // Carrega e Ordena o Ranking
    LoadRankingFromFile("ranking.txt");
    BubbleSort();

    // Desenho do Fundo animado
    srand(42);
    int sw = GetScreenWidth(), sh = GetScreenHeight();
    circuitCount = BG_CIRC_NODES;
    for (int i = 0; i < circuitCount; ++i) {
        circuitSegs[i].a = (Vector2){ rand()%sw, rand()%sh };
        int len = 60 + rand()%80;
        int dir = rand()%3;
        switch (dir) {
            case 0: circuitSegs[i].b = (Vector2){ circuitSegs[i].a.x+len, circuitSegs[i].a.y }; break;
            case 1: circuitSegs[i].b = (Vector2){ circuitSegs[i].a.x+len, circuitSegs[i].a.y-len }; break;
            case 2: circuitSegs[i].b = (Vector2){ circuitSegs[i].a.x+len, circuitSegs[i].a.y+len }; break;
        }
    }
}

void Update_Ranking(void)
{
    Vector2 mouse = GetMousePosition();
    bool mouseClick = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Rectangle menuRect   = GetMenuBtnRect();
    Rectangle filterRect = GetFilterBtnRect();

    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, menuRect)) || IsKeyPressed(KEY_M))
    {
        PlaySound(clickSound);
        fase_concluida = true;
        return;
    }

    if (mouseClick && CheckCollisionPointRec(mouse, filterRect))
    {
        PlaySound(clickSound);
        showReprovados = !showReprovados;
        currentPage    = 0;
    }

    /* ---------- paginação ---------- */
    int filteredCnt = GetFilteredCount();
    int totalPages  = (filteredCnt + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;

    /*  ---  reconstruímos as mesmas caixas usadas no Draw_Ranking --- */
    Rectangle rankingBox = GetRankingBox();

    const float BTN_W = 120, BTN_H = 44, GAP = 40;
    float midX = rankingBox.x + rankingBox.width / 2.0f;

    Rectangle btnPrev = { midX - GAP/2.0f - BTN_W, rankingBox.y + rankingBox.height + 20, BTN_W, BTN_H };
    Rectangle btnNext = { midX + GAP/2.0f, btnPrev.y, BTN_W, BTN_H };

    /* próximo */
    if ((mouseClick && CheckCollisionPointRec(mouse, btnNext)) || (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)))
    {
        PlaySound(clickSound);
        if (currentPage < totalPages - 1) currentPage++;
    }

    /* anterior */
    if ((mouseClick && CheckCollisionPointRec(mouse, btnPrev)) || (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)))
    {
        PlaySound(clickSound);
        if (currentPage > 0) currentPage--;
    }
}

void Draw_Ranking(void)
{
    const int sw = GetScreenWidth();

    /* ---- cores de tema ---- */
    const Color NEON_GREEN    = (Color){0, 255, 120, 255};
    const Color NEON_GREEN_LT = (Color){0, 255, 120, 80};
    const Color NEON_RED      = { 255,  80,  80, 255 };
    const Color NEON_RED_LT   = { 255,  80,  80,  80 };

    const Color THEME     = showReprovados ? NEON_RED     : NEON_GREEN;
    const Color THEME_LT  = showReprovados ? NEON_RED_LT  : NEON_GREEN_LT;

    const int TITLE_SIZE = 52;
    const float HOVER_SCALE = 1.01f;

    BeginDrawing();
    ClearBackground(BLACK);
    DrawRectangle(0,0, sw, GetScreenHeight(), Fade(BLACK, 0.60f));

    /* 2.2  LINHAS VERTICAIS pulsantes ----------------------------------- */
    float tm = GetTime();
    for (int x = 0; x < sw; x += BG_VERT_SPACING) {
        /* brilho oscila 0-40% (seno defasado por coluna) */
        float alpha = 0.2f + 0.2f * sinf( (float)x*0.05f + tm*2.0f );
        Color vcol  = Fade(showReprovados ? (Color){255,80,80,255}
                                        : (Color){0,255,120,255}, alpha);
        DrawLine(x, 0, x, GetScreenHeight(), vcol);
    }

    /* 2.3  MAPA DE CIRCUITOS -------------------------------------------- */
    for (int i = 0; i < circuitCount; ++i) {
        /* pulsar suave (0.15 → 0.35) */
        float glow = 0.15f + 0.10f * sinf(tm*1.6f + i);
        Color ccol = Fade(showReprovados ? (Color){255,80,80,255}
                                        : (Color){0,255,120,255}, glow);
        DrawLineEx(circuitSegs[i].a, circuitSegs[i].b, 2, ccol);

        /* “nós” pequenos */
        DrawCircleV(circuitSegs[i].a, 3, ccol);
        DrawCircleV(circuitSegs[i].b, 3, ccol);
    }

    Rectangle menuRect   = GetMenuBtnRect();
    Rectangle filterRect = GetFilterBtnRect();

    /* ─── Menu [M] (verde) ────────────────────────────────────────────── */
    bool hoverMenu = CheckCollisionPointRec(GetMousePosition(), menuRect);
    DrawNeonButton(menuRect, "Menu", THEME, THEME_LT, hoverMenu);

    /* ─── Reprovados (vermelho fixo) ──────────────────────────────────── */
    bool hoverFilter = CheckCollisionPointRec(GetMousePosition(), filterRect);
    DrawNeonButton(filterRect, "Reprovados", NEON_RED, NEON_RED_LT, hoverFilter);

    /* ---------- cabeçalho ---------- */
    const char *title = showReprovados ? "reprovados" : "aprovados";
    Vector2 tSz  = MeasureTextEx(cyberFont, title, (float)TITLE_SIZE, 1);
    Vector2 base = { (sw - tSz.x) / 2, 40 };

    float pulse  = 0.6f + 0.4f * sinf(tm * 3.0f);
    Color core   = Fade(THEME, 0.8f);
    Color glow   = Fade(THEME, 0.4f * pulse);

    for (int d = 3; d > 0; --d) 
        DrawTextEx(cyberFont, title, (Vector2){ base.x - d, base.y - d }, TITLE_SIZE, 2, glow);
    DrawTextEx(cyberFont, title, base, TITLE_SIZE, 2, core);

    int  frame = (int)(tm * 60.0f);
    if (frame % 30 == 0) {
        int dx  = (GetRandomValue(-2, 2));
        int dy  = (GetRandomValue(-1, 1));
        DrawTextEx(cyberFont, title, (Vector2){ base.x + dx, base.y + dy }, TITLE_SIZE, 2, Fade(core, 0.7f));
    }

    int borderOffset = 2 + (int)(2 * sinf(GetTime() * 4));
    for (int d = borderOffset; d > 0; --d) {
        Vector2 offsetPos = { base.x - d, base.y - d };
        DrawTextEx(cyberFont, title, offsetPos, TITLE_SIZE, 2, Fade(THEME, 0.2f));
    }

    /* ---- retângulo principal ---- */
    Rectangle rankingBox = GetRankingBox();
    
    /* ---------- caixa grande ---------- */
    Color bgBoxColor = (Color){10, 10, 10, 255};  // Preto claro com leve transparência
    Color bgBorderColor = THEME;   
    DrawRectangleRounded(rankingBox, 0.03f, 8, bgBoxColor);
    DrawRectangleRoundedLines(rankingBox, 0.03f, 8, bgBorderColor);

    /* índices filtrados + paginação */
    int filteredCnt = GetFilteredCount();
    int startIdx    = currentPage * ITEMS_PER_PAGE;
    int drawCnt     = filteredCnt - startIdx;
    if (drawCnt > ITEMS_PER_PAGE) drawCnt = ITEMS_PER_PAGE;

    /* ---------- linhas (máx. 12) ---------- */
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < drawCnt; ++i)
    {
        int idx = GetFilteredIndex(startIdx + i);
        if (idx < 0) break;

        const float PAD = 0.05f; 
        Rectangle base = {
            .width  = rankingBox.width * (1 - PAD*2),
            .height = BASE_H,
            .x      = rankingBox.x + rankingBox.width * PAD,
            .y = rankingBox.y + 20 + i * ROW_HEIGHT
        };

        bool hover = CheckCollisionPointRec(mouse, base);

        Rectangle box = base;
        if (hover)
        {
            float nw = box.width * HOVER_SCALE;
            float nh = box.height * HOVER_SCALE;
            box.x -= (nw - box.width) / 2;
            box.y -= (nh - box.height) / 2;
            box.width = nw;
            box.height = nh;
        }

        Rectangle shadow = { box.x - 1, box.y + 2, box.width + 2, box.height };
        DrawRectangleRounded(shadow, 0.10f, 6, Fade(BLACK, 0.4f));

        /* gradiente horizontal sutil (esq►dir) */
        Color c1 = Fade(WHITE, 0.05f);      // 5 % opaco
        Color c2 = Fade(THEME, 0.06f);      // 6 % opaco na cor do tema
        DrawRectangleGradientEx((Rectangle){
                box.x, box.y, box.width, box.height
            }, c1, c2, c2, c1);

        Color lineColor = hover ? THEME : THEME_LT;
        DrawRectangleRoundedLines(box, 0.10f, 6, lineColor);

        /* ---- 1) posição ---- */
        int rankPos = startIdx + i + 1;
        char posStr[6];
        snprintf(posStr, sizeof(posStr), "# %d", rankPos);
        DrawTextEx(cyberFont,
                   posStr,
                   (Vector2){box.x + 12, box.y + (box.height - 24) / 2},
                   24,
                   2,
                   WHITE);

        /* ---- 2) avatar placeholder ---- */
        Vector2 avPos = {box.x + 120, box.y + box.height / 2};
        int id = AvatarFor(RankingPlayers[idx].characterName);
        Texture2D avatar = avatarTex[id];

        float scale  = hover ? HOVER_SCALE : 1.0f;
        float radius = AV_RADIUS * scale;
        float diam   = radius * 2.0f;

        Rectangle src = { 0, 0, avatar.width, avatar.height };
        Rectangle dst = { avPos.x - radius, avPos.y - radius, diam, diam };

        DrawTexturePro(avatar, src, dst, (Vector2){0,0}, 0.0f, WHITE);
        DrawCircleLines((int)avPos.x, (int)avPos.y, (int)radius, lineColor);
        DrawTexturePro(avatar, src, dst, (Vector2){0,0}, 0.0f, WHITE);

        for (int g = 0; g < 3; ++g) {
            int glowR = (int)radius + 2 + g*2;
            Color glow = Fade(lineColor, 0.30f - g*0.09f);   // 30%, 21%, 12%
            DrawCircleLines((int)avPos.x, (int)avPos.y, glowR, glow);
        }
        DrawCircleLines((int)avPos.x, (int)avPos.y, (int)radius, lineColor);

        /* ---- 3) nome ---- */
        float fontSz = 24 * (hover ? HOVER_SCALE : 1.0f);
        Vector2 nameSize = MeasureTextEx(cyberFont, RankingPlayers[idx].playerName, fontSz, 2);
        Vector2 namePos = { box.x + (box.width-nameSize.x)/2,
                            box.y + (box.height-fontSz)/2 };
        DrawGlowText(cyberFont,
                    RankingPlayers[idx].playerName,
                    namePos, fontSz, 2,
                    WHITE, lineColor, 2
        );

        /* ---- 4) pontuação ---- */
        char scoreStr[16];
        snprintf(scoreStr, sizeof(scoreStr), "%.2f", RankingPlayers[idx].notalGeral);
        float scoreFont = fontSz;
        Vector2 scoreSz = MeasureTextEx(cyberFont, scoreStr, scoreFont, 2);
        Vector2 scorePos = { box.x + box.width - scoreSz.x - 16,
                            namePos.y };
        DrawGlowText(cyberFont, scoreStr, scorePos, scoreFont, 2,
                    lineColor, lineColor, 1);
    }

    /* ---  botões “ant” e “prox”  (adicione abaixo do for(...) e acima do EndDrawing) --- */
    const float BTN_W   = 120;
    const float BTN_H   = 44;
    const float GAP     = 40;                   // espaço entre botões
    float midX          = rankingBox.x + rankingBox.width / 2.0f;

    Rectangle btnPrev = { midX - GAP/2.0f - BTN_W, rankingBox.y + rankingBox.height + 20, BTN_W, BTN_H };
    Rectangle btnNext = { midX + GAP/2.0f, btnPrev.y, BTN_W, BTN_H };

    bool prevHover = CheckCollisionPointRec(mouse, btnPrev);
    bool nextHover = CheckCollisionPointRec(mouse, btnNext);

    /* fundo + borda */
    DrawRectangleRounded(btnPrev, 0.25f, 6, prevHover? Fade(WHITE,0.15f) : Fade(WHITE,0.07f));
    DrawRectangleRoundedLines(btnPrev, 0.25f, 6, THEME);
    DrawRectangleRounded(btnNext, 0.25f, 6, nextHover? Fade(WHITE,0.15f) : Fade(WHITE,0.07f));
    DrawRectangleRoundedLines(btnNext, 0.25f, 6, THEME);

    /* rótulos */
    DrawTextEx(cyberFont, "ant",
               (Vector2){btnPrev.x + 32, btnPrev.y + 8}, 26, 2, WHITE);

    DrawTextEx(cyberFont, "prox",
               (Vector2){btnNext.x + 20, btnNext.y + 8}, 26, 2, WHITE);

    EndDrawing();
}

bool Ranking_Concluido(void){ return fase_concluida; }

void Unload_Ranking(void)
{
    PlayerCont = 0;
    for (int i = 0; i < MAX_RANKING_PLAYERS; i++)
        memset(&RankingPlayers[i], 0, sizeof(PlayerStatsDTO));

    for (int i = 0; i < 4; ++i) UnloadTexture(avatarTex[i]);
    UnloadSound(clickSound);
}
