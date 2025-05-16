#include "ranking.h"
#include "raylib.h"
#include "playerStats.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ITEMS_PER_PAGE 12
#define MENU_BOX_RECT (Rectangle){20, 20, 160, 40}

static PlayerStatsDTO RankingPlayers[MAX_RANKING_PLAYERS];
static int PlayerCont = 0;
static bool fase_concluida = false;
static int currentPage = 0;
static Font cyberFont;
static Sound clickSound;

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

        if (sscanf(line, " %25[^,], %25[^,], %f, %d", playerName, characterName, &pontuacao, &passouInt) == 4)
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

void Init_Ranking(void)
{
    fase_concluida = false;
    PlayerCont = 0;
    cyberFont = GetFontDefault();
    LoadRankingFromFile("ranking.txt");
    clickSound = LoadSound("src/music/buttonPress.wav");
    BubbleSort();
}

void Update_Ranking(void)
{
    Vector2 mouse = GetMousePosition();

    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, MENU_BOX_RECT)) || IsKeyPressed(KEY_M))
    {
        PlaySound(clickSound);
        fase_concluida = true;
        return;
    }

    /* ---------- paginação ---------- */
    int totalPages = (PlayerCont + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;

    /*  ---  reconstruímos as mesmas caixas usadas no Draw_Ranking --- */
    Rectangle rankingBox = {
        .width = GetScreenWidth() * 0.65f,
        .x = (GetScreenWidth() * 0.35f) / 2,
        .y = 120,
        .height = (ITEMS_PER_PAGE * 60) + 40 /* 60 = 50 de altura + 10 de gap */
    };
    Rectangle btnPrev = {rankingBox.x, rankingBox.y + rankingBox.height + 20, 120, 44};
    Rectangle btnNext = {rankingBox.x + rankingBox.width - 120, btnPrev.y, 120, 44};

    bool mouseClick = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    /* próximo */
    if ((mouseClick && CheckCollisionPointRec(mouse, btnNext)) || (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)))
    {
        PlaySound(clickSound);
        if (currentPage < totalPages - 1)
            currentPage++;
    }

    /* anterior */
    if ((mouseClick && CheckCollisionPointRec(mouse, btnPrev)) || (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)))
    {
        PlaySound(clickSound);
        if (currentPage > 0)
            currentPage--;
    }
}

void Draw_Ranking(void)
{
    const int sw = GetScreenWidth();

    /* ---- cores de tema ---- */
    const Color NEON_GREEN = (Color){0, 255, 120, 255};
    const Color NEON_GREEN_LT = (Color){0, 255, 120, 80};
    const Color BG_TRANS = (Color){0, 0, 0, 150};

    const int TITLE_SIZE = 52;
    const int ROW_HEIGHT = 60; /* 50px de caixa + 10px de espaçamento */
    const int AV_RADIUS = 18;
    const int BASE_H = 50;
    const float HOVER_SCALE = 1.01f;

    /* --- caixa “Menu [M]” (canto superior esquerdo) ---------------------------- */
    bool hoverMenu = CheckCollisionPointRec(GetMousePosition(), MENU_BOX_RECT);
    Color menuFill = hoverMenu ? NEON_GREEN_LT : BG_TRANS;
    Color menuLine = hoverMenu ? NEON_GREEN : NEON_GREEN_LT;
    DrawRectangleRounded(MENU_BOX_RECT, 0.25f, 6, menuFill);
    DrawRectangleRoundedLines(MENU_BOX_RECT, 0.25f, 6, 2.0f, menuLine); // Aqui, "2.0f" representa a espessura da linha
    DrawTextEx(cyberFont, "Menu [M]", (Vector2){MENU_BOX_RECT.x + 16, MENU_BOX_RECT.y + 8}, 24, 2, WHITE);

    /* ---- retângulo principal ---- */
    Rectangle rankingBox = {
        .width = sw * 0.65f,
        .x = (sw * 0.35f) / 2,
        .y = 120,
        .height = (ITEMS_PER_PAGE * ROW_HEIGHT) + 40};

    BeginDrawing();
    ClearBackground(BLACK);

    /* ---------- cabeçalho ---------- */
    const char *title = "Aprovados";
    Vector2 tSz = MeasureTextEx(cyberFont, title, (float)TITLE_SIZE, 1);
    DrawText(title,
             (sw - (int)tSz.x) / 2,
             40,
             TITLE_SIZE,
             NEON_GREEN);

    /* ---------- caixa grande ---------- */
    DrawRectangleRounded(rankingBox, 0.03f, 8, BG_TRANS);
    DrawRectangleRoundedLines(MENU_BOX_RECT, 0.25f, 6, 2.0f, menuLine); // Aqui, "2.0f" representa a espessura da linha

    /* ---------- linhas (máx. 12) ---------- */
    int numToDraw = (PlayerCont < ITEMS_PER_PAGE) ? PlayerCont : ITEMS_PER_PAGE;
    Vector2 mouse = GetMousePosition();

    int startIdx = currentPage * ITEMS_PER_PAGE;
    int numRemaining = PlayerCont - startIdx;
    if (numRemaining < numToDraw)
        numToDraw = numRemaining;

    for (int i = 0; i < numToDraw; ++i)
    {
        int idx = startIdx + i;

        /* posição e tamanho base da linha */
        Rectangle base = {
            .width = rankingBox.width * 0.85f,
            .height = BASE_H,
            .x = rankingBox.x + (rankingBox.width * 0.05f),
            .y = rankingBox.y + 20 + i * ROW_HEIGHT};

        /* checa hover sobre a caixa BASE (não a ampliada) */
        bool hover = CheckCollisionPointRec(mouse, base);

        /* duplicamos em ‘box’, aplicando escala se necessário */
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

        /* fundo + borda (mais brilhante se hover) */
        Color lineColor = hover ? NEON_GREEN : NEON_GREEN_LT;
        DrawRectangleRounded(box, 0.10f, 6, Fade(WHITE, hover ? 0.07f : 0.03f));
        DrawRectangleRoundedLines(MENU_BOX_RECT, 0.25f, 6, 2.0f, menuLine); // Aqui, "2.0f" representa a espessura da linha

        /* ---- 1) posição ---- */
        char posStr[6];
        snprintf(posStr, sizeof(posStr), "# %d", idx + 1);
        DrawTextEx(cyberFont,
                   posStr,
                   (Vector2){box.x + 12, box.y + (box.height - 24) / 2},
                   24,
                   2,
                   WHITE);

        /* ---- 2) avatar placeholder ---- */
        Vector2 avPos = {box.x + 110, box.y + box.height / 2};
        DrawCircleV(avPos, AV_RADIUS * (hover ? HOVER_SCALE : 1.0f), NEON_GREEN_LT);
        DrawCircleLines((int)avPos.x, (int)avPos.y,
                        (int)(AV_RADIUS * (hover ? HOVER_SCALE : 1.0f)), lineColor);

        /* ---- 3) nome ---- */
        Vector2 nameSize = MeasureTextEx(cyberFont, RankingPlayers[idx].playerName, 22, 2);
        Vector2 namePos = {
            box.x + (box.width / 2 - nameSize.x / 2), /* centralizado horizontalmente */
            box.y + (box.height - 22) / 2             /* centralizado verticalmente */
        };
        DrawTextEx(cyberFont,
                   RankingPlayers[idx].playerName,
                   namePos,
                   22 * (hover ? HOVER_SCALE : 1.0f),
                   2,
                   LIGHTGRAY);

        /* ---- 4) pontuação ---- */
        char scoreStr[16];
        snprintf(scoreStr, sizeof(scoreStr), "%.2f", RankingPlayers[idx].notalGeral);
        float scoreFont = 22 * (hover ? HOVER_SCALE : 1.0f);
        Vector2 scoreSz = MeasureTextEx(cyberFont, scoreStr, scoreFont, 2);
        DrawTextEx(cyberFont,
                   scoreStr,
                   (Vector2){box.x + box.width - scoreSz.x - 16,
                             box.y + (box.height - scoreFont) / 2},
                   scoreFont,
                   2,
                   lineColor);
    }

    /* ---  botões “ant” e “prox”  (adicione abaixo do for(...) e acima do EndDrawing) --- */
    Rectangle btnPrev = {rankingBox.x, rankingBox.y + rankingBox.height + 20, 120, 44};
    Rectangle btnNext = {rankingBox.x + rankingBox.width - 120, btnPrev.y, 120, 44};

    bool prevHover = CheckCollisionPointRec(mouse, btnPrev);
    bool nextHover = CheckCollisionPointRec(mouse, btnNext);

    /* fundo + borda */
    DrawRectangleRounded(btnPrev, 0.25f, 6, prevHover ? NEON_GREEN_LT : BG_TRANS);
    DrawRectangleRoundedLines(MENU_BOX_RECT, 0.25f, 6, 2.0f, menuLine); // Aqui, "2.0f" representa a espessura da linha

    DrawRectangleRounded(btnNext, 0.25f, 6, nextHover ? NEON_GREEN_LT : BG_TRANS);
    DrawRectangleRoundedLines(MENU_BOX_RECT, 0.25f, 6, 2.0f, menuLine); // Aqui, "2.0f" representa a espessura da linha

    /* rótulos */
    DrawTextEx(cyberFont, "ant",
               (Vector2){btnPrev.x + 32, btnPrev.y + 8}, 26, 2, WHITE);

    DrawTextEx(cyberFont, "prox",
               (Vector2){btnNext.x + 20, btnNext.y + 8}, 26, 2, WHITE);

    EndDrawing();
}

bool Ranking_Concluido(void)
{
    return fase_concluida;
}

void Unload_Ranking(void)
{
    PlayerCont = 0;
    for (int i = 0; i < MAX_RANKING_PLAYERS; i++)
        memset(&RankingPlayers[i], 0, sizeof(PlayerStatsDTO));
    UnloadSound(clickSound);
}
