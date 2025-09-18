#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "estado.h"
#include "menu.h"

#define MAX_OBS 64
#define LANE_X(i) ( (1-i) * 2.5f )
#define GRAVITY -30.0f

typedef enum { P_RUNNING, P_JUMPING, P_SLIDING } PlayerState;

typedef struct {
    int lane; // 0..2
    float x,y,z;
    float vy;
    PlayerState state;
    float width, height, depth;
} Player;

typedef struct {
    int active;
    int lane;
    float x,y,z;
    float w,h,d;
} Obstacle;

Player player;
Obstacle obsPool[MAX_OBS];

float worldSpeed = 12.0f; // units per second, increases with time
float spawnTimer = 0.0f;
float spawnInterval = 1.0f;

extern void resetGame(); // declara função para menu chamar

void endSlide(int value) {
    player.height = 2.0f;
    player.state = P_RUNNING;
}

void resetGame() {
    player.lane = 1; player.x = LANE_X(1); player.y = 0; player.z = 0;
    player.vy = 0; player.state = P_RUNNING;
    player.width = 1.0f; player.height = 2.0f; player.depth = 1.0f;
    for(int i=0;i<MAX_OBS;i++) obsPool[i].active = 0;
    worldSpeed = 12.0f;
    spawnTimer = 0.0f;
}

void spawnObstacle() {
    for(int i=0;i<MAX_OBS;i++){
        if(!obsPool[i].active){
            obsPool[i].active = 1;
            obsPool[i].lane = rand()%3;
            obsPool[i].x = LANE_X(obsPool[i].lane);
            obsPool[i].y = 0;
            obsPool[i].z = 40.0f + (rand()%20);
            obsPool[i].w = 1.0f; obsPool[i].h = 2.2f; obsPool[i].d = 1.0f;
            return;
        }
    }
}

int aabbCollision(float ax, float ay, float az, float aw, float ah, float ad,
                  float bx, float by, float bz, float bw, float bh, float bd) {
    if (fabs(ax - bx) * 2.0f < (aw + bw) &&
        fabs(ay - by) * 2.0f < (ah + bh) &&
        fabs(az - bz) * 2.0f < (ad + bd)) return 1;
    return 0;
}

void update(float dt) {
    if(modoAtual != MODO_JOGO) return;

    worldSpeed += dt * 0.5f;

    player.x = LANE_X(player.lane);

    if(player.state == P_JUMPING) {
        player.vy += GRAVITY * dt;
        player.y += player.vy * dt;
        if(player.y <= 0.0f) { player.y = 0.0f; player.vy = 0.0f; player.state = P_RUNNING; }
    }

    for(int i=0;i<MAX_OBS;i++){
        if(!obsPool[i].active) continue;
        obsPool[i].z -= worldSpeed * dt;
        if(obsPool[i].z < -10.0f) obsPool[i].active = 0;
        else {
            float ph = player.height, pw = player.width, pd = player.depth;
            float oby = obsPool[i].y + obsPool[i].h*0.5f;
            if (aabbCollision(player.x, player.y + ph*0.5f, player.z, pw, ph, pd,
                              obsPool[i].x, oby, obsPool[i].z, obsPool[i].w, obsPool[i].h, obsPool[i].d)) {
                modoAtual = MODO_GAMEOVER;
            }
        }
    }

    spawnTimer += dt;
    if(spawnTimer >= spawnInterval) {
        spawnTimer = 0;
        spawnObstacle();
        spawnInterval = 0.8f - fmin(0.5f, worldSpeed * 0.01f);
    }
}

void drawCube(float x, float y, float z, float sx, float sy, float sz) {
    glPushMatrix();
    glTranslatef(x,y+sy*0.5f,z);
    glScalef(sx,sy,sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void renderScene() {
    if(modoAtual == MODO_MENU) {
        desenhaMenu();
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float camX = player.x;
    float camY = 3.0f;
    float camZpos = player.z - 6.0f;
    gluLookAt(camX, camY, camZpos, player.x, 1.0f, player.z+6.0f, 0,1,0);

    // Ground
    for(int i=0;i<60;i++){
        glPushMatrix();
        glTranslatef(0, -0.01f, i*5.0f - fmodf((float)time(NULL),5.0f)*0 );
        glBegin(GL_QUADS);
        glColor3f(0.2f,0.6f,0.2f);
        glVertex3f(-10, 0, i*5 - 50);
        glVertex3f(10, 0, i*5 - 50);
        glVertex3f(10, 0, i*5 - 49.5);
        glVertex3f(-10, 0, i*5 - 49.5);
        glEnd();
        glPopMatrix();
    }

    // Player
    glColor3f(0.1f,0.4f,0.9f);
    drawCube(player.x, player.y, player.z, player.width, player.height, player.depth);

    // Obstacles
    glColor3f(0.9f,0.2f,0.2f);
    for(int i=0;i<MAX_OBS;i++){
        if(!obsPool[i].active) continue;
        drawCube(obsPool[i].x, obsPool[i].y, obsPool[i].z, obsPool[i].w, obsPool[i].h, obsPool[i].d);
    }

    // UI - GameOver text
    if(modoAtual == MODO_GAMEOVER){
        int w = glutGet(GLUT_WINDOW_WIDTH), h = glutGet(GLUT_WINDOW_HEIGHT);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix(); glLoadIdentity();
        gluOrtho2D(0,w,0,h);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix(); glLoadIdentity();

        glColor3f(1,1,1);
        glRasterPos2i(w/2 - 80, h/2);
        const char *s = "GAME OVER - R to restart";
        for(const char *c=s; *c; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    glutSwapBuffers();
}

void idleCB() {
    static float last = 0;
    float now = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    if(last == 0) last = now;
    float dt = now - last;
    last = now;
    update(dt);
    glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y) {
    if(modoAtual == MODO_MENU) cliqueMenu(button, state, x, y);
}

void keyboardCB(unsigned char key, int x, int y) {
    if(key == 27) exit(0); // ESC
    if(modoAtual == MODO_GAMEOVER && (key=='r'||key=='R')) {
        modoAtual = MODO_MENU;
        return;
    }
    if(modoAtual != MODO_JOGO) return;

    if(key == 'w' || key == ' ') {
        if(player.state == P_RUNNING) {
            player.state = P_JUMPING;
            player.vy = 12.0f;
        }
    } else if(key == 's') {
        if(player.state == P_RUNNING) {
            player.state = P_SLIDING;
            player.height = 1.0f;
            glutTimerFunc(600, endSlide, 0);
        }
    }
}

void specialCB(int key, int x, int y) {
    if(modoAtual != MODO_JOGO) return;
    if(key == GLUT_KEY_LEFT && player.lane > 0) player.lane--;
    else if(key == GLUT_KEY_RIGHT && player.lane < 2) player.lane++;
}

void reshape(int w, int h) {
    if(h==0) h=1;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w/(float)h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1024, 600);
    glutCreateWindow("Simple 3D Runner - Prototype");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    resetGame(); // inicia jogo

    glutDisplayFunc(renderScene);
    glutIdleFunc(idleCB);
    glutKeyboardFunc(keyboardCB);
    glutSpecialFunc(specialCB);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseCB);

    glutMainLoop();
    return 0;
}
