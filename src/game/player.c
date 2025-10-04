#include "player.h"

#include "ecs/components/model.h"
#include "ecs/components/coin.h"
#include "ecs/systems/music_bus.h"

#include <GL/glut.h>

#include <stdint.h>
#define LANE_X(i) ((i) * 2.5f)
#define GRAVITY -30.0f

#define WALK_SFX_CHANNEL 1
#define WALK_SOUND_TIME 4.7f // sec
static float last_step_timer = 0.0f;

static float const move_speed = 10.0f;

extern float world_speed;

static void play_steps()
{
    if (world_speed > 20.0f)
        audio_bus_play_channel(SFX_RUN, WALK_SFX_CHANNEL);
    else
        audio_bus_play_channel(SFX_WALK, WALK_SFX_CHANNEL);
    last_step_timer = 0.0f;
}

static void set_state_run(Player* p) {
    p->state = P_RUNNING;
    play_steps();
}

void initPlayer(Player *p) {
    p->lane = 1;
    p->x = LANE_X(1);
    p->y = 0.0f;
    p->z = 0.0f;
    p->vy = 0.0f;
    p->width = 1.0f;
    p->height = 2.0f;
    p->depth = 1.0f;
    set_state_run(p);
}

static void set_state_jump(Player* p) {
    p->state = P_JUMPING;
    audio_bus_stop_channel(WALK_SFX_CHANNEL); // stop steps
    audio_bus_play_sfx(rand() % 2 ? SFX_JUMP_1 : SFX_JUMP_2);
}

void update_player(Player *p, float dt) {
    p->x = lerp(p->x, LANE_X(p->lane), ease_out_quad(dt * move_speed));

    switch (p->state) {

        case P_JUMPING: {
            p->vy += GRAVITY * dt;
            p->y += p->vy * dt;
            if (p->y <= 0.0f) {
                p->y = 0.0f;
                p->vy = 0.0f;
                set_state_run(p);
            }
        } break;

        case P_RUNNING: {
            last_step_timer += dt;
            if (last_step_timer >= WALK_SOUND_TIME)
                play_steps();
        } break;

        default:
            break;
    }

    // Slide (recuperação automática feita por timer externo)
}

static void endSlide(void *data) {
    Player *p = (Player*)data;
    p->height = 2.0f;
    set_state_run(p);
}

static void move_left(Player* p) {
    if (p->lane > 0) {
        p->lane--;
        audio_bus_play_sfx(SFX_DASH);
    }
}

static void move_right(Player* p) {
    if (p->lane < 2) {
        p->lane++;
        audio_bus_play_sfx(SFX_DASH);
    }
}

static void jump(Player* p) {
    if (p->state == P_RUNNING) {
        p->vy = 12.0f;
        set_state_jump(p);
    }
}

void handlePlayerInput(Player *p, unsigned char key) {
    switch (key) {
        case 'a': case 'A':
            move_left(p);
            break;
        case 'd': case 'D':
            move_right(p);
            break;
        case 'w': case 'W': case ' ':
            jump(p);
            break;
        /*case 's': case 'S':
            if (p->state == P_RUNNING) {
                // p->state = P_SLIDING;
                set_state_slide(p);
                p->height = 1.0f;
                p->slideTimeRemaining = 0.6f;
            }
            break;*/
        default:
            break;
    }
}

void handlePlayerSpecial(Player *p, int key) {
    switch (key) {
        case GLUT_KEY_LEFT:
            move_left(p);
            break;
        case GLUT_KEY_RIGHT:
            move_right(p);
            break;
        case GLUT_KEY_UP:
            jump(p);
            break;
        /*case GLUT_KEY_DOWN:
            if (p->state == P_RUNNING) {
                //p->state = P_SLIDING;
                set_state_slide(p);
                p->height = 1.0f;
                p->slideTimeRemaining = 0.6f;
            }
            break;*/
    }
}

void drawPlayer(const Player *p) {
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.1f, 0.4f, 0.9f);
    glPushMatrix();
    glTranslatef(p->x, p->y + p->height * 0.5f, p->z);
    glScalef(p->width, p->height, p->depth);
    glutSolidCube(1.0f);
    glPopMatrix();
    glDisable(GL_COLOR_MATERIAL);
}
