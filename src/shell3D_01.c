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

static bool portaAtendida = false;
static bool iniciandoTransicao = false;
static float fadeAlpha = 0.0f;

static float tempoDesdeFadeCompleto = 0.0f;
static bool fadeCompleto = false;
static bool somAbrindoTocado = false;
static bool fase_concluida = false;

void Init_Shell3D_01()
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

void Update_Shell3D_01(void)
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
        }
    }
}

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


bool Fase_Shell3D_01_Concluida(void)
{
    return fase_concluida;
}

void Unload_Shell3D_01(void)
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadTexture(portaTexture);
    UnloadMusicStream(somBaterPorta);
    UnloadSound(somAbrindoPorta);
    EnableCursor();
}
