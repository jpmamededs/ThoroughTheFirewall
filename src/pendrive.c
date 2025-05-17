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
static Vector3 computerWorldPos = { 0.0f, 0.0f, -2.0f };
static bool   mostrarUsarPendrive = false;

static TypeWriter tw;
static bool mensagemInicialFinalizada = false;

static bool fase_concluida = false; 

static float tempoDesdeInicio = 0.0f;
static bool dicaVisivel = false;
static float dicaTimer = 0.0f;
static bool dicaAnimando = false;
static float posicaoDicaX = -300.0f;
static const float velocidadeDica = 300.0f;
static Sound steam_som;
static bool steam_tocando = false;

static bool mostrarPegarPendrive = false;
static const float yawAlvo = 0.15f; 

static Vector3 Vector3SubtractManual(Vector3 a, Vector3 b) {
    return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static float YawDiff(float yawA, float yawB)
{
    float d = yawA - yawB;
    if (d >  PI)  d -= 2.0f * PI;
    if (d < -PI)  d += 2.0f * PI;
    return fabsf(d);
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
    steam_som  = LoadSound("src/music/steam-achievement.mp3");

    for (int i = 0; i < 4 && i < usbDriveModel.materialCount; i++) {
        usbDriveModel.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture = usbTextures[i];
    }

    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    DisableCursor();

    pendriveSelecionado = false;
    pendriveUsavel = false;
    mensagemInicialFinalizada = false;

    fase_concluida = false;

    InitTypeWriter(&tw, "Ué, onde eu deixei meu pendrive?", 35.0f);

    // DICA
    tempoDesdeInicio = 0.0f;
    dicaVisivel = false;
    dicaTimer = 0.0f;
    dicaAnimando = false;
    posicaoDicaX = -300.0f;
    steam_tocando = false;
}

void Update_Pendrive(void)
{
    float dt = GetFrameTime();
    tempoDesdeInicio += dt;

    /* Texto inicial ---------------------------------------------------- */
    if (!mensagemInicialFinalizada) {
        UpdateTypeWriter(&tw, dt, IsKeyPressed(KEY_SPACE));
        if (tw.done) mensagemInicialFinalizada = true;
    }

    /* Rotação da câmera ------------------------------------------------ */
    cameraYaw += GetMouseDelta().x * 0.002f;
    if (cameraYaw > maxYaw) cameraYaw = maxYaw;
    if (cameraYaw < minYaw) cameraYaw = minYaw;

    float dist = 1.0f;
    camera.target = (Vector3){
        camera.position.x + sinf(cameraYaw) * dist,
        camera.position.y,
        camera.position.z - cosf(cameraYaw) * dist
    };

    Vector3 dirPendrive  = Vector3SubtractManual(usbWorldPos,      camera.position);
    Vector3 dirComputer  = Vector3SubtractManual(computerWorldPos, camera.position);

    float yawPendrive = atan2f(dirPendrive.x,  -dirPendrive.z);
    float yawComputer = atan2f(dirComputer.x,  -dirComputer.z);

    /* --- Diferenças ao alvo -------------------------------------- */
    float diffPendrive = YawDiff(yawPendrive, cameraYaw);
    float diffComputer = YawDiff(yawComputer, cameraYaw);

    mostrarPegarPendrive = (!pendriveSelecionado && diffPendrive < yawAlvo);
    mostrarUsarPendrive  = (pendriveSelecionado  && diffComputer < yawAlvo);

    /* Interações ------------------------------------------------------- */
    if (mostrarPegarPendrive && IsKeyPressed(KEY_SPACE))
        pendriveSelecionado = true;

    if (mostrarUsarPendrive && IsKeyPressed(KEY_SPACE))
        fase_concluida = true;

    pendriveUsavel = mostrarUsarPendrive;   // usado no desenho antigo

    /* Dica animada (inalterada) ---------------------------------------- */
    if (tempoDesdeInicio >= 2.0f && !dicaVisivel) { dicaVisivel = dicaAnimando = true; }
    if (dicaVisivel) {
        dicaTimer += dt;
        if (dicaAnimando && dicaTimer < 1.0f) {
            posicaoDicaX += velocidadeDica * dt;
            if (posicaoDicaX >= 20.0f) { posicaoDicaX = 20.0f; dicaAnimando = false; }
        }
        if (dicaTimer >= 5.0f && dicaTimer < 7.0f) {
            dicaAnimando = true;
            posicaoDicaX -= velocidadeDica * dt;
            if (posicaoDicaX <= -420.0f) { posicaoDicaX = -422.0f; dicaVisivel = false; }
        }
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

    if (mostrarPegarPendrive) {
        Rectangle r = { GetScreenWidth()/2 - 160, GetScreenHeight() - 140, 320, 48 };
        Color     c = (Color){  38, 142, 255, 255 };   // azul “neon”
        DrawPromptBox(r, c, "Pegar Pendrive [SPACE]", true);
    }

    if (mostrarUsarPendrive) {
        Rectangle r = { GetScreenWidth()/2 - 140, GetScreenHeight() - 400, 280, 48 };
        Color     c = (Color){  46, 191,  87, 255 };   // verde vibrante
        DrawPromptBox(r, c, "Usar Pendrive [SPACE]", false);
    }

    if (dicaVisivel) 
    {
        DrawDica(posicaoDicaX, 20, "Dica: Procure o Pendrive ao redor");
        if (!steam_tocando)
        {
            PlaySound(steam_som);
            steam_tocando = true;
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
    UnloadSound(steam_som);
    EnableCursor();
} 
