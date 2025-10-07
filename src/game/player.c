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

#pragma region Globals
    extern float world_speed;

    extern GLfloat shadow_light_direction[];
#pragma region 

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
   // p->x = LANE_X(1);
    p->x = LANE_X(p->lane);
    p->y = 0.0f;
    p->z = 0.0f;
    p->vy = 0.0f;
    p->width = 1.0f;
    p->height = 2.0f;
    p->depth = 1.0f;
    p->animationTime = 0.0f;
    p->side = 0;
    p->sideTimer = 0.0f;
    set_state_run(p);
}

static void set_state_jump(Player* p) {
    p->state = P_JUMPING;
    audio_bus_stop_channel(WALK_SFX_CHANNEL); // stop steps
    audio_bus_play_sfx(rand() % 2 ? SFX_JUMP_1 : SFX_JUMP_2);
}

void update_player(Player *p, float dt) {
    p->x = lerp(p->x, LANE_X(p->lane), ease_out_quad(dt * move_speed));

    if (p->side != 0) {
        p->sideTimer += dt;
        if (p->sideTimer >= 0.4f) { 
            p->side = 0;
            p->sideTimer = 0.0f;
        }
    }


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
            p->animationTime += 0.5f;
            last_step_timer += dt;
            if (last_step_timer >= WALK_SOUND_TIME)
                play_steps();
        } break;

        default:
            p->animationTime = 0.0f;
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
    p->sideTimer = 0.0f;
    p->side = 1;
    if (p->lane > 0) {
        p->lane--;
        audio_bus_play_sfx(SFX_DASH);
    }
}

static void move_right(Player* p) {
    p->sideTimer = 0.0f;
    p->side = 2;
    if (p->lane < 2) {
        p->lane++;
        audio_bus_play_sfx(SFX_DASH);
    }
}

static void jump(Player* p) {
    //p->sideTimer = 0.0f;
    p->side = 0;
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

void _drawPlayer(const Player *p) {

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glColor3f(0.1f, 0.4f, 0.9f);
    glPushMatrix();
    glTranslatef(p->x, p->y + p->height * 0.5f, p->z);
    glScalef(p->width, p->height, p->depth);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.1f,0.1f,0.1f);
    glPushMatrix();
    glTranslatef(p->x, p->y + p->height + 0.2f, p->z);
    glScalef(p->width,0.1f,p->depth);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(p->x, p->y + p->height + 0.3f, p->z);
    glScalef(p->width - 0.3f,0.3f,p->depth - 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();


    glColor3f(0.1f,0.4f,0.9f);

    float armAngle = 0.0f;

    if(p->side == 1){
        armAngle = 30.0f;
        // Braço esquerdo
        glPushMatrix();
        glTranslatef(p->x - p->width * 0.5f, p->y + p->height * 0.5f, p->z);
        glRotatef(armAngle,0.0f,0.0f,1.0f);
        glTranslatef(-p->width * 0.2f + 0.4f, 0.0f, 0.0f);
        glScalef(p->width * 0.4f, p->height * 0.6f, p->depth * 0.4f);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Braço direito
        glPushMatrix();
        glTranslatef(p->x + p->width * 0.5f, p->y + p->height * 0.5f, p->z);
        glRotatef(armAngle,0.0f,0.0f,1.0f);
        glTranslatef(p->width * 0.2f + 0.1f, 0.0f, 0.0f);
        glScalef(p->width * 0.4f, p->height * 0.6f, p->depth * 0.4f);
        glutSolidCube(1.0f);
        glPopMatrix();

    }else if (p->side == 2){
        armAngle = -30.0f;
        // Braço esquerdo
        glPushMatrix();
        glTranslatef(p->x - p->width * 0.5f, p->y + p->height * 0.5f, p->z);
        glRotatef(armAngle,0.0f,0.0f,1.0f);
        glTranslatef(-p->width * 0.2f -0.1f, 0.0f, 0.0f);
        glScalef(p->width * 0.4f, p->height * 0.6f, p->depth * 0.4f);
        glutSolidCube(1.0f);
        glPopMatrix();
        
        // Braço direito
        glPushMatrix();
        glTranslatef(p->x + p->width * 0.5f, p->y + p->height * 0.5f, p->z);
        glRotatef(armAngle,0.0f,0.0f,1.0f);
        glTranslatef(p->width * 0.2f - 0.5f, 0.0f, 0.0f);
        glScalef(p->width * 0.4f, p->height * 0.6f, p->depth * 0.4f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }else{
        if(p->state == P_RUNNING){
            armAngle = sinf(p->animationTime) * 30.0f;
            // Braço esquerdo
            glPushMatrix();
            glTranslatef(p->x - p->width * 0.5f, p->y + p->height * 0.5f, p->z);
            glRotatef(armAngle,1.0f,0.0f,0.0f);
            glTranslatef(-p->width * 0.2f, 0.0f, 0.0f);
            glScalef(p->width * 0.4f, p->height * 0.6f, p->depth * 0.4f);
            glutSolidCube(1.0f);
            glPopMatrix();
            
            // Braço direito
            glPushMatrix();
            glTranslatef(p->x + p->width * 0.5f, p->y + p->height * 0.5f, p->z);
            glRotatef(-armAngle,1.0f,0.0f,0.0f);
            glTranslatef(p->width * 0.2f, 0.0f, 0.0f);
            glScalef(p->width * 0.4f, p->height * 0.6f, p->depth * 0.4f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }else if(p->state == P_JUMPING){
            armAngle = 120;
            glPushMatrix();
            glTranslatef(p->x - p->width * 0.9, p->y + p->height * 0.65f, p->z);
            glRotatef(-armAngle,0.0f,0.0f,1.0f);
            glTranslatef(-p->width * 0.2f, 0.0f, 0.0f);
            glScalef(p->width * 0.4f, p->height * 0.6f, p->depth * 0.4f);
            glutSolidCube(1.0f);
            glPopMatrix();
            
            // Braço direito
            glPushMatrix();
            glTranslatef(p->x + p->width * 0.9f, p->y + p->height * 0.65f, p->z);
            glRotatef(armAngle,0.0f,0.0f,1.0f);
            glTranslatef(p->width * 0.2f, 0.0f, 0.0f);
            glScalef(p->width * 0.4f, p->height * 0.6f, p->depth * 0.4f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }
    glDisable(GL_CULL_FACE);
    glDisable(GL_COLOR_MATERIAL);
}

void drawSimplePlayerShadow(const Player *p) {
    glDisable(GL_LIGHTING);
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    // Position shadow slightly above floor to avoid z-fighting
    glTranslatef(p->x, 0.02f, p->z);
    
    // Scale based on player height (shadow gets larger when player is higher)
    float shadowScale = 0.8f + (p->y * 0.1f);
    glScalef(shadowScale, 1.0f, shadowScale);
    
    // Draw circular shadow
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= 360; i += 20) {
        float angle = i * 3.14159f / 180.0f;
        glVertex3f(cos(angle) * p->width, 0.0f, sin(angle) * p->depth);
    }
    glEnd();
    
    glPopMatrix();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// void drawPlayer(const Player *p)
// {
//     // Draw your model normally
//     _drawPlayer(p);
// 
//     // Draw simple circular shadow
// }

void drawCuboidProjectionShadow(const Player *p) {
    glDisable(GL_LIGHTING);
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Light direction (normalized)
    float light_dir[3] = {shadow_light_direction[0], 
                         shadow_light_direction[1], 
                         shadow_light_direction[2]};
    
    // Normalize light direction if needed
    float length = sqrt(light_dir[0]*light_dir[0] + 
                       light_dir[1]*light_dir[1] + 
                       light_dir[2]*light_dir[2]);
    if (length > 0) {
        light_dir[0] /= length;
        light_dir[1] /= length;
        light_dir[2] /= length;
    }

    // Player dimensions
    float half_width = p->width * 0.5f;
    float half_depth = p->depth * 0.5f;
    float height = p->height;

    // Calculate the 8 vertices of the player's bounding box
    float vertices[8][3] = {
        // Bottom face
        {p->x - half_width, p->y, p->z - half_depth},
        {p->x + half_width, p->y, p->z - half_depth},
        {p->x + half_width, p->y, p->z + half_depth},
        {p->x - half_width, p->y, p->z + half_depth},
        // Top face
        {p->x - half_width, p->y + height, p->z - half_depth},
        {p->x + half_width, p->y + height, p->z - half_depth},
        {p->x + half_width, p->y + height, p->z + half_depth},
        {p->x - half_width, p->y + height, p->z + half_depth}
    };

    // Project each vertex onto the floor (y=0) along light direction
    float shadow_vertices[8][3];
    for (int i = 0; i < 8; i++) {
        // If light direction is vertical, handle separately
        if (fabs(light_dir[1]) < 0.001f) {
            // Horizontal light - project to infinity or use simple shadow
            shadow_vertices[i][0] = vertices[i][0];
            shadow_vertices[i][1] = 0.02f;
            shadow_vertices[i][2] = vertices[i][2];
        } else {
            // Calculate projection onto floor plane (y=0)
            float t = -vertices[i][1] / light_dir[1];
            shadow_vertices[i][0] = vertices[i][0] + light_dir[0] * t;
            shadow_vertices[i][1] = 0.02f; // Slightly above floor
            shadow_vertices[i][2] = vertices[i][2] + light_dir[2] * t;
        }
    }

    // Draw the shadow as a projected cuboid
    glBegin(GL_QUADS);
    
    // Bottom face (projected)
    glVertex3fv(shadow_vertices[0]);
    glVertex3fv(shadow_vertices[1]);
    glVertex3fv(shadow_vertices[2]);
    glVertex3fv(shadow_vertices[3]);
    
    // Top face (projected)
    glVertex3fv(shadow_vertices[4]);
    glVertex3fv(shadow_vertices[5]);
    glVertex3fv(shadow_vertices[6]);
    glVertex3fv(shadow_vertices[7]);
    
    // Side faces
    glVertex3fv(shadow_vertices[0]); glVertex3fv(shadow_vertices[1]);
    glVertex3fv(shadow_vertices[5]); glVertex3fv(shadow_vertices[4]);
    
    glVertex3fv(shadow_vertices[1]); glVertex3fv(shadow_vertices[2]);
    glVertex3fv(shadow_vertices[6]); glVertex3fv(shadow_vertices[5]);
    
    glVertex3fv(shadow_vertices[2]); glVertex3fv(shadow_vertices[3]);
    glVertex3fv(shadow_vertices[7]); glVertex3fv(shadow_vertices[6]);
    
    glVertex3fv(shadow_vertices[3]); glVertex3fv(shadow_vertices[0]);
    glVertex3fv(shadow_vertices[4]); glVertex3fv(shadow_vertices[7]);
    
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void drawPlayer(const Player *p)
{
    // Draw your model normally
    _drawPlayer(p);

    // Draw proper cuboid projection shadow
    drawCuboidProjectionShadow(p);

    drawSimplePlayerShadow(p);
}
