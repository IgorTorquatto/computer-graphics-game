#include "player.h"
#include <GL/glut.h>

#define LANE_X(i) ((i) * 2.5f)
#define GRAVITY -30.0f

/* Inicializa o jogador com valores padrão */
void initPlayer(Player *p) {
    p->lane = 1;
    p->x = LANE_X(1);
    p->y = 0.0f;
    p->z = 0.0f;
    p->vy = 0.0f;
    p->state = P_RUNNING;
    p->width = 1.0f;
    p->height = 2.0f;
    p->depth = 1.0f;
}

/* Atualiza física do jogador (pulo, slide, movimentação lateral) */
void updatePlayer(Player *p, float dt) {
    p->x = LANE_X(p->lane);

    if (p->state == P_JUMPING) {
        p->vy += GRAVITY * dt;
        p->y += p->vy * dt;
        if (p->y <= 0.0f) {
            p->y = 0.0f;
            p->vy = 0.0f;
            p->state = P_RUNNING;
        }
    }
    // Slide (recuperação automática feita por timer externo)
}

/* Função auxiliar para reset do slide do jogador */
static void endSlide(void *data) {
    Player *p = (Player*)data;
    p->height = 2.0f;
    p->state = P_RUNNING;
}

/* Entrada por teclado para movimentação do jogador */
void handlePlayerInput(Player *p, unsigned char key) {
    switch (key) {
        case 'w': case 'W': case ' ':
            if (p->state == P_RUNNING) {
                p->state = P_JUMPING;
                p->vy = 12.0f;
            }
            break;
        case 's': case 'S':
            if (p->state == P_RUNNING) {
                p->state = P_SLIDING;
                p->height = 1.0f;
                // Timer para retornar ao estado normal após 600ms
                // Necessário passar ponteiro do player via cast para int
                glutTimerFunc(600, (void (*)(int))endSlide, (int)(intptr_t)p);
            }
            break;
        case 'a': case 'A':
            if (p->lane > 0) {
                p->lane--;
            }
            break;
        case 'd': case 'D':
            if (p->lane < 2) {
                p->lane++;
            }
            break;
    }
}

/* Entrada por teclas especiais (setas) */
void handlePlayerSpecial(Player *p, int key) {
    switch (key) {
        case GLUT_KEY_LEFT:
            if (p->lane > 0) p->lane--;
            break;
        case GLUT_KEY_RIGHT:
            if (p->lane < 2) p->lane++;
            break;
        case GLUT_KEY_UP:
            if (p->state == P_RUNNING) {
                p->state = P_JUMPING;
                p->vy = 12.0f;
            }
            break;
        case GLUT_KEY_DOWN:
            if (p->state == P_RUNNING) {
                p->state = P_SLIDING;
                p->height = 1.0f;
                glutTimerFunc(600, (void (*)(int))endSlide, (int)(intptr_t)p);
            }
            break;
    }
}

/* Desenha o jogador como um cubo colorido */
void drawPlayer(const Player *p) {
    glColor3f(0.1f, 0.4f, 0.9f);
    glPushMatrix();
    glTranslatef(p->x, p->y + p->height * 0.5f, p->z);
    glScalef(p->width, p->height, p->depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}
