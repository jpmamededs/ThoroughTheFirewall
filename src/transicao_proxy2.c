#include "transicao_proxy2.h"
#include "raylib.h"
#include "generalFunctions.h"
#include "playerStats.h"
#include <string.h>
#include <math.h>
static Model modelo3D;
static Texture2D pergunta_img;
static Texture2D telefone_sprite;
static Texture2D hankFalaSprite;
static Sound somFase1;
static Sound somTelefone;
static Sound somRadio;
static Camera3D camera;
static Model portaModel;
static Texture2D portaTexture;
static float cameraYaw = 0.0f;
static const float maxYaw = PI / 4.0f;
static const float minYaw = -PI / 4.0f;
static bool somFase1Tocado = false;
static bool somRadioTocado = false;
static bool interromperTelefone = false;
static bool telefoneVisivel = false;
static bool animandoTelefone = false;
static bool telefoneSubindo = false;
static bool animacaoFeita = false;
static float animacaoTelefoneY = 0.0f;
static float tempoDesdeInicio = 0.0f;
static float cooldownTelefone = -7.0f;
static Rectangle telefoneBounds = {0};
static float delayTexto = 0.0f;
static float hangUpCooldown = -1.0f;
static TypeWriter fase1Writer;
static bool typeStarted = false;
static float fadeAlphaFase1 = 2.0f;
static const float FADEIN_DURATION = 2.0f;
static bool fase1FadeInDone = false;
static bool telefoneAtendido = false;
static Sound somPersonagem;
static TypeWriter personagemWriter;
static bool personagemTypeStarted = false;
static bool personagemAudioTocado = false;
static bool unknownDone        = false;
static float timeAfterUnknown  = -1.0f;
static float postUnknownTimer = -1.0f;
static Sound somChamadaAcabada;
static bool somChamadaTocado = false;
static float gapTimer = -1.0f;
static bool  gapSoundPlayed = false;
#define RING_GAP 0.60f
static bool done = false;
static const char *characterName = "";
static const char *GetCurrentText(TypeWriter *writer) { return writer->text; }
static bool introActive   = true;
static float introTimer   = 0.0f;
static float introAlpha   = 0.0f;
static const float INTRO_FADE = 1.5f;
static const float INTRO_HOLD = 2.5f;
static bool fasePrincipalDoProxy = false;
static float autoProceedTimer = -1.0f;
static bool transicaoFinalIniciada = false;
static bool transicaoFinalFadeOut = false;
static float transicaoFinalStartTime = 0.0f;
static float transicaoFinalAnimStart = 0.0f;
static bool transicaoFinalAnimTerminada = false;
static bool transicaoFinalFadePosAnim = false;
static float transicaoFinalFadePosAnimStart = 0.0f;
static bool transicaoFinalEsperaPreDone = false;
static Texture2D proxyEmpresa1;
static Texture2D proxyDuplaTex;

static void CarregarDuplaPorPersonagem(void)
{
    if (strcasecmp(gSelectedCharacterName, "Alice") == 0)
        proxyDuplaTex = LoadTexture("src/sprites/dupla.png");
    else if (strcasecmp(gSelectedCharacterName, "Dante") == 0)
        proxyDuplaTex = LoadTexture("src/sprites/dupla2.png");
    else if (strcasecmp(gSelectedCharacterName, "Jade") == 0)
        proxyDuplaTex = LoadTexture("src/sprites/dupla3.png");
    else
        proxyDuplaTex = LoadTexture("src/sprites/dupla4.png");
}
static void UnloadDupla(void)
{
    if (proxyDuplaTex.id != 0)
        UnloadTexture(proxyDuplaTex);
    proxyDuplaTex.id = 0;
}

// =================== AJUSTE TOP FAIXA ==================
static void DrawPanningEmpresa1(float tempo, float duracao)
{
    float panT = tempo / duracao;
    if (panT > 1.0f) panT = 1.0f;
    float panEase = panT * panT * (3.0f - 2.0f * panT);

    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // Calcula area de recorte fonte para SEMPRE preencher tela (letterbox/crop tipo "cover")
    float texW = proxyEmpresa1.width;
    float texH = proxyEmpresa1.height;
    float screenRatio = (float)w / h;
    float texRatio = texW / texH;
    float cropW, cropH, cropX, cropY;
    if (screenRatio > texRatio) {
        cropW = texW;
        cropH = texW / screenRatio;
        cropY = (texH - cropH) * 0.5f;
        cropX = 0;
    } else {
        cropH = texH;
        cropW = texH * screenRatio;
        cropX = (texW - cropW) * 0.5f;
        cropY = 0;
    }

    // Pan horizontal: move cropX à direita
    float maxPan = cropW * 0.07f;
    float pan = maxPan * panEase;
    DrawTexturePro(
        proxyEmpresa1,
        (Rectangle){cropX + pan, cropY, cropW - pan, cropH},
        (Rectangle){0, 0, w, h},
        (Vector2){0,0}, 0.0f, WHITE
    );

    if (proxyDuplaTex.id != 0) {
        float FADESURG = 1.7f;
        float fadeT = tempo / FADESURG;
        if (fadeT > 1.0f) fadeT = 1.0f;
        float fadeEase = fadeT * fadeT * (3.0f - 2.0f * fadeT);
        unsigned char duplaAlpha = (unsigned char)(255 * fadeEase);
        float escBase = 0.65f;
        float spriteW = proxyDuplaTex.width;
        float spriteH = proxyDuplaTex.height;
        float maxW = w * 0.60f, maxH = h * 0.46f;
        float escW = maxW / spriteW, escH = maxH / spriteH;
        float escala = escBase;
        if (escW < escala) escala = escW;
        if (escH < escala) escala = escH;
        float dstW = spriteW * escala;
        float dstH = spriteH * escala;
        float x = (w / 2.0f - dstW/2.0f) - 500;
        float y = (h / 2.0f - dstH/2.0f) + 280;
        DrawTexturePro(
            proxyDuplaTex,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){x, y, dstW, dstH},
            (Vector2){0, 0}, 0.0f, (Color){255,255,255,duplaAlpha}
        );
    }
}
// =======================================================

void Init_Transicao_Proxy2(void)
{
    modelo3D = LoadModel("src/models/old-computer.obj");
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");
    telefone_sprite = LoadTexture("src/sprites/tel_hank.png");
    hankFalaSprite = LoadTexture("src/sprites/hankFala.png");
    somFase1 = LoadSound("src/music/fase1-mateus.wav");
    somTelefone = LoadSound("src/music/telefone.mp3");
    somRadio = LoadSound("src/music/voz-grosa.mp3");
    somPersonagem = LoadSound("");
    somChamadaAcabada = LoadSound("src/music/som_telefone_sinal_desligado_ou_ocupado_caio_audio.mp3");
    characterName = gSelectedCharacterName;
    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;
    SetSoundVolume(somFase1, 1.0f);
    SetSoundVolume(somTelefone, 1.0f);
    SetSoundVolume(somRadio, 3.5f);
    SetSoundVolume(somPersonagem, 1.0f);
    SetMasterVolume(1.0f);
    SetSoundVolume(somChamadaAcabada, 2.0f);
    somFase1Tocado = false;
    somRadioTocado = false;
    interromperTelefone = false;
    telefoneVisivel = false;
    animandoTelefone = false;
    telefoneSubindo = false;
    animacaoFeita = false;
    animacaoTelefoneY = 0.0f;
    tempoDesdeInicio = 0.0f;
    cooldownTelefone = -7.0f;
    delayTexto = 0.0f;
    typeStarted = false;
    hangUpCooldown = -1.0f;
    done = false;
    telefoneAtendido = false;
    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    DisableCursor();
    introActive = true;
    introTimer = 0.0f;
    introAlpha = 0.0f;
    fasePrincipalDoProxy = false;
    autoProceedTimer = -1.0f;
    transicaoFinalIniciada = false;
    transicaoFinalFadeOut = false;
    transicaoFinalStartTime = 0.0f;
    transicaoFinalAnimStart = 0.0f;
    transicaoFinalAnimTerminada = false;
    transicaoFinalFadePosAnim = false;
    transicaoFinalFadePosAnimStart = 0.0f;
    transicaoFinalEsperaPreDone = false;
    proxyEmpresa1 = LoadTexture("src/sprites/intro/empresa1.jpg");
    CarregarDuplaPorPersonagem();
}

void Update_Transicao_Proxy2(void)
{
    float delta = GetFrameTime();
    if (transicaoFinalIniciada) { return; }
    if (introActive) {
        introTimer += delta;
        if (introTimer <= INTRO_FADE)
            introAlpha = introTimer / INTRO_FADE;
        else if (introTimer <= INTRO_FADE + INTRO_HOLD)
            introAlpha = 1.0f;
        else if (introTimer <= INTRO_FADE*2 + INTRO_HOLD)
            introAlpha = 1.0f - (introTimer - INTRO_FADE - INTRO_HOLD) / INTRO_FADE;
        else {
            introActive = false;
            introAlpha  = 0.0f;
            fasePrincipalDoProxy = true;
        }
        return;
    }
    if (!fasePrincipalDoProxy) return;
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
    if (delayTexto > 0.0f)
    {
        delayTexto -= delta;
        if (delayTexto <= 0.0f && !somRadioTocado)
        {
            PlaySound(somRadio);
            somRadioTocado = true;
            const char *fala =
                "Fantástico, você esta indo muito bem. Irei passar agora na sua casa para lhe levar para o proximo desafio, você vai se surpreender \ncom o local. Espero que seu entendimento sobre criptografia esteja em dia, irá por em prática.";
            InitTypeWriter(&fase1Writer, fala, 16.5f);
            typeStarted = true;
        }
    }
    if (typeStarted) UpdateTypeWriter(&fase1Writer, delta, IsKeyPressed(KEY_SPACE));
    if (typeStarted && !unknownDone && fase1Writer.drawnChars >= strlen(GetCurrentText(&fase1Writer)))
    {
        unknownDone       = true;
        timeAfterUnknown  = 0.0f;
    }
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
            const char *falaP = "Combinado, já estou arrumando minhas coisas!";
            InitTypeWriter(&personagemWriter, falaP, 18.5f);
            personagemTypeStarted = true;
        }
    }
    if (typeStarted && !unknownDone &&
        fase1Writer.drawnChars >= (int)strlen(GetCurrentText(&fase1Writer)))
    {
        unknownDone       = true;
        postUnknownTimer  = 0.0f;
        gapTimer     = 0.0f;
        gapSoundPlayed = false;
    }
    if (unknownDone && postUnknownTimer >= 0.0f)
    {
        postUnknownTimer += delta;
        if (postUnknownTimer >= 1.0f && typeStarted)
        {
            typeStarted = false;
            unknownDone = false;
            char *mutableText = (char *)fase1Writer.text;
            mutableText[0] = '\0';
            fase1Writer.drawnChars = 0;
        }
    }
    if (personagemTypeStarted && personagemWriter.drawnChars >= (int)strlen(GetCurrentText(&personagemWriter)))
    {
        if (!transicaoFinalIniciada) {
            transicaoFinalIniciada = true;
            transicaoFinalFadeOut = true;
            transicaoFinalStartTime = GetTime();
            StopSound(somFase1);
            StopSound(somTelefone);
            StopSound(somRadio);
        }
        return;
    }
    if (personagemTypeStarted)
        UpdateTypeWriter(&personagemWriter, delta, IsKeyPressed(KEY_SPACE));
    if (!interromperTelefone && !telefoneAtendido)
    {
        if (IsSoundPlaying(somTelefone))
        {
            cooldownTelefone = 0.0f;
        }
        else
        {
            cooldownTelefone += delta;
            if (cooldownTelefone >= RING_GAP)
            {
                PlaySound(somTelefone);
                telefoneVisivel = true;
                if (!animacaoFeita)
                {
                    animandoTelefone   = true;
                    telefoneSubindo    = true;
                    animacaoTelefoneY  = 1.0f;
                }
                cooldownTelefone = 0.0f;
            }
        }
    }
    else
    {
        if (!telefoneAtendido && IsKeyPressed(KEY_D))
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
    if (telefoneVisivel && !telefoneAtendido && IsKeyPressed(KEY_A))
    {
        telefoneVisivel  = false;
        interromperTelefone = true;
        telefoneAtendido = true;
        if (!typeStarted) delayTexto = 2.3f;
        StopSound(somTelefone);
        animandoTelefone = true;
        telefoneSubindo  = false;
        animacaoTelefoneY = 0.0f;
    }
    else if (telefoneVisivel && !telefoneAtendido && IsKeyPressed(KEY_D))
    {
        StopSound(somTelefone);
        telefoneVisivel  = false;
        animacaoFeita    = false;
        animandoTelefone = false;
        hangUpCooldown   = 0.0f;
        cooldownTelefone = -1.0f;
    }
    if (tempoDesdeInicio >= 4.0f && !somFase1Tocado)
    {
        PlaySound(somFase1);
        somFase1Tocado = true;
    }
    
    float mouseDeltaX = GetMouseDelta().x;
    cameraYaw += mouseDeltaX * 0.002f;
    if (cameraYaw > maxYaw) cameraYaw = maxYaw;
    if (cameraYaw < minYaw) cameraYaw = minYaw;
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
void Draw_Transicao_Proxy2()
{
    BeginDrawing();
    if (transicaoFinalIniciada)
    {
        float tempo = GetTime() - transicaoFinalStartTime;
        const float FADE_DUR = 0.57f;
        const float PAN_DUR = 4.2f;
        const float FADE2_DUR = 1.4f; // ou mais lento
        if (transicaoFinalFadeOut) {
            float fadeT = tempo / FADE_DUR;
            if (fadeT > 1.0f) fadeT = 1.0f;
            ClearBackground(BLACK);
            unsigned char fadeAlpha = (unsigned char)(255 * fadeT);
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0,0,0, fadeAlpha});
            if (fadeT >= 1.0f) {
                transicaoFinalFadeOut = false;
                transicaoFinalAnimStart = GetTime();
            }
            EndDrawing();
            return;
        }
        else if (transicaoFinalFadePosAnim) {
            DrawPanningEmpresa1(PAN_DUR, PAN_DUR);
            float tempoFade = GetTime() - transicaoFinalFadePosAnimStart;
            float fadeT = tempoFade / FADE2_DUR;
            if(fadeT > 1.0f) fadeT = 1.0f;
            unsigned char fadeAlpha = (unsigned char)(255 * fadeT);
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0,0,0,fadeAlpha});
            if (fadeT >= 1.0f && !transicaoFinalEsperaPreDone) {
                transicaoFinalEsperaPreDone = true;
            }
            else if (fadeT >= 1.0f && transicaoFinalEsperaPreDone && !done) {
                done = true;
            }
            EndDrawing();
            return;
        }
        else {
            float animElapsed = GetTime() - transicaoFinalAnimStart;
            float animMax = PAN_DUR;
            if (animElapsed > animMax) animElapsed = animMax;
            DrawPanningEmpresa1(animElapsed, PAN_DUR);
            if (animElapsed >= PAN_DUR && !transicaoFinalAnimTerminada) {
                transicaoFinalAnimTerminada = true;
                transicaoFinalFadePosAnim = true;
                transicaoFinalFadePosAnimStart = GetTime();
                transicaoFinalEsperaPreDone = false;
            }
            EndDrawing();
            return;
        }
    }
    if (introActive || introAlpha > 0.0f)
    {
        ClearBackground(BLACK);
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),Fade(BLACK,introAlpha));
        const char *msg = "Instantes depois";
        int fSize = 40;
        int txtW = MeasureText(msg, fSize);
        DrawText(msg, (GetScreenWidth()-txtW)/2, GetScreenHeight()/2-fSize/2, fSize, Fade(WHITE, introAlpha));
        EndDrawing();
        return;
    }
    ClearBackground(BLACK);
    BeginMode3D(camera);
    DrawModel(modelo3D, (Vector3){0.0f, -0.5f, -2.0f}, 0.05f, WHITE);
    Vector3 portaPos = {28.0f, -1.0f, -12.0f};
    Vector3 portaRotAxis = {0.0f, 1.0f, 0.0f};
    float portaRotAngle = 45.0f;
    Vector3 portaScale = {0.05f, 0.05f, 0.05f};
    DrawModelEx(portaModel, portaPos, portaRotAxis, portaRotAngle, portaScale, WHITE);
    EndMode3D();
    bool drawUnknownNow = (interromperTelefone && typeStarted && !personagemTypeStarted);
    if (drawUnknownNow)
    {
        //int imgWidth = hankFalaSprite.width;
        int imgHeight = hankFalaSprite.height;
        int x = 40;
        int y = GetScreenHeight() - 220 - imgHeight + 200;
        float hankScale = 0.6f;
        DrawTextureEx(hankFalaSprite, (Vector2){x, y}, 0.0f, hankScale, WHITE);
        DrawDialogueBox("Agente Hank", &fase1Writer, 24, 26);
    }
    if (personagemTypeStarted)
    {
        DrawDialogueBox(characterName, &personagemWriter, 24, 26);
    }
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
            (Vector2){0, 0}, 0.0f, WHITE);
    }
    if (fadeAlphaFase1 > 0.0f)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, (unsigned char)(fadeAlphaFase1 * 255)});
    }

    EndDrawing();
}
bool Transicao_Proxy2_Done(void) { return done; }
void Unload_Transicao_Proxy2(void)
{
    UnloadModel(modelo3D);
    UnloadTexture(pergunta_img);
    UnloadTexture(telefone_sprite);
    UnloadTexture(hankFalaSprite);
    UnloadSound(somFase1);
    UnloadSound(somTelefone);
    UnloadSound(somRadio);
    UnloadSound(somPersonagem);
    UnloadSound(somChamadaAcabada);
    UnloadTexture(proxyEmpresa1);
    UnloadDupla();
    EnableCursor();
}