
#pragma once

typedef enum { P_RUNNING, P_JUMPING, P_SLIDING } PlayerState;

typedef struct {
    int lane;
    float x, y, z;
    float vy;
    PlayerState state;
    float width, height, depth;
    float slideTimeRemaining;
    float animationTime;
    int side;
    float sideTimer;
} Player;

/* Inicializa o jogador com valores padrão */
void initPlayer(Player *p);

/* Atualiza física do jogador (pulo, slide, movimentação lateral) */
void update_player(Player *p, float dt);

/* Função auxiliar para reset do slide do jogador */
static void endSlide(void *data);

/* Entrada por teclado para movimentação do jogador */
void handlePlayerInput(Player *p, unsigned char key);

/* Entrada por teclas especiais (setas) */
void handlePlayerSpecial(Player *p, int key);

/* Desenha o jogador como um cubo colorido */
void drawPlayer(const Player *p);


