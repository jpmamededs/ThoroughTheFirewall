#include "pendrive.h"
#include "raylib.h"
#include "generalFunctions.h"
#include <math.h>
#include <string.h>

static Model modelo3D;
static Model portaModel;
static Model usbDriveModel;

static Texture2D portaTexture;
static Texture2D usbTextures[4];

static Camera3D camera;
static float cameraYaw = 0.0f;
static const float maxYaw = PI;
static const float minYaw = -PI;

static bool pendriveSelecionado = false;
static bool pendriveUsavel = false;

static Vector3 usbWorldPos = {-2.0f, 0.5f, 2.0f};

static TypeWriter tw;
static bool mensagemInicialFinalizada = false;

static bool fase_concluida = false; 

static Vector3 Vector3SubtractManual(Vector3 a, Vector3 b) {
    return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}

void Init_Pendrive(void)
{
    modelo3D = LoadModel("src/models/old-computer.obj");
    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;

    usbDriveModel = LoadModel("src/models/UsbDrive_OBJ.obj");
    usbTextures[0] = LoadTexture("src/models/UsbDrive_FBX_UsbDrive_SH_BaseColor.png");
    usbTextures[1] = LoadTexture("src/models/UsbDrive_FBX_UsbDrive_SH_Metalness.png");
    usbTextures[2] = LoadTexture("src/models/UsbDrive_FBX_UsbDrive_SH_Normal.png");
    usbTextures[3] = LoadTexture("src/models/UsbDrive_FBX_UsbDrive_SH_Roughness.png");

    for (int i = 0; i < 4 && i < usbDriveModel.materialCount; i++) {
        usbDriveModel.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture = usbTextures[i];
    }

    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    pendriveSelecionado = false;
    pendriveUsavel = false;
    mensagemInicialFinalizada = false;

    fase_concluida = false;

    InitTypeWriter(&tw, "Ué, onde eu deixei meu pendrive?", 35.0f);
}

void Update_Pendrive(void)
{
    float delta = GetFrameTime();

    if (!mensagemInicialFinalizada) {
        UpdateTypeWriter(&tw, delta, IsKeyPressed(KEY_SPACE));
        if (tw.done) mensagemInicialFinalizada = true;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) cameraYaw -= 0.002f;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) cameraYaw += 0.002f;

    if (cameraYaw > maxYaw) cameraYaw = maxYaw;
    if (cameraYaw < minYaw) cameraYaw = minYaw;

    float distance = 1.0f;
    camera.target.x = camera.position.x + sinf(cameraYaw) * distance;
    camera.target.z = camera.position.z - cosf(cameraYaw) * distance;
    camera.target.y = camera.position.y;

    if (!pendriveSelecionado && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 pendriveScreenPos = GetWorldToScreen(usbWorldPos, camera);
        Vector2 mouse = GetMousePosition();
        float distancia = sqrtf((mouse.x - pendriveScreenPos.x)*(mouse.x - pendriveScreenPos.x) +
                                (mouse.y - pendriveScreenPos.y)*(mouse.y - pendriveScreenPos.y));
        if (distancia < 50.0f) {
            pendriveSelecionado = true;
        }
    }

    if (pendriveSelecionado && fabsf(cameraYaw) < 0.1f) {
        pendriveUsavel = true;
    }
}

void Draw_Pendrive(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);

    DrawModel(modelo3D, (Vector3){0.0f, -0.5f, -2.0f}, 0.05f, WHITE);
    DrawModelEx(portaModel, (Vector3){28.0f, -1.0f, -12.0f}, (Vector3){0, 1, 0}, 45.0f,
                (Vector3){0.05f, 0.05f, 0.05f}, WHITE);

    if (pendriveSelecionado) {
        Vector3 frenteCamera = Vector3SubtractManual(camera.target, camera.position);
        float mag = sqrtf(frenteCamera.x * frenteCamera.x + frenteCamera.y * frenteCamera.y + frenteCamera.z * frenteCamera.z);
        if (mag == 0) mag = 1.0f;

        Vector3 frenteNorm = {
            frenteCamera.x / mag,
            frenteCamera.y / mag,
            frenteCamera.z / mag
        };

        Vector3 pos = {
            camera.position.x + frenteNorm.x * 0.2f + 0.08f,
            camera.position.y + frenteNorm.y * 0.2f - 0.1f,
            camera.position.z + frenteNorm.z * 0.2f
        };

        Vector3 scale = {0.035f, 0.035f, 0.035f};
        DrawModelEx(usbDriveModel, pos, (Vector3){0, 0, 0}, 0.0f, scale, WHITE);
    } else {
        DrawModelEx(usbDriveModel, usbWorldPos, (Vector3){0, 1, 0}, 0.0f,
                    (Vector3){0.05f, 0.05f, 0.05f}, WHITE);
    }

    EndMode3D();

    if (!mensagemInicialFinalizada) {
        char buffer[256] = {0};
        strncpy(buffer, tw.text, tw.drawnChars);
        DrawText(buffer, 40, GetScreenHeight() - 160, 22, WHITE);
    }

    if (pendriveUsavel) {
        Rectangle btn = {
            GetScreenWidth() / 2 - 100,
            GetScreenHeight() - 140,
            200,
            40
        };
        DrawRectangleRec(btn, GREEN);
        DrawText("Usar Pendrive", btn.x + 20, btn.y + 10, 20, WHITE);

        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            fase_concluida = true;
        }
    }

    EndDrawing();
}

bool Fase_Pendrive_Concluida(void)
{
    return fase_concluida;
}

void Unload_Pendrive(void)
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadTexture(portaTexture);
    UnloadModel(usbDriveModel);
    for (int i = 0; i < 4; i++) {
        UnloadTexture(usbTextures[i]);
    }
} 
