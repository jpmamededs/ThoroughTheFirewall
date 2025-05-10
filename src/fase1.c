#include "fase1.h"
#include "raylib.h"
#include "generalFunctions.h"
#include <string.h>
#include <math.h>

extern AppState state;

static Model modelo3D;
static Texture2D pergunta_img;
static Texture2D telefone_sprite;
static Sound somFase1;
static Sound somTelefone;
static Sound somRadio;
static Camera3D camera;

static Model portaModel;
static Texture2D portaTexture;

static float cameraYaw = 0.0f;
static const float maxYaw = 45.0f * DEG2RAD;
static const float minYaw = -45.0f * DEG2RAD;

static bool somFase1Tocado = false;
static bool somRadioTocado = false;
static bool interromperTelefone = false;
static bool telefoneVisivel = false;
static bool animandoTelefone = false;
static bool telefoneSubindo = false;
static bool animacaoFeita = false;
static float animacaoTelefoneY = 0.0f;
static float tempoDesdeInicio = 0.0f;
static float cooldownTelefone = -5.0f;
static Rectangle telefoneBounds = {0};
static float delayTexto = 0.0f;
static float hangUpCooldown = -1.0f;
static Rectangle btnHangUpBounds = {0};  

static TypeWriter fase1Writer;
static bool typeStarted = false;
static Music typingMusicF1 = {0};
static bool typingLoaded = false;

static float fadeAlphaFase1 = 2.0f;
static const float FADEIN_DURATION = 2.0f;
static bool fase1FadeInDone = false;
static bool showComputerButton = false;

static Sound somPersonagem;            // áudio do personagem
static TypeWriter personagemWriter;    // typewriter do personagem
static bool personagemTypeStarted = false;
static bool personagemAudioTocado   = false;
static bool unknownDone        = false;   // texto do desconhecido concluído?
static float timeAfterUnknown  = -1.0f; 
static float postUnknownTimer = -1.0f;

static Sound somChamadaAcabada;
static bool somChamadaTocado = false; 
static float gapTimer = -1.0f;        // cronômetro entre Fala-1 e Fala-2
static bool  gapSoundPlayed = false;

const char *GetCurrentText(TypeWriter *writer)
{
    return writer->text;
}

void InitFase1(void)
{
    modelo3D = LoadModel("src/models/old-computer.obj");
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");
    telefone_sprite = LoadTexture("src/sprites/phone.png");

    somFase1 = LoadSound("src/music/fase1-mateus.wav");
    somTelefone = LoadSound("src/music/telefone.mp3");
    somRadio = LoadSound("src/music/voz-grosa.mp3");
    somPersonagem = LoadSound(""); // se quiser voz no cara, é so colocar o caminho aqui
    somChamadaAcabada = LoadSound("src/music/som_telefone_sinal_desligado_ou_ocupado_caio_audio.mp3");
    
    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;

    SetSoundVolume(somFase1, 1.0f);
    SetSoundVolume(somTelefone, 1.0f);
    SetSoundVolume(somRadio, 3.5f);
    SetSoundVolume(somPersonagem, 1.0f);    
    SetMasterVolume(1.0f);
    SetSoundVolume(somChamadaAcabada, 2.0f);

    // Reset de estado
    somFase1Tocado = false;
    somRadioTocado = false;
    interromperTelefone = false;
    telefoneVisivel = false;
    animandoTelefone = false;
    telefoneSubindo = false;
    animacaoFeita = false;
    animacaoTelefoneY = 0.0f;
    tempoDesdeInicio = 0.0f;
    cooldownTelefone = -5.0f;
    delayTexto = 0.0f;
    typeStarted = false;
    hangUpCooldown = -1.0f;

    // Câmera
    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void UpdateFase1(void)
{
    float delta = GetFrameTime();
    tempoDesdeInicio += delta;
    
    if (!fase1FadeInDone)
    {
        fadeAlphaFase1 = UpdateFade(delta, FADEIN_DURATION, true);
        if (fadeAlphaFase1 <= 0.0f) {
            fadeAlphaFase1 = 0.0f;
            fase1FadeInDone = true;
        }
    }

    if (hangUpCooldown >= 0.0f) {
        hangUpCooldown += delta;
        if (hangUpCooldown >= 5.0f) {
            hangUpCooldown = -1.0f;
            cooldownTelefone = 0.0f;
        }
    }

    // Inicia fala digitada após delay
    if (delayTexto > 0.0f)
    {
        delayTexto -= delta;
        if (delayTexto <= 0.0f && !somRadioTocado)
        {
            // Inicia o áudio e a caixa juntos após o delay
            PlaySound(somRadio);
            somRadioTocado = true;

            const char *fala =
                "Atenção! você foi selecionado para um processo ultrassecreto. "
                "Antes de prosseguirmos, preciso confirmar que suas habilidades\n"
                "estão à altura. Mostre que é capaz de passar pelo firewall que "
                "acabei de enviar para o seu computador e faça isso sem ser detectado.";

            InitTypeWriter(&fase1Writer, fala, 16.5f);
            typeStarted = true;
        }
    }

    if (typeStarted)
        UpdateTypeWriter(&fase1Writer, delta, IsKeyPressed(KEY_SPACE));
    if (typingLoaded)
        UpdateMusicStream(typingMusicF1);

    if (typeStarted && !unknownDone && fase1Writer.drawnChars >= strlen(GetCurrentText(&fase1Writer)))
    {
        unknownDone       = true;
        timeAfterUnknown  = 0.0f;   // inicia a contagem de 2 s
    }

    /* 3.2 – contar 2 s e disparar fala do personagem */
    if (unknownDone && !personagemAudioTocado)
    {
        if (!somChamadaTocado) {
            PlaySound(somChamadaAcabada);
            somChamadaTocado = true;
        }

        timeAfterUnknown += delta;
        if (timeAfterUnknown >= 5.0f)
        {
            PlaySound(somPersonagem);
            personagemAudioTocado = true;

            const char *falaP = "O quê? Uma ligação assim do nada...? Isso parece suspeito... Melhor verificar isso direito.";    // texto do personagem
            InitTypeWriter(&personagemWriter, falaP, 18.5f);
            personagemTypeStarted = true;
        }
    }

    /********* Fim do texto do DESCONHECIDO *********/
    if (typeStarted && !unknownDone &&
        fase1Writer.drawnChars >= (int)strlen(GetCurrentText(&fase1Writer)))
    {
        unknownDone       = true;
        postUnknownTimer  = 0.0f;  
        gapTimer     = 0.0f;          // começa intervalo
        gapSoundPlayed = false;        // começa contagem
    }

    /********* Contagem após término *********/
    if (unknownDone && postUnknownTimer >= 0.0f)
    {
        postUnknownTimer += delta;

        /* 1 s depois: apaga o texto anterior e a caixa */
        if (postUnknownTimer >= 1.0f && typeStarted)
        {
            typeStarted = false;
            unknownDone = false;

            // Limpeza da string do TypeWriter
            char *mutableText = (char *)fase1Writer.text;
            mutableText[0] = '\0';
            fase1Writer.drawnChars = 0;
        }
    }

    // Verifica se a fala 2 terminou para habilitar o botão
    if (personagemTypeStarted && 
        personagemWriter.drawnChars >= (int)strlen(GetCurrentText(&personagemWriter)))
    {
        showComputerButton = true;
    }

    /* 3.3 – atualizar o TypeWriter do personagem (se ativo) */
    if (personagemTypeStarted)
        UpdateTypeWriter(&personagemWriter, delta, IsKeyPressed(KEY_SPACE));

    // Toca telefone automaticamente com cooldown
    if (!interromperTelefone)
    {
        cooldownTelefone += delta;
        if (!IsSoundPlaying(somTelefone) && cooldownTelefone >= 0.0f)
        {
            PlaySound(somTelefone);
            cooldownTelefone = -9999.0f;
            telefoneVisivel = true;
            if (!animacaoFeita)
            {
                animandoTelefone = true;
                telefoneSubindo = true;
                animacaoTelefoneY = 1.0f;
            }
        }
    }
    else
    {
        Vector2 mouse = GetMousePosition();

        int boxX = 60;
        int marginBottom = 220;
        int boxY = GetScreenHeight() - marginBottom;
        int boxWidth = GetScreenWidth() - 120;
        int btnW = 48, btnH = 48;
        btnHangUpBounds = (Rectangle){
            boxX + boxWidth - btnW - 20,   // canto direito da caixa
            boxY - btnH - 10,              // um pouco acima
            btnW, btnH
        };

        if (CheckCollisionPointRec(mouse, btnHangUpBounds) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            StopSound(somRadio);
            StopSound(somTelefone);

            interromperTelefone = false;
            telefoneVisivel     = false;
            animacaoFeita       = false;
            animandoTelefone    = false;
            telefoneSubindo     = false;

            hangUpCooldown   = 0.0f; 
            cooldownTelefone = -5.0f; 
            typeStarted      = false;
        }
    }

    Vector2 mouse = GetMousePosition();

    if (telefoneVisivel &&
        CheckCollisionPointRec(mouse, telefoneBounds) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        bool clicouEsquerda = (mouse.x < telefoneBounds.x + telefoneBounds.width * 0.5f);
    
        if (clicouEsquerda)
        {
            interromperTelefone = true;
    
            if (!typeStarted) delayTexto = 2.3f;
    
            StopSound(somTelefone);
    
            animandoTelefone = true;
            telefoneSubindo  = false;
            animacaoTelefoneY = 0.0f;
        }
        else
        {
            StopSound(somTelefone);
            telefoneVisivel  = false;
            animacaoFeita    = false;
            animandoTelefone = false;
    
            hangUpCooldown   = 0.0f;
            cooldownTelefone = -1.0f;
        }
    }

    if (tempoDesdeInicio >= 4.0f && !somFase1Tocado)
    {
        PlaySound(somFase1);
        somFase1Tocado = true;
    }

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

    if (animandoTelefone)
    {
        float speed = 2.5f * delta;
        animacaoTelefoneY += (telefoneSubindo ? -speed : speed);

        if ((telefoneSubindo && animacaoTelefoneY <= 0.0f) || (!telefoneSubindo && animacaoTelefoneY >= 1.0f))
        {
            animacaoTelefoneY = telefoneSubindo ? 0.0f : 1.0f;
            animandoTelefone = false;
            animacaoFeita = true;
            if (!telefoneSubindo)
                telefoneVisivel = false;
        }
    }
}

static void DrawDialogueBox(const char *speaker,
                            const TypeWriter *writer,
                            int fontTitle,
                            int fontBody)
{
    int boxX = 60;
    int marginBottom = 220;
    int boxY = GetScreenHeight() - marginBottom;
    int boxWidth = GetScreenWidth() - 120;
    int boxHeight = 130;
    int borderRadius = boxHeight / 2;

    /* retrato + balão */
    int imgW = 1000;
    int imgH = pergunta_img.height - 130;
    int imgX = boxX;
    int imgY = boxY - imgH;
    DrawTexturePro(pergunta_img, (Rectangle){0, 0, pergunta_img.width, pergunta_img.height},
                   (Rectangle){imgX, imgY, imgW, imgH}, (Vector2){0, 0}, 0.0f, WHITE);

    DrawText(speaker, imgX + 10, imgY + imgH - 30, fontTitle, WHITE);

    DrawRectangle(boxX, boxY, boxWidth - borderRadius, boxHeight,
                  (Color){20, 20, 20, 220});
    DrawCircle(boxX + boxWidth - borderRadius, boxY + borderRadius,
               borderRadius, (Color){20, 20, 20, 220});

    if (writer->drawnChars > 0)
    {
        char tmp[writer->drawnChars + 1];
        strncpy(tmp, GetCurrentText((TypeWriter *)writer), writer->drawnChars);
        tmp[writer->drawnChars] = '\0';
        DrawText(tmp, boxX + 20, boxY + 30, fontBody, WHITE);
    }
}

void DrawFase1(const char *nome)
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

    // Caixa de fala - Condição corrigida
    bool drawUnknownNow = (interromperTelefone && typeStarted && !personagemTypeStarted);

    if (drawUnknownNow)
    {
        DrawDialogueBox("???", &fase1Writer, 24, 26);
    }

    // Caixa de fala do personagem
    if (personagemTypeStarted)
    {
        DrawDialogueBox(nome, &personagemWriter, 24, 26);
    }

    // Telefone animado
    if (telefoneVisivel)
    {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        int spriteW = 389;
        int spriteH = 803;
        float scale = 1.25f;
        int destW = spriteW * scale;
        int destH = spriteH * scale * 0.75f;
        int destX = screenWidth - destW - 180;
        int destYFinal = screenHeight - destH;
        int deslocamentoY = (int)(animacaoTelefoneY * (screenHeight + destH));
        int destY = telefoneSubindo ? destYFinal + deslocamentoY : destYFinal + deslocamentoY;
        if (!animandoTelefone)
            destY = destYFinal;

        telefoneBounds = (Rectangle){destX, destY, destW, destH};

        DrawTexturePro(
            telefone_sprite,
            (Rectangle){0, 0, spriteW, spriteH * 0.75f},
            telefoneBounds,
            (Vector2){0, 0},
            0.0f,
            WHITE);
    }

    // Botão "Usar Computador"
    if (showComputerButton)
    {
        Rectangle btnBounds = {
            GetScreenWidth() / 2 - 100,
            GetScreenHeight() / 2 + 100,
            200,
            50
        };

        DrawRectangleRec(btnBounds, GREEN);
        DrawText("Usar Computador", btnBounds.x + 20, btnBounds.y + 15, 20, BLACK);

        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, btnBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            StopSound(somFase1);
            StopSound(somTelefone);
            StopSound(somRadio);

            state = APP_PC_SCREEN;
        }
    }

    if (fadeAlphaFase1 > 0.0f)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    (Color){0, 0, 0, (unsigned char)(fadeAlphaFase1 * 255)});
    }

    EndDrawing();
}

void UnloadFase1(void)
{
    UnloadModel(modelo3D);
    UnloadTexture(pergunta_img);
    UnloadTexture(telefone_sprite);
    UnloadSound(somFase1);
    UnloadSound(somTelefone);
    UnloadSound(somRadio);
    UnloadSound(somPersonagem);
    UnloadSound(somChamadaAcabada);
}
