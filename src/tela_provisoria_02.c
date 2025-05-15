#include "tela_provisoria_02.h"
#include "raylib.h"
#include "generalFunctions.h"

// ESSA É A TELA DO RESULTADO DO PROCESSO SELETIVO

static bool fase_concluida = false;

// Definir as variáveis para armazenar as texturas dos sprites
static Texture2D carinhaRevelationSilhouette;
static Texture2D carinhaRevelation; // Nova textura
static Texture2D meninaRevelationSilhouette;
static Texture2D calvoRevelationSilhouette;
static Texture2D deBoneRevelationSilhouette;

// Definir a variável para carregar a música
static Music writtenInTheStars;

// Variável para controlar o movimento do offsetXIncrement
static float offsetXIncrement = 0.0f;    // Começa com 0 e vai aumentando ao longo do tempo
static float tempoMovimento = 0.0f;      // Tempo de animação para o movimento
static bool movimentoFinalizado = false; // Marca se o movimento foi finalizado

// Variáveis para controlar o tempo da troca
static float tempoTrocaSprite = 0.0f;
static bool spriteTrocaRealizada = false;

// Variáveis para controlar a animação de transição do sprite
static float transicaoTempo = 0.0f;
static float scaleCarinha = 0.4f;
static float offsetCarinha = 0.0f;

// Variáveis para o efeito de flash (duração aumentada para 0.6 segundos)
static float flashAlpha = 0.0f;
static bool flashAtivo = false;
static float flashDuration = 0.6f; // Aumentei de 0.3f para 0.6f (o dobro de duração)
static float flashTimer = 0.0f;

void Init_Tela_02(void)
{
    fase_concluida = false;

    carinhaRevelationSilhouette = LoadTexture("src/sprites/revelation/carinhaRevelationSilhouette.png");
    carinhaRevelation = LoadTexture("src/sprites/revelation/carinhaRevelation.png"); // Carregar o novo sprite
    meninaRevelationSilhouette = LoadTexture("src/sprites/revelation/meninaRevelationSilhouette.png");
    calvoRevelationSilhouette = LoadTexture("src/sprites/revelation/calvoRevelationSilhouette.png");
    deBoneRevelationSilhouette = LoadTexture("src/sprites/revelation/deBoneRevelationSilhouette.png");

    // Carregar a música
    writtenInTheStars = LoadMusicStream("src/music/writtenInTheStars.mp3");

    // Iniciar a reprodução da música
    PlayMusicStream(writtenInTheStars);
}

void Update_Tela_02(void)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        // Define que a tela acaba!
        fase_concluida = true;
    }

    // Atualizar o stream de áudio
    UpdateMusicStream(writtenInTheStars);

    // Se o movimento ainda não foi finalizado (duração de 5 segundos)
    if (!movimentoFinalizado)
    {
        tempoMovimento += GetFrameTime(); // Aumenta o tempo decorrido

        // Aumentar o offsetXIncrement para dar sensação de movimento
        offsetXIncrement += 0.05f; // Aumenta gradualmente para mover os sprites

        // Se passaram 14 segundos, parar o movimento
        if (tempoMovimento >= 13.0f)
        {
            movimentoFinalizado = true; // Marca o movimento como finalizado
        }
    }

    // Verificar se o personagem selecionado é "Dante" ou qualquer outro nome
    if (strcmp(gSelectedCharacterName, "Dante") == 0 && !spriteTrocaRealizada)
    {
        tempoTrocaSprite += GetFrameTime(); // Aumentar o tempo decorrido

        // Se passaram 14 segundos, fazer a troca do sprite
        if (tempoTrocaSprite >= 13.0f)
        {
            spriteTrocaRealizada = true; // Marcar que a troca foi realizada
            transicaoTempo = 0.0f;       // Resetar tempo de transição
            flashAtivo = true;           // Ativar o flash
            flashTimer = 0.0f;           // Resetar timer do flash
        }
    }

    // Controlar a animação da transição (aumento da escala e movimento)
    if (spriteTrocaRealizada)
    {
        transicaoTempo += GetFrameTime() * 5.0f; // Velocidade moderada (5x)

        if (transicaoTempo < 0.35f) // Duração da animação rápida
        {
            scaleCarinha = 0.4f + transicaoTempo * 0.57f; // Cresce de 0.4 até ~0.6
            offsetCarinha = transicaoTempo * -1200.0f;    // Move rápido para esquerda
        }
        else
        {
            scaleCarinha = 0.5f; // Escala final
            
            // Terminou a animação rápida, agora começa o movimento lento
            static float movimentoLentoTimer = 0.0f;
            static bool movimentoLentoAtivo = false;
            
            if (!movimentoLentoAtivo)
            {
                movimentoLentoAtivo = true;
                movimentoLentoTimer = 0.0f;
            }
            
            if (movimentoLentoTimer < 5.0f) // Movimento lento por 5 segundos
            {
                movimentoLentoTimer += GetFrameTime();
                offsetCarinha = -200.0f + (movimentoLentoTimer * -20.0f); // Move -20 pixels por segundo
            }
            else
            {
                offsetCarinha = -300.0f; // Posição final após os 5 segundos
            }
        }
    }

    // Controlar o efeito de flash
    if (flashAtivo)
    {
        flashTimer += GetFrameTime();
        if (flashTimer < flashDuration)
        {
            // Fade in rápido e fade out mais lento
            if (flashTimer < flashDuration/2)
                flashAlpha = flashTimer * 2.0f / flashDuration; // Aumenta de 0 a 1
            else
                flashAlpha = 1.0f - (flashTimer - flashDuration/2) * 2.0f / flashDuration; // Diminui de 1 a 0
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

    // Posição inicial à borda direita
    int baseY = GetScreenHeight(); // Fica encostando na borda inferior

    // Escalas fixas para os outros sprites
    float scaleMenina = 0.6f;
    float scaleCalvo = 0.8f;
    float scaleDeBone = 1.0f;

    // Calcular as posições ajustadas, levando em consideração a altura de cada sprite escalado
    int yCarinha = baseY - (spriteTrocaRealizada ? carinhaRevelation.height : carinhaRevelationSilhouette.height) * scaleCarinha;
    int yMenina = baseY - meninaRevelationSilhouette.height * scaleMenina;
    int yCalvo = baseY - calvoRevelationSilhouette.height * scaleCalvo;
    int yDeBone = baseY - deBoneRevelationSilhouette.height * scaleDeBone;

    // Posições no eixo X para os outros sprites (usando offsetXIncrement normalmente)
    int xMenina = 700 + offsetXIncrement;
    int xCalvo = xMenina + offsetXIncrement;
    int xDeBone = xCalvo + offsetXIncrement;

    // Posição do carinha (com offset adicional quando a troca ocorrer)
    int xCarinha = 700 + (spriteTrocaRealizada ? offsetCarinha : 0);

    // Desenhar o carinha primeiro (para ficar atrás dos outros)
    Texture2D currentCarinhaTexture = spriteTrocaRealizada ? carinhaRevelation : carinhaRevelationSilhouette;
    Rectangle destRectCarinha = {xCarinha, yCarinha, currentCarinhaTexture.width * scaleCarinha, currentCarinhaTexture.height * scaleCarinha};
    DrawTexturePro(currentCarinhaTexture, (Rectangle){0, 0, currentCarinhaTexture.width, currentCarinhaTexture.height}, destRectCarinha, (Vector2){0, 0}, 0.0f, WHITE);

    // Desenhar os outros sprites por cima
    Rectangle destRectMenina = {xMenina, yMenina, meninaRevelationSilhouette.width * scaleMenina, meninaRevelationSilhouette.height * scaleMenina};
    Rectangle destRectCalvo = {xCalvo, yCalvo, calvoRevelationSilhouette.width * scaleCalvo, calvoRevelationSilhouette.height * scaleCalvo};
    Rectangle destRectDeBone = {xDeBone, yDeBone, deBoneRevelationSilhouette.width * scaleDeBone, deBoneRevelationSilhouette.height * scaleDeBone};

    DrawTexturePro(meninaRevelationSilhouette, (Rectangle){0, 0, meninaRevelationSilhouette.width, meninaRevelationSilhouette.height}, destRectMenina, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(calvoRevelationSilhouette, (Rectangle){0, 0, calvoRevelationSilhouette.width, calvoRevelationSilhouette.height}, destRectCalvo, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(deBoneRevelationSilhouette, (Rectangle){0, 0, deBoneRevelationSilhouette.width, deBoneRevelationSilhouette.height}, destRectDeBone, (Vector2){0, 0}, 0.0f, WHITE);

    // Desenhar o efeito de flash por cima de tudo
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
    // Lógica de desalocar as texturas dos sprites
    UnloadTexture(carinhaRevelationSilhouette);
    UnloadTexture(carinhaRevelation); // Descarregar o novo sprite
    UnloadTexture(meninaRevelationSilhouette);
    UnloadTexture(calvoRevelationSilhouette);
    UnloadTexture(deBoneRevelationSilhouette);

    // Parar a música e descarregar
    StopMusicStream(writtenInTheStars);
    UnloadMusicStream(writtenInTheStars);
}