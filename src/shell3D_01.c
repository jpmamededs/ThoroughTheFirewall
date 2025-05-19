#include "raylib.h"
#include "generalFunctions.h"
#include "playerStats.h"  // Inclui o array do nome do personagem
#include <math.h>
#include <string.h>

// --- GLOBAIS ---
static Model modelo3D;
static Model portaModel;
static Texture2D portaTexture;
static Texture2D pergunta_img;
static Music somBaterPorta;
static Sound somAbrindoPorta;
static Camera3D camera;
static float cameraYaw = 0.0f;
static bool somTocado = false;
static bool somParado = false;
static float tempoDesdeInicio = 0.0f;
static bool promptLiberado = false;
static bool portaAtendida = false;
static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;
static float tempoDesdeFadeCompleto = 0.0f;
static bool fadeCompleto = false;
static bool somAbrindoTocado = false;
static bool fase_concluida = false;
static TypeWriter personagemWriter;
static bool personagemTypeStarted = false;
static bool personagemTypeVisible = false; // <--- CONTROLA VISIBILIDADE DO TEXTO

// ================ DICA ================
static bool dicaVisivel = false;
static float dicaTimer = 0.0f;
static bool dicaAnimando = false;
static float posicaoDicaX = -300.0f;
static const float velocidadeDica = 300.0f;
static Sound steam_som;
static bool steam_tocando = false;
// =======================================

// --- FUNÇÃO: DRAW DIALOGUE BOX ---
static void DrawDialogueBox(const char *speaker, const TypeWriter *writer, int fontTitle, int fontBody)
{
    int boxX = 60;
    int marginBottom = 220;
    int boxY = GetScreenHeight() - marginBottom;
    int boxWidth = GetScreenWidth() - 120;
    int boxHeight = 130;
    int borderRadius = boxHeight / 2;
    int imgW = pergunta_img.width;
    int imgH = pergunta_img.height > 150 ? pergunta_img.height - 130 : pergunta_img.height;
    int imgX = boxX;
    int imgY = boxY - imgH;
    // Caixa sprite (ou fallback verde)
    if (pergunta_img.width > 0 && pergunta_img.height > 0) {
        DrawTexturePro(pergunta_img, (Rectangle){0, 0, pergunta_img.width, pergunta_img.height},
                       (Rectangle){imgX, imgY, imgW, imgH}, (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        DrawRectangle(imgX, imgY, imgW, imgH, DARKGREEN);
        DrawText("Sprite NAO carregada!", imgX + 20, imgY + 20, 24, RED);
    }
    // Nome do personagem
    DrawText(speaker, imgX + 10, imgY + imgH - 30, fontTitle, WHITE);
    // Caixa arredondada
    DrawRectangle(boxX, boxY, boxWidth - borderRadius, boxHeight, (Color){20, 20, 20, 220});
    DrawCircle(boxX + boxWidth - borderRadius, boxY + borderRadius, borderRadius, (Color){20, 20, 20, 220});
    // Texto
    if (writer && writer->drawnChars > 0)
    {
        char tmp[writer->drawnChars + 1];
        strncpy(tmp, writer->text, writer->drawnChars);
        tmp[writer->drawnChars] = '\0';
        DrawText(tmp, boxX + 20, boxY + 30, fontBody, WHITE);
    }
}

// --- INIT ---
void Init_Shell3D_01()
{
    modelo3D = LoadModel("src/models/old-computer.obj");
    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");
    TraceLog(LOG_INFO, "pergunta_img: %dx%d", pergunta_img.width, pergunta_img.height); // debug
    somBaterPorta = LoadMusicStream("src/music/batida-de-porta.mp3");
    somAbrindoPorta = LoadSound("src/music/doorOpening.mp3");
    SetMusicVolume(somBaterPorta, 3.0f);

    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    DisableCursor();
    promptLiberado = false;
    portaAtendida = false;
    personagemTypeStarted = true;
    personagemTypeVisible = true;  // <--- VISÍVEL NO INÍCIO
    iniciandoTransicao = false;
    fadeAlpha = 0.0f;
    fadeCompleto = false;
    somAbrindoTocado = false;
    fase_concluida = false;
    tempoDesdeFadeCompleto = 0.0f;
    tempoDesdeInicio = 0.0f;
    cameraYaw = 0.0f;
    somTocado = false;
    somParado = false;
    // INICIA O TEXTO IMEDIATO
    InitTypeWriter(&personagemWriter, "Quem esta batendo na porta esta hora??", 17.0f);

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

// --- UPDATE ---
void Update_Shell3D_01(void)
{
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

    tempoDesdeInicio += delta;
    float mouseDeltaX = GetMouseDelta().x;
    cameraYaw += mouseDeltaX * 0.002f;
    const float maxYaw = PI / 4.0f;
    const float minYaw = -PI / 4.0f;
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
        UpdateMusicStream(somBaterPorta);

    // Typewriter roda normalmente, VISÍVEL até liberar prompt ao olhar para a porta
    if (personagemTypeStarted)
        UpdateTypeWriter(&personagemWriter, delta, IsKeyPressed(KEY_SPACE));
    // SÓ ao terminar a fala E olhar para a porta libera o prompt
    if (personagemTypeStarted
        && personagemWriter.drawnChars >= (int)strlen(personagemWriter.text)
        && cameraYaw >= 41.0f * DEG2RAD)
    {
        personagemTypeVisible = false;
        personagemTypeStarted = false;
        promptLiberado = true;
    }
    // Depois disso (prompt de atender aparece, controlar normalmente)
    if (!portaAtendida && promptLiberado)
    {
        if (IsKeyPressed(KEY_SPACE)) {
            StopMusicStream(somBaterPorta);
            somParado = true;
            portaAtendida = true;
            iniciandoTransicao = true;
        }
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
                    fase_concluida = true;
            }
        }
    }
}

// --- DRAW ---
void Draw_Shell3D_01(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera);
    Vector3 portaPos = {28.0f, -1.0f, -12.0f};
    Vector3 portaRotAxis = {0.0f, 1.0f, 0.0f};
    float portaRotAngle = 45.0f;
    Vector3 portaScale = {0.05f, 0.05f, 0.05f};
    DrawModel(modelo3D, (Vector3){0.0f, -0.5f, -2.0f}, 0.05f, WHITE);
    DrawModelEx(portaModel, portaPos, portaRotAxis, portaRotAngle, portaScale, WHITE);
    EndMode3D();
    // Caixa box de fala visível ENQUANTO personagemTypeVisible
    if (personagemTypeVisible)
    {
        // Usa o array gSelectedCharacterName declarado em playerStats.h
        const char *nome = (gSelectedCharacterName[0]) ? gSelectedCharacterName : "???";
        DrawDialogueBox(nome, &personagemWriter, 24, 26);
    }
    // Prompt SÓ aparece depois da fala TERMINAR e após olhar para a porta
    if (!portaAtendida && promptLiberado)
    {
        int fontSize = 20;
        const char* text = "Atender porta [SPACE]";
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
    }
    if (iniciandoTransicao)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      (Color){0, 0, 0, (unsigned char)(fadeAlpha * 255)});
    }
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

bool Fase_Shell3D_01_Concluida(void)
{
    return fase_concluida;
}

void Unload_Shell3D_01(void)
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadTexture(portaTexture);
    UnloadTexture(pergunta_img);
    UnloadMusicStream(somBaterPorta);
    UnloadSound(somAbrindoPorta);
    UnloadSound(steam_som); // dica
    EnableCursor();
}