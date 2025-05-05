#include "raylib.h"
#include "fase1.h"
#include "generalFunctions.h"
#include <math.h>

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

void InitFase1_2(void)
{
    modelo3D = LoadModel("src/models/old-computer.obj");

    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;

    somBaterPorta = LoadMusicStream("src/music/doorKnock.mp3");
    somAbrindoPorta = LoadSound("src/music/doorOpening.mp3"); // NOVO

    tempoDesdeInicio = 0.0f;
    somTocado = false;
    somParado = false;
    portaAtendida = false;
    iniciandoTransicao = false;
    fadeAlpha = 0.0f;
    fadeCompleto = false;
    tempoDesdeFadeCompleto = 0.0f;
    somAbrindoTocado = false;

    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void UpdateFase1_2(void)
{
    const float maxYaw = 45.0f * DEG2RAD;
    const float minYaw = -45.0f * DEG2RAD;

    float delta = GetFrameTime();
    tempoDesdeInicio += delta;

    // Rotação da câmera
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        cameraYaw -= 0.0018f;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        cameraYaw += 0.0018f;

    if (cameraYaw > maxYaw)
        cameraYaw = maxYaw;
    if (cameraYaw < minYaw)
        cameraYaw = minYaw;

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
                    state = INTERROGATORIO;
                }
            }
        }
    }
}

void DrawFase1_2(void)
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

    if (!portaAtendida && cameraYaw >= 30.0f * DEG2RAD && somTocado)
    {
        Rectangle btn = {
            GetScreenWidth() / 2 - 100,
            GetScreenHeight() - 100,
            200,
            40};

        DrawRectangleRec(btn, DARKGREEN);
        DrawText("Atender Porta", btn.x + 20, btn.y + 10, 20, WHITE);

        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
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

void UnloadFase1_2(void)
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadTexture(portaTexture);
    UnloadMusicStream(somBaterPorta);
    UnloadSound(somAbrindoPorta); // NOVO
}
