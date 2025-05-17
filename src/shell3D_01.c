<<<<<<< HEAD:src/template_3D_02.c
=======
#include "shell3D_01.h"
>>>>>>> 190e95519dad26732afe71103d226417edf3e2c5:src/shell3D_01.c
#include "raylib.h"
#include "generalFunctions.h"
#include "playerStats.h"
#include <math.h>

static Model modelo3D;
static Model portaModel;
static Texture2D portaTexture;
static Music somBaterPorta;
static Sound somAbrindoPorta;

static Camera3D camera;

static float cameraYaw = 0.0f;
static bool somTocado = false;
static bool somParado = false;
static float tempoDesdeInicio = 0.0f;

<<<<<<< HEAD:src/template_3D_02.c
static bool portaAtendida = false;
static bool iniciandoTransicao = false;
=======
static bool fase_concluida = false;
static float timeElapsed = 0.0f;
static bool soundPlaying = false;
static bool doorAnswered = false;

// Variáveis do Flash e Fade
>>>>>>> 190e95519dad26732afe71103d226417edf3e2c5:src/shell3D_01.c
static float fadeAlpha = 0.0f;

<<<<<<< HEAD:src/template_3D_02.c
static float tempoDesdeFadeCompleto = 0.0f;
static bool fadeCompleto = false;
static bool somAbrindoTocado = false;
static bool fase_concluida = false;

void Init_Template_3D_02()
=======
void Init_Shell3D_01(void)
>>>>>>> 190e95519dad26732afe71103d226417edf3e2c5:src/shell3D_01.c
{
    modelo3D = LoadModel("src/models/old-computer.obj");

    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;

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
}

<<<<<<< HEAD:src/template_3D_02.c
void Update_Template_3D_02(void)
{
    float delta = GetFrameTime();
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
=======
void StartFadeOut()
{
    isFading = true;
    fadeAlpha = 1.0f;
    fadeComplete = false;
    flashTimer = 0.0f;
    flashActive = true;
}

void UpdateFadeOut(float deltaTime)
{
    if (flashActive)
    {
        flashTimer += deltaTime;

        // Fase 1: Flash Branco Intenso
        if (flashTimer < flashHoldTime)
        {
            fadeAlpha = 1.0f;
        }
        else
        {
            // Fase 2: Transição para a Tela Preta (Fade Out)
            fadeAlpha -= fadeSpeed * deltaTime;
            if (fadeAlpha <= 0.0f)
            {
                fadeAlpha = 0.0f;
                isFading = false;
                fadeComplete = true;
                flashActive = false;

                // Ativar a exibição da caixa fechada após o fade
                showBox = true;

                // Tocar o som após o fade estar completo
                PlaySound(doorOpeningSound);
            }
        }
    }
}

void Update_Shell3D_01(void)
{
    float deltaTime = GetFrameTime();
    timeElapsed += deltaTime;
    
    float mouseDeltaX = GetMouseDelta().x;
    cameraYaw += mouseDeltaX * 0.002f;
    cameraYaw = fmaxf(fminf(cameraYaw, maxYaw), minYaw);

    camera.target.x = camera.position.x + sinf(cameraYaw);
    camera.target.z = camera.position.z - cosf(cameraYaw);
    
    if (fadeComplete)
    {
        fase_concluida = true;
        return;
    }
    
    UpdateFadeOut(deltaTime);

    if (!isFading && timeElapsed >= 2.0f && !doorAnswered) 
    {
        if (!soundPlaying)
        {
            PlaySound(doorKnockSound);
            soundPlaying = true;
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            if (IsSoundPlaying(doorKnockSound))
            {
                StopSound(doorKnockSound);
                soundPlaying = false;
            }

            doorAnswered = true;
            StartFadeOut();
>>>>>>> 190e95519dad26732afe71103d226417edf3e2c5:src/shell3D_01.c
        }
    }
}

void Draw_Shell3D_01(void)
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
        DrawText(text, btnBounds.x + (btnBounds.width - textWidth) / 2, btnBounds.y + (btnBounds.height - fontSize) / 2, fontSize, BLACK);

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

    EndDrawing();
}

<<<<<<< HEAD:src/template_3D_02.c
bool Fase_Template_3D_02_Concluida(void)
{
    return fase_concluida;
}

void Unload_Template_3D_02(void)
=======
void Unload_Shell3D_01(void)
>>>>>>> 190e95519dad26732afe71103d226417edf3e2c5:src/shell3D_01.c
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadTexture(portaTexture);
    UnloadMusicStream(somBaterPorta);
    UnloadSound(somAbrindoPorta);
    EnableCursor();
}
<<<<<<< HEAD:src/template_3D_02.c
=======

bool Fase_Shell3D_01_Concluida(void)
{
    return fase_concluida;
}
>>>>>>> 190e95519dad26732afe71103d226417edf3e2c5:src/shell3D_01.c
