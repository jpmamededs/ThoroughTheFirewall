#include "transicao_proxy.h"
#include "raylib.h"
#include "generalFunctions.h"
#include "playerStats.h"
#include <string.h>
#include <math.h>
static Model modelo3D;
static Texture2D pergunta_img;
static Texture2D telefone_sprite;
static Texture2D hankFalaSprite; // AJUSTADO NOVO SPRITE
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
static bool personagemAudioTocado   = false;
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
static bool dicaVisivel = false;
static float dicaTimer = 0.0f;
static bool dicaAnimando = false;
static float posicaoDicaX = -300.0f;
static const float velocidadeDica = 300.0f;
static const char *GetCurrentText(TypeWriter *writer)
{
    return writer->text;
}
// --- "3 dias depois" effect ---
static bool introActive   = true;
static float introTimer   = 0.0f;
static float introAlpha   = 0.0f;
static const float INTRO_FADE = 1.5f;
static const float INTRO_HOLD = 2.5f;
static bool fasePrincipalDoProxy = false;
static float autoProceedTimer = -1.0f;
void Init_Transicao_Proxy(void)
{
    modelo3D = LoadModel("src/models/old-computer.obj");
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");
    telefone_sprite = LoadTexture("src/sprites/tel_hank.png");
    hankFalaSprite = LoadTexture("src/sprites/hankFala.png"); // AJUSTADO CARREGA SPRITE
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
    dicaVisivel = false;
    dicaTimer = 0.0f;
    dicaAnimando = false;
    posicaoDicaX = -300.0f;
    // 3 dias depois
    introActive = true;
    introTimer = 0.0f;
    introAlpha = 0.0f;
    fasePrincipalDoProxy = false;
    autoProceedTimer = -1.0f;
}
void Update_Transicao_Proxy(void)
{
    float delta = GetFrameTime();
    // --- INTRO "3 dias depois" ---
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
    if (!fasePrincipalDoProxy)
        return;
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
                "Aqui é o Hank, você deve imaginar que o processo seletivo não será fácil, precisamos de você disponível a qualquer momento. Acabamos \nde detectar um tráfego incomum nos nossos servidores proxy." 
                " Acesse da sua residencia e reconfigure o proxy para reforçar nossa \nsegurança. Siga os passos que deixei no post-it na sua mesa.";
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
            const char *falaP = "Acabei de encontrar o post-it, agora basta configurar tudo.";
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
    if (personagemTypeStarted && 
        personagemWriter.drawnChars >= (int)strlen(GetCurrentText(&personagemWriter)))
    {
        if (autoProceedTimer < 0.0f) // se ainda não iniciou o timer
            autoProceedTimer = 0.0f; // começa a contar
    }
    if (autoProceedTimer >= 0.0f && !done) {
        autoProceedTimer += delta;
        if (autoProceedTimer >= 2.0f) {
            StopSound(somFase1);
            StopSound(somTelefone);
            StopSound(somRadio);
            done = true;
        }
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
    if (tempoDesdeInicio >= 2.0f && !dicaVisivel) 
    {
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
void Draw_Transicao_Proxy()
{
    BeginDrawing();
    // --- INTRO "3 dias depois" ---
    if (introActive || introAlpha > 0.0f) {
        ClearBackground(BLACK);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, introAlpha));
        const char *msg = "Na madrugada do mesmo dia";
        int fSize = 40;
        int txtW = MeasureText(msg, fSize);
        DrawText(msg, (GetScreenWidth() - txtW) / 2, GetScreenHeight() / 2 - fSize / 2, fSize, Fade(WHITE, introAlpha));
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
        // --- AJUSTADO: Desenha a sprite do Hank logo acima da caixa do dialogue ---
        int imgHeight = hankFalaSprite.height;
        int x = 40; // igual a boxX na DrawDialogueBox
        int y = GetScreenHeight() - 220 - imgHeight + 200; // ajuste para ficar logo acima (marginBottom + pequeno espaço)
        float hankScale = 0.6f; // ajuste esse valor como quiser (1.0 = original, 2.0 = dobro, 0.5 = metade)
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
            (Vector2){0, 0},
            0.0f,
            WHITE);
    }
    if (autoProceedTimer > 0.0f && autoProceedTimer < 2.0f)
    {
        const char* msg = "Prosseguindo para o Proxy...";
        int fontSize = 28;
        int textW = MeasureText(msg, fontSize);
        DrawText(msg, (GetScreenWidth() - textW) / 2, GetScreenHeight() - 120, fontSize, LIGHTGRAY);
    }
    if (fadeAlphaFase1 > 0.0f)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                    (Color){0, 0, 0, (unsigned char)(fadeAlphaFase1 * 255)});
    }
    if (dicaVisivel) 
    {
        DrawDica(posicaoDicaX, 20, "Dica: atenda o telefone");
    }
    EndDrawing();
}
bool Transicao_Proxy_Done(void)
{
    return done;
}
void Unload_Transicao_Proxy(void)
{
    UnloadModel(modelo3D);
    UnloadTexture(pergunta_img);
    UnloadTexture(telefone_sprite);
    UnloadTexture(hankFalaSprite); // AJUSTADO: descarrega sprite
    UnloadSound(somFase1);
    UnloadSound(somTelefone);
    UnloadSound(somRadio);
    UnloadSound(somPersonagem);
    UnloadSound(somChamadaAcabada);
    EnableCursor();
}