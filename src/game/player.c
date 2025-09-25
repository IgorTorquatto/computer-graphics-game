#include "player.h"
#include "ecs/component.h"

#include <GL/glut.h>

#include <stdint.h>
#define LANE_X(i) ((i) * 2.5f)
#define GRAVITY -30.0f

float const move_speed = 10.0f;

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

void update_player(Player *p, float dt) {
    p->x = lerp(p->x, LANE_X(p->lane), ease_out_quad(dt * move_speed));

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

static void endSlide(void *data) {
    Player *p = (Player*)data;
    p->height = 2.0f;
    p->state = P_RUNNING;
}

void handlePlayerInput(Player *p, unsigned char key) {
    switch (key) {
        case 'w': case 'W': case ' ':
            if (p->state == P_RUNNING) {
                p->state = P_JUMPING;
                p->vy = 16.0f;
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
        /*case GLUT_KEY_DOWN:
            if (p->state == P_RUNNING) {
                p->state = P_SLIDING;
                p->height = 1.0f;
                p->slideTimeRemaining = 0.6f;
            }
            break;*/
    }
}

void drawPlayer(const Player *p) {
    glColor3f(0.1f, 0.4f, 0.9f);
    glPushMatrix();
    glTranslatef(p->x, p->y + p->height * 0.5f, p->z);
    glScalef(p->width, p->height, p->depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}
