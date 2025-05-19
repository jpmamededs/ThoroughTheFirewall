#include "raylib.h"
#include "ligacao_desconhecido.h"
#include "generalFunctions.h"
#include <math.h>
#include "playerStats.h"
extern AppState state;
static Model modelo3D;
static Model portaModel;
static Texture2D portaTexture;
static Music somBaterPorta;
static Sound somAbrindoPorta; // NOVO
static Camera3D camera;
static float cameraYaw = 0.0f;
static bool somTocado = false;
static bool somParado = false;
static float tempoDesdeInicio = 0.0f;
static bool portaAtendida = false;
static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;
static float tempoDesdeFadeCompleto = 0.0f;
static bool fadeCompleto = false;
static bool somAbrindoTocado = false; // NOVO
static const char *introSpeaker = "";
static bool fase_concluida = false; 
// --- introdução “3 dias depois” ---
static bool  introActive   = true;
static float introTimer    = 0.0f;   // cronômetro
static float introAlpha    = 0.0f;   // 0‥1
static const float INTRO_FADE = 1.5f;   // seg. de fade-in / fade-out
static const float INTRO_HOLD = 2.5f;   // seg. texto cheio na tela
// --- diálogo pré-intro ---
static Texture2D pergunta_img;             // mesma arte da Fase1
static TypeWriter introWriter;
static bool  introBoxActive   = true;      // mostra primeiro
static bool  introTypeStarted = false;
static float introBoxTimer    = 0.0f;      // cronômetro
static bool  introTextDone   = false;   // texto completo?
static float postIntroTimer  = 0.0f;    // cronômetro de espera (2 s)
static const float POST_INTRO_DELAY = 6.0f;

//// =============== DICA ================
static bool dicaVisivel = false;
static float dicaTimer = 0.0f;
static bool dicaAnimando = false;
static float posicaoDicaX = -300.0f;
static const float velocidadeDica = 300.0f;
static Sound steam_som;
static bool steam_tocando = false;
//// =====================================

static void DrawDialogueBoxIntro(const TypeWriter *writer)
{
    int boxX = 60;
    int marginBottom = 220;
    int boxY = GetScreenHeight() - marginBottom;
    int boxWidth = GetScreenWidth() - 120;
    int boxHeight = 130;
    int borderRadius = boxHeight / 2;
    int imgW = 1000;
    int imgH = pergunta_img.height - 130;
    int imgX = boxX;
    int imgY = boxY - imgH;
    DrawTexturePro(pergunta_img,
                   (Rectangle){0, 0, pergunta_img.width, pergunta_img.height},
                   (Rectangle){imgX, imgY, imgW, imgH},
                   (Vector2){0, 0}, 0.0f, WHITE);
    // Sem “speaker”: usa string vazia para não aparecer nada
    DrawText(introSpeaker, imgX + 10, imgY + imgH - 30, 26, WHITE);
    DrawRectangle(boxX, boxY, boxWidth - borderRadius, boxHeight,
                  (Color){20, 20, 20, 220});
    DrawCircle(boxX + boxWidth - borderRadius, boxY + borderRadius,
               borderRadius, (Color){20, 20, 20, 220});
    if (writer->drawnChars > 0)
    {
        char tmp[writer->drawnChars + 1];
        strncpy(tmp, writer->text, writer->drawnChars);
        tmp[writer->drawnChars] = '\0';
        DrawText(tmp, boxX + 20, boxY + 30, 26, WHITE);
    }
}
void Init_Porta_Batendo()
{
    modelo3D = LoadModel("src/models/old-computer.obj");
    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;
    somBaterPorta = LoadMusicStream("src/music/batida-de-porta.mp3");
    somAbrindoPorta = LoadSound("src/music/doorOpening.mp3");
    SetMusicVolume(somBaterPorta, 3.0f);
    tempoDesdeInicio = 0.0f;
    somTocado = false;
    somParado = false;
    portaAtendida = false;
    iniciandoTransicao = false;
    fadeAlpha = 0.0f;
    fadeCompleto = false;
    tempoDesdeFadeCompleto = 0.0f;
    somAbrindoTocado = false;
    fase_concluida = false;
    introSpeaker = gSelectedCharacterName; 
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");
    InitTypeWriter(&introWriter, "Isso foi fácil demais!", 18.5f);
    introTypeStarted = true;
    introBoxActive   = true;
    introBoxTimer    = 0.0f;
    introActive      = false;
    introTimer       = 0.0f;
    introAlpha  = 0.0f;
    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    DisableCursor();

    // ==== DICA ====
    steam_som  = LoadSound("src/music/steam-achievement.mp3");
    SetSoundVolume(steam_som, 1.0f);
    dicaVisivel = false;
    dicaTimer = 0.0f;
    dicaAnimando = false;
    posicaoDicaX = -300.0f;
    steam_tocando = false;
    // ==============
}
void Update_Porta_Batendo(void)
{
    const float maxYaw = PI / 4.0f;
    const float minYaw = -PI / 4.0f;
    float delta = GetFrameTime();

    // ---- DICA ----
    if (tempoDesdeInicio >= 2.0f && !dicaVisivel) {
        dicaVisivel = true;
        dicaAnimando = true;
    }
    if (dicaVisivel)
    {
        dicaTimer += delta;
        if (dicaAnimando && dicaTimer < 1.0f)
        {
            posicaoDicaX += velocidadeDica * delta;
            if (posicaoDicaX >= 20.0f)
            {
                posicaoDicaX = 20.0f;
                dicaAnimando = false;
            }
        }
        if (dicaTimer >= 5.0f && dicaTimer < 7.0f)
        {
            dicaAnimando = true;
            posicaoDicaX -= velocidadeDica * delta;
            if (posicaoDicaX <= -420.0f)
            {
                posicaoDicaX = -422.0f;
                dicaVisivel = false;
            }
        }
    }
    // --------------

    /* ---------- Caixa “O que será que isso vai dar?” ---------- */
    if (introBoxActive)
    {
        float delta = GetFrameTime();
        UpdateTypeWriter(&introWriter, delta, IsKeyPressed(KEY_SPACE));
        if (!introTextDone &&
            introWriter.drawnChars >= (int)strlen(introWriter.text))
        {
            introTextDone  = true;
            postIntroTimer = 0.0f;
        }
        if (introTextDone)
        {
            postIntroTimer += delta;
            if (postIntroTimer >= POST_INTRO_DELAY)
            {
                introBoxActive = false;
                introActive    = true;
                introTimer     = 0.0f;
            }
        }
        return;
    }
    if (introActive)
    {
        introTimer += delta;
        if (introTimer <= INTRO_FADE)                         // fade-in
            introAlpha = introTimer / INTRO_FADE;
        else if (introTimer <= INTRO_FADE + INTRO_HOLD)       // mantém 100 %
            introAlpha = 1.0f;
        else if (introTimer <= INTRO_FADE*2 + INTRO_HOLD)     // fade-out
            introAlpha = 1.0f - (introTimer - INTRO_FADE - INTRO_HOLD) / INTRO_FADE;
        else                                                  // termina
        {
            introActive = false;
            introAlpha  = 0.0f;
        }
        if (introActive) return;
    }
    tempoDesdeInicio += delta;
    float mouseDeltaX = GetMouseDelta().x;
    cameraYaw += mouseDeltaX * 0.002f;
    if (cameraYaw > maxYaw) cameraYaw = maxYaw;
    if (cameraYaw < minYaw) cameraYaw = minYaw;
    float distance = 1.0f;
    camera.target.x = camera.position.x + sinf(cameraYaw) * distance;
    camera.target.z = camera.position.z - cosf(cameraYaw) * distance;
    camera.target.y = camera.position.y;
    if (!somTocado && tempoDesdeInicio >= 2.0f)
    {
        PlayMusicStream(somBaterPorta);
        somTocado = true;
    }
    if (!somParado && somTocado)
    {
        UpdateMusicStream(somBaterPorta);
    }
    if (iniciandoTransicao)
    {
        fadeAlpha += delta;
        if (fadeAlpha >= 1.0f)
        {
            fadeAlpha = 1.0f;
            if (!fadeCompleto)
            {
                fadeCompleto = true;
                tempoDesdeFadeCompleto = 0.0f;
                // Toca som da porta abrindo assim que tela fica preta
                if (!somAbrindoTocado)
                {
                    PlaySound(somAbrindoPorta);
                    somAbrindoTocado = true;
                }
            }
            else
            {
                tempoDesdeFadeCompleto += delta;
                if (tempoDesdeFadeCompleto >= 2.0f)
                {
                    fase_concluida = true;
                }
            }
        }
    }
}
void Draw_Porta_Batendo(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera);
    DrawModel(modelo3D, (Vector3){0.0f, -0.5f, -2.0f}, 0.05f, WHITE);
    Vector3 portaPos = {28.0f, -1.0f, -12.0f};
    Vector3 portaRotAxis = {0.0f, 1.0f, 0.0f};
    float portaRotAngle = 45.0f;
    Vector3 portaScale = {0.05f, 0.05f, 0.05f};
    DrawModelEx(portaModel, portaPos, portaRotAxis, portaRotAngle, portaScale, WHITE);
    EndMode3D();
    if (!portaAtendida && cameraYaw >= 41.0f * DEG2RAD && somTocado)
    {
        int fontSize = 20;
        const char* text = "Atender Porta [SPACE]";
        int textWidth = MeasureText(text, fontSize);
        int padding = 20;
        Rectangle btnBounds = {
            GetScreenWidth() / 2 - (textWidth / 2) - padding,
            GetScreenHeight() / 2 + 100,
            textWidth + (2 * padding),
            fontSize + 20
        };
        DrawRectangleRec(btnBounds, GREEN);
        DrawText(text, btnBounds.x + (btnBounds.width - textWidth) / 2,
                btnBounds.y + (btnBounds.height - fontSize) / 2, fontSize, BLACK);
        if (IsKeyPressed(KEY_SPACE))
        {
            StopMusicStream(somBaterPorta);
            somParado = true;
            portaAtendida = true;
            iniciandoTransicao = true;
        }
    }
    if (iniciandoTransicao)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, (unsigned char)(fadeAlpha * 255)});
    }
    /* ---------- INTRO “3 dias depois” ---------- */
    if (introActive || introAlpha > 0.0f)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      Fade(BLACK, introAlpha));
        const char *msg = "3 dias depois";
        int fSize = 40;
        int txtW  = MeasureText(msg, fSize);
        DrawText(msg, (GetScreenWidth() - txtW) / 2, GetScreenHeight() / 2 - fSize / 2, fSize, Fade(WHITE, introAlpha));
    }
    if (introBoxActive)
        DrawDialogueBoxIntro(&introWriter);

    // --- DICA TOPO ESQUERDO ---
    if (dicaVisivel)
    {
        DrawDica(posicaoDicaX, 20, "Dica: atenda a porta");
        if (!steam_tocando)
        {
            PlaySound(steam_som);
            steam_tocando = true;
        }
    }
    // ---------------------------
    EndDrawing();
}
bool Fase_Porta_Batendo_Concluida(void)
{
    return fase_concluida;
}
void Unload_Porta_Batendo(void)
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadTexture(portaTexture);
    UnloadMusicStream(somBaterPorta);
    UnloadSound(somAbrindoPorta);
    UnloadTexture(pergunta_img);
    UnloadSound(steam_som); // dica
    EnableCursor();
}