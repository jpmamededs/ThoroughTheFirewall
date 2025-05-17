#include "proxy3D.h"
#include "raylib.h"
#include <math.h>
#include "generalFunctions.h"

static Model     modelo3D;
static Model     portaModel;
static Model     serverModel;
static Texture2D portaTexture;
static Texture2D serverTextures[3];
static Texture2D pc2dSprite;      // sprite 2D que aparece no HUD
static Texture2D postitTexture;   // sprite do “post-it”
static Sound     grabbingSound;    // som que toca quando o HUD aparece
static Music     footstepsMusic;   // som de passos em loop (streamed)
static Camera3D  camera;
static float     cameraYaw        = 0.0f;
static Vector3   cameraPosition   = { 0.0f, 1.6f, 0.0f };
static float     moveSpeed        = 10.0f;
static float     cameraPitch      = 0.0f;    // Rotação para cima/baixo
static float     sensitivity      = 0.0022f; // Sensibilidade do mouse
static bool      notebookFollow   = false;
static bool      footstepsPlaying = false;
static const float NOTEBOOK_SCALE = 0.05f;
static const Vector3 NOTEBOOK_POS = { 0.0f, -0.5f, -2.0f };
static Vector3   serverInitialPos;
static bool      fase_concluida   = false;

static float tempoDesdeInicio = 0.0f;
static bool dicaVisivel = false;
static float dicaTimer = 0.0f;
static bool dicaAnimando = false;
static float posicaoDicaX = -300.0f;
static const float velocidadeDica = 300.0f;
static Sound steam_som;
static bool steam_tocando = false;

// ———————————————————
// Post-it animation vars
// ———————————————————
static float     postitPosY;
static float     postitTargetY;
static bool      postitAnimating = false;
static bool      lastInside      = false;
// Posiciona ainda mais abaixo: margem menor
static const int POSTIT_MARGIN_BOTTOM = -20;
// Aumenta a velocidade da animação
static const float POSTIT_SPEED = 10000.0f; // pixels por segundo

// ———————————————————
// Funções utilitárias
// ———————————————————
static Vector3 AddVector3(Vector3 a, Vector3 b) {
    return (Vector3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

static Vector3 ScaleVector3(Vector3 v, float scale) {
    return (Vector3){ v.x * scale, v.y * scale, v.z * scale };
}

static Vector3 NormalizeVector3(Vector3 v) {
    float length = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    if (length == 0.0f) return (Vector3){0,0,0};
    return (Vector3){ v.x/length, v.y/length, v.z/length };
}

void UpdateFootstepsSound(bool moving) {
    if (moving) {
        if (!footstepsPlaying) {
            PlayMusicStream(footstepsMusic);
            footstepsPlaying = true;
        }
    } else {
        if (footstepsPlaying) {
            StopMusicStream(footstepsMusic);
            footstepsPlaying = false;
        }
    }
}

void UpdateCameraPosition() {
    camera.position = cameraPosition;
    Vector3 forward = NormalizeVector3((Vector3){
        cosf(cameraPitch)*sinf(cameraYaw),
        0.0f,
       -cosf(cameraPitch)*cosf(cameraYaw)
    });
    camera.target = AddVector3(camera.position, forward);
}

void UpdateCameraWithMouse(void) {
    Vector2 md = GetMouseDelta();
    cameraYaw   += md.x * sensitivity;
    cameraPitch -= md.y * sensitivity;
    const float maxPitch = 85.0f * DEG2RAD;
    if (cameraPitch >  maxPitch) cameraPitch =  maxPitch;
    if (cameraPitch < -maxPitch) cameraPitch = -maxPitch;
    Vector3 forward = {
        cosf(cameraPitch)*sinf(cameraYaw),
        sinf(cameraPitch),
       -cosf(cameraPitch)*cosf(cameraYaw)
    };
    camera.target = AddVector3(camera.position, forward);
}

void UpdateMovement(float dt) {
    Vector3 forward = NormalizeVector3((Vector3){
        cosf(cameraPitch)*sinf(cameraYaw),
        0.0f,
       -cosf(cameraPitch)*cosf(cameraYaw)
    });
    Vector3 right = NormalizeVector3((Vector3){
        cosf(cameraYaw), 0.0f, sinf(cameraYaw)
    });

    bool moving = false;
    if (IsKeyDown(KEY_W)) {
        cameraPosition = AddVector3(cameraPosition, ScaleVector3(forward,  moveSpeed*dt));
        moving = true;
    }
    if (IsKeyDown(KEY_S)) {
        cameraPosition = AddVector3(cameraPosition, ScaleVector3(forward, -moveSpeed*dt));
        moving = true;
    }
    if (IsKeyDown(KEY_A)) {
        cameraPosition = AddVector3(cameraPosition, ScaleVector3(right,   -moveSpeed*dt));
        moving = true;
    }
    if (IsKeyDown(KEY_D)) {
        cameraPosition = AddVector3(cameraPosition, ScaleVector3(right,    moveSpeed*dt));
        moving = true;
    }
    UpdateFootstepsSound(moving);
}

void Init_Proxy3D(void) 
{
    // Carrega modelos e texturas
    modelo3D       = LoadModel("src/models/old-computer.obj");
    portaModel     = LoadModel("src/models/DOOR.obj");
    portaTexture   = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;
    serverModel    = LoadModel("src/models/server computer.obj");
    serverTextures[0] = LoadTexture("src/models/serverfront.png");
    serverTextures[1] = LoadTexture("src/models/server top, bottom, side.jpg");
    serverTextures[2] = LoadTexture("src/models/Server_back side.jpg");
    for (int i = 0; i < serverModel.materialCount; i++) {
        serverModel.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture = serverTextures[i];
    }

    // Carrega sprites
    pc2dSprite    = LoadTexture("src/sprites/pc2d.png");
    postitTexture = LoadTexture("src/sprites/postit.png");

    // Inicializa animação do post-it (fora da tela)
    postitPosY    = GetScreenHeight() + postitTexture.height + POSTIT_MARGIN_BOTTOM;
    postitTargetY = postitPosY;
    lastInside    = false;
    postitAnimating = false;

    // Carrega sons
    grabbingSound  = LoadSound("src/music/grabbing.mp3");
    steam_som  = LoadSound("src/music/steam-achievement.mp3");
    footstepsMusic = LoadMusicStream("src/music/footsteps.mp3");
    SetMusicVolume(footstepsMusic, 15.0f);
    footstepsMusic.looping = true;

    // Configura câmera
    cameraYaw   = 0.0f;
    cameraPitch = 0.0f;
    camera.position   = cameraPosition;
    camera.target     = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up         = (Vector3){0.0f, 1.0f,  0.0f};
    camera.fovy       = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetMousePosition(GetScreenWidth()/2, GetScreenHeight()/2);
    DisableCursor();

    // Posição inicial do servidor
    serverInitialPos = (Vector3){
        cameraPosition.x,
        cameraPosition.y - 1.8f,
        cameraPosition.z + 20.0f
    };

    fase_concluida = false;
    
    // DICA
    tempoDesdeInicio = 0.0f;
    dicaVisivel = false;
    dicaTimer = 0.0f;
    dicaAnimando = false;
    posicaoDicaX = -300.0f;
    steam_tocando = false;
}

void Update_Proxy3D(void) 
{
    float dt = GetFrameTime();
    tempoDesdeInicio += dt;

    UpdateCameraWithMouse();
    UpdateMovement(dt);
    UpdateCameraPosition();
    UpdateMusicStream(footstepsMusic);

    if (tempoDesdeInicio >= 2.0f && !dicaVisivel) 
    {
        dicaVisivel = true;
        dicaAnimando = true;
    }

    if (dicaVisivel) 
    {
        dicaTimer += dt;

        if (dicaAnimando && dicaTimer < 1.0f) 
        {
            posicaoDicaX += velocidadeDica * dt;
            if (posicaoDicaX >= 20.0f) 
            {
                posicaoDicaX = 20.0f;
                dicaAnimando = false;
            }
        }

        if (dicaTimer >= 5.0f && dicaTimer < 7.0f) 
        {
            dicaAnimando = true;
            posicaoDicaX -= velocidadeDica * dt;
            if (posicaoDicaX <= -420.0f) 
            {
                posicaoDicaX = -422.0f;
                dicaVisivel = false;
            }
        }
    }

    if (!notebookFollow && (IsKeyDown(KEY_W)||IsKeyDown(KEY_S)||IsKeyDown(KEY_A)||IsKeyDown(KEY_D))) {
        notebookFollow = true;
        PlaySound(grabbingSound);
    }

    bool inside =
        fabsf(cameraPosition.z - serverInitialPos.z) <= 5.0f &&
        fabsf(cameraPosition.x - serverInitialPos.x) <= 5.0f;

    if (inside && !lastInside) {
        postitTargetY  = GetScreenHeight() - postitTexture.height - POSTIT_MARGIN_BOTTOM;
        postitAnimating = true;
    } else if (!inside && lastInside) {
        postitTargetY  = GetScreenHeight() + postitTexture.height + POSTIT_MARGIN_BOTTOM;
        postitAnimating = true;
    }
    lastInside = inside;

    if (postitAnimating) {
        float delta = postitTargetY - postitPosY;
        float dir   = (delta > 0) ? 1.0f : -1.0f;
        float move  = dir * POSTIT_SPEED * dt;
        if (fabsf(move) >= fabsf(delta)) {
            postitPosY = postitTargetY;
            postitAnimating = false;
        } else {
            postitPosY += move;
        }
    }

    if (inside && IsKeyPressed(KEY_SPACE)) {
        fase_concluida = true;
        EnableCursor();
        PlaySound(grabbingSound);
    }
}

void Draw_Proxy3D(void) {
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);
    if (!notebookFollow) DrawModel(modelo3D, NOTEBOOK_POS, NOTEBOOK_SCALE, WHITE);
    DrawModelEx(portaModel, (Vector3){28,-1,-12}, (Vector3){0,1,0}, 45, (Vector3){0.05f,0.05f,0.05f}, WHITE);
    DrawModelEx(serverModel, serverInitialPos, (Vector3){0,1,0}, 180, (Vector3){0.01f,0.01f,0.01f}, WHITE);
    EndMode3D();

    if (notebookFollow) {
        int x = -500, y = GetScreenHeight() - pc2dSprite.height - 10;
        DrawTextureEx(pc2dSprite, (Vector2){(float)x,(float)y}, 5.0f, 1.0f, WHITE);
    }

    bool inside =
        fabsf(cameraPosition.z - serverInitialPos.z) <= 5.0f &&
        fabsf(cameraPosition.x - serverInitialPos.x) <= 5.0f;

    if (inside) {
        const char *lbl = "Configurar Proxy [SPACE]";
        int fw = MeasureText(lbl,20), bw = fw+20, bh = 48;
        int bx = (GetScreenWidth()-bw)/2, by = (GetScreenHeight()-bh)/2;
        DrawRectangle(bx,by,bw,bh,DARKGRAY);
        DrawRectangleLines(bx,by,bw,bh,BLACK);
        DrawText(lbl, bx+10, by+(bh-20)/2, 20, WHITE);
    }

    if (inside || postitAnimating) {
        int px = GetScreenWidth() - postitTexture.width - 10;
        DrawTexture(postitTexture, px, (int)postitPosY, WHITE);
    }

    if (dicaVisivel) 
    {
        DrawDica(posicaoDicaX, 20, "Dica: Procure o Servidor Proxy na sala");
        if (!steam_tocando)
        {
            PlaySound(steam_som);
            steam_tocando = true;
        }
    }

    EndDrawing();
}

bool Fase_Proxy3D_Concluida(void) {
    return fase_concluida;
}

void Unload_Proxy3D(void) {
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadModel(serverModel);
    UnloadTexture(portaTexture);
    for (int i = 0; i < 3; i++) UnloadTexture(serverTextures[i]);
    UnloadTexture(pc2dSprite);
    UnloadTexture(postitTexture);
    UnloadSound(grabbingSound);
    UnloadSound(steam_som);
    UnloadMusicStream(footstepsMusic);
}
