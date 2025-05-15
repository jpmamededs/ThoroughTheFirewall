#include "tela_provisoria_02.h"
#include "raylib.h"
#include "generalFunctions.h"
#include "playerStats.h"

// ESSA É A TELA DO RESULTADO DO PROCESSO SELETIVO

static bool fase_concluida = false;

// Definir as variáveis para armazenar as texturas dos sprites
static Texture2D carinhaRevelationSilhouette;
static Texture2D carinhaRevelation;
static Texture2D meninaRevelationSilhouette;
static Texture2D calvoRevelationSilhouette;
static Texture2D deBoneRevelationSilhouette;
static Texture2D deBoneRevelation; // Nova textura para Jade

// Definir a variável para carregar a música
static Music writtenInTheStars;

// Variáveis para controle de movimento
static float offsetXIncrement = 0.0f;
static float tempoMovimento = 0.0f;
static bool movimentoFinalizado = false;

// Variáveis para controle de troca de sprites
static float tempoTrocaSprite = 0.0f;
static bool spriteTrocaRealizada = false;
static bool deBoneTrocaRealizada = false; // Nova flag para controle de Jade

// Variáveis para animação do carinha
static float transicaoTempo = 0.0f;
static float scaleCarinha = 0.4f;
static float offsetCarinha = 0.0f;

// Variáveis para animação do deBone (Jade)
static float transicaoDeBoneTempo = 0.0f;
static float scaleDeBone = 1.0f;
static float offsetDeBone = 0.0f;

// Variáveis para efeito de flash
static float flashAlpha = 0.0f;
static bool flashAtivo = false;
static float flashDuration = 0.6f;
static float flashTimer = 0.0f;

void Init_Tela_02(void)
{
    fase_concluida = false;

    carinhaRevelationSilhouette = LoadTexture("src/sprites/revelation/carinhaRevelationSilhouette.png");
    carinhaRevelation = LoadTexture("src/sprites/revelation/carinhaRevelation.png");
    meninaRevelationSilhouette = LoadTexture("src/sprites/revelation/meninaRevelationSilhouette.png");
    calvoRevelationSilhouette = LoadTexture("src/sprites/revelation/calvoRevelationSilhouette.png");
    deBoneRevelationSilhouette = LoadTexture("src/sprites/revelation/deBoneRevelationSilhouette.png");
    deBoneRevelation = LoadTexture("src/sprites/revelation/deBoneRevelation.png"); // Carregar o novo sprite para Jade

    writtenInTheStars = LoadMusicStream("src/music/writtenInTheStars.mp3");
    PlayMusicStream(writtenInTheStars);
}

void Update_Tela_02(void)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        fase_concluida = true;
    }

    UpdateMusicStream(writtenInTheStars);

    if (!movimentoFinalizado)
    {
        tempoMovimento += GetFrameTime();
        offsetXIncrement += 0.05f;

        if (tempoMovimento >= 13.0f)
        {
            movimentoFinalizado = true;
        }
    }

    // Lógica para Dante (carinha)
    if (strcmp(gSelectedCharacterName, "Dante") == 0 && !spriteTrocaRealizada)
    {
        tempoTrocaSprite += GetFrameTime();

        if (tempoTrocaSprite >= 13.0f)
        {
            spriteTrocaRealizada = true;
            transicaoTempo = 0.0f;
            flashAtivo = true;
            flashTimer = 0.0f;
        }
    }
    // Lógica para Jade (deBone)
    else if (strcmp(gSelectedCharacterName, "Jade") == 0 && !deBoneTrocaRealizada)
    {
        tempoTrocaSprite += GetFrameTime();

        if (tempoTrocaSprite >= 13.0f)
        {
            deBoneTrocaRealizada = true;
            transicaoDeBoneTempo = 0.0f;
            flashAtivo = true;
            flashTimer = 0.0f;
        }
    }

    // Animação para Dante (carinha)
    if (spriteTrocaRealizada)
    {
        transicaoTempo += GetFrameTime() * 5.0f;

        if (transicaoTempo < 0.35f)
        {
            scaleCarinha = 0.4f + transicaoTempo * 0.57f;
            offsetCarinha = transicaoTempo * -1200.0f;
        }
        else
        {
            scaleCarinha = 0.5f;
            
            static float movimentoLentoTimer = 0.0f;
            static bool movimentoLentoAtivo = false;
            
            if (!movimentoLentoAtivo)
            {
                movimentoLentoAtivo = true;
                movimentoLentoTimer = 0.0f;
            }
            
            if (movimentoLentoTimer < 5.0f)
            {
                movimentoLentoTimer += GetFrameTime();
                offsetCarinha = -200.0f + (movimentoLentoTimer * -20.0f);
            }
            else
            {
                offsetCarinha = -300.0f;
            }
        }
    }

    // Animação para Jade (deBone) - mesma lógica que para Dante
    if (deBoneTrocaRealizada)
    {
        transicaoDeBoneTempo += GetFrameTime() * 5.0f;

        if (transicaoDeBoneTempo < 0.35f)
        {
            scaleDeBone = 1.0f + transicaoDeBoneTempo * 0.5f; // Cresce de 1.0 até ~1.35
            offsetDeBone = transicaoDeBoneTempo * -1200.0f;    // Move rápido para esquerda
        }
        else
        {
            scaleDeBone = 1.35f;
            
            static float movimentoLentoTimer = 0.0f;
            static bool movimentoLentoAtivo = false;
            
            if (!movimentoLentoAtivo)
            {
                movimentoLentoAtivo = true;
                movimentoLentoTimer = 0.0f;
            }
            
            if (movimentoLentoTimer < 5.0f)
            {
                movimentoLentoTimer += GetFrameTime();
                offsetDeBone = -200.0f + (movimentoLentoTimer * -20.0f);
            }
            else
            {
                offsetDeBone = -300.0f;
            }
        }
    }

    // Efeito de flash
    if (flashAtivo)
    {
        flashTimer += GetFrameTime();
        if (flashTimer < flashDuration)
        {
            if (flashTimer < flashDuration/2)
                flashAlpha = flashTimer * 2.0f / flashDuration;
            else
                flashAlpha = 1.0f - (flashTimer - flashDuration/2) * 2.0f / flashDuration;
        }
        else
        {
            flashAtivo = false;
            flashAlpha = 0.0f;
        }
    }
}

void Draw_Tela_02(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    int baseY = GetScreenHeight();

    float scaleMenina = 0.6f;
    float scaleCalvo = 0.8f;
    float currentScaleDeBone = deBoneTrocaRealizada ? scaleDeBone : 1.0f;

    // Posições Y (altura)
    int yCarinha = baseY - carinhaRevelationSilhouette.height * 0.4f; // Sempre usa a silhueta com escala 0.4
    int yMenina = baseY - meninaRevelationSilhouette.height * scaleMenina;
    int yCalvo = baseY - calvoRevelationSilhouette.height * scaleCalvo;
    int yDeBone = baseY - (deBoneTrocaRealizada ? deBoneRevelation.height : deBoneRevelationSilhouette.height) * currentScaleDeBone;

    // Posições X (horizontal)
    int xMenina = 700 + offsetXIncrement;
    int xCalvo = xMenina + offsetXIncrement;
    int xDeBone = xCalvo + offsetXIncrement + (deBoneTrocaRealizada ? offsetDeBone : 0);
    int xCarinha = 700; // Posição fixa para a silhueta

    // DESENHAR OS SPRITES NA ORDEM CORRETA:

    // 1. Primeiro o carinha (sempre a silhueta)
    Rectangle destRectCarinha = {xCarinha, yCarinha, 
                               carinhaRevelationSilhouette.width * 0.4f, 
                               carinhaRevelationSilhouette.height * 0.4f};
    DrawTexturePro(carinhaRevelationSilhouette, 
                  (Rectangle){0, 0, carinhaRevelationSilhouette.width, carinhaRevelationSilhouette.height}, 
                  destRectCarinha, (Vector2){0, 0}, 0.0f, WHITE);

    // 2. Os outros sprites (menina e calvo)
    Rectangle destRectMenina = {xMenina, yMenina, meninaRevelationSilhouette.width * scaleMenina, meninaRevelationSilhouette.height * scaleMenina};
    Rectangle destRectCalvo = {xCalvo, yCalvo, calvoRevelationSilhouette.width * scaleCalvo, calvoRevelationSilhouette.height * scaleCalvo};
    
    DrawTexturePro(meninaRevelationSilhouette, (Rectangle){0, 0, meninaRevelationSilhouette.width, meninaRevelationSilhouette.height}, destRectMenina, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(calvoRevelationSilhouette, (Rectangle){0, 0, calvoRevelationSilhouette.width, calvoRevelationSilhouette.height}, destRectCalvo, (Vector2){0, 0}, 0.0f, WHITE);

    // 3. deBone (pode ser a versão normal ou revelada)
    Texture2D currentDeBoneTexture = deBoneTrocaRealizada ? deBoneRevelation : deBoneRevelationSilhouette;
    Rectangle destRectDeBone = {xDeBone, yDeBone, 
                               currentDeBoneTexture.width * currentScaleDeBone, 
                               currentDeBoneTexture.height * currentScaleDeBone};
    DrawTexturePro(currentDeBoneTexture, 
                 (Rectangle){0, 0, currentDeBoneTexture.width, currentDeBoneTexture.height}, 
                 destRectDeBone, (Vector2){0, 0}, 0.0f, WHITE);

    // 4. Se for Dante, desenhar o carinha revelado por cima
    if (strcmp(gSelectedCharacterName, "Dante") == 0 && spriteTrocaRealizada)
    {
        Rectangle destRectCarinhaRevelation = {700 + offsetCarinha, 
                                             baseY - carinhaRevelation.height * scaleCarinha,
                                             carinhaRevelation.width * scaleCarinha,
                                             carinhaRevelation.height * scaleCarinha};
        DrawTexturePro(carinhaRevelation, 
                      (Rectangle){0, 0, carinhaRevelation.width, carinhaRevelation.height}, 
                      destRectCarinhaRevelation, (Vector2){0, 0}, 0.0f, WHITE);
    }

    // Efeito de flash (por cima de tudo)
    if (flashAlpha > 0.0f)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), 
                     (Color){255, 255, 255, (unsigned char)(flashAlpha * 255)});
    }

    EndDrawing();
}

bool Fase_Tela02_Concluida(void)
{
    return fase_concluida;
}

void Unload_Tela_02(void)
{
    UnloadTexture(carinhaRevelationSilhouette);
    UnloadTexture(carinhaRevelation);
    UnloadTexture(meninaRevelationSilhouette);
    UnloadTexture(calvoRevelationSilhouette);
    UnloadTexture(deBoneRevelationSilhouette);
    UnloadTexture(deBoneRevelation);

    StopMusicStream(writtenInTheStars);
    UnloadMusicStream(writtenInTheStars);
}