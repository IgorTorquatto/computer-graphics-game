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

extern void resetGame(); // declara fun��o para menu chamar

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
            obsPool[i].z = 40.0f + (rand()%20); // spawn ahead
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
    if(gstate != STATE_PLAY) return;
    // speed up slowly
    worldSpeed += dt * 0.5f;

    // player lane smoothing (instant for now)
    player.x = LANE_X(player.lane);

    // jump physics
    if(player.state == P_JUMPING) {
        player.vy += GRAVITY * dt;
        player.y += player.vy * dt;
        if(player.y <= 0.0f) { player.y = 0.0f; player.vy = 0.0f; player.state = P_RUNNING; }
    }

    // obstacles move towards player (z decreases)
    for(int i=0;i<MAX_OBS;i++){
        if(!obsPool[i].active) continue;
        obsPool[i].z -= worldSpeed * dt;
        // if passed behind player, deactivate
        if(obsPool[i].z < -10.0f) obsPool[i].active = 0;
        else {
            // collision check
            float ph = player.height, pw = player.width, pd = player.depth;
            float pye = player.y + ph*0.5f;
            float oby = obsPool[i].y + obsPool[i].h*0.5f;
            if (aabbCollision(player.x, player.y + ph*0.5f, player.z, pw, ph, pd,
                              obsPool[i].x, oby, obsPool[i].z, obsPool[i].w, obsPool[i].h, obsPool[i].d)) {
                gstate = STATE_GAMEOVER;
            }
        }
    }

    // spawning
    spawnTimer += dt;
    if(spawnTimer >= spawnInterval) {
        spawnTimer = 0;
        spawnObstacle();
        // vary spawn interval with speed
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

    // ground (simple repeated quads)
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



/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wire-frame and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// === ECS ===
#include "ecs/entity.h"
#include "ecs/component.h"
#include "ecs/system.h"

// === Game Objects ===
#include "game_objects/test.h"


static int slices = 16;
static int stacks = 16;
static EntityId geometry_samples[6];
// TODO -> Cone component


void update_geometry_slices_and_stacks() {
    for (EntityId id = 0; id < (sizeof(geometry_samples) / sizeof(EntityId)); ++id) {
        if (id == INVALID_ENTITY)
            continue;
        GeometryId geometry_id = game_objects[id].geometry.id;
        switch (game_objects[id].geometry.type) {
            case SPHERE:
                spheres[geometry_id].slices = slices;
                spheres[geometry_id].stacks = stacks;
                break;

            case TORUS:
                toruses[geometry_id].slices = slices;
                toruses[geometry_id].stacks = stacks;
                break;

            default:
                break;
        }
    }
}


void calculate_frames_per_second() {
    static int frameCount = 0;
    static int previousTime = 0;
    frameCount++;
    int currentTime = glutGet(GLUT_ELAPSED_TIME); // milliseconds
    int timeInterval = currentTime - previousTime;

    if (timeInterval > 1000) { // update every second
        fps = frameCount / (timeInterval / 1000.0f);
        previousTime = currentTime;
        frameCount = 0;
    }
}


int init()
{
    const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

    const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
    const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
    const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat high_shininess[] = { 100.0f };

    glClearColor(1,1,1,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    // Initialize game objects to "empty"

    memset(geometry_samples, INVALID_ENTITY, sizeof(geometry_samples) / sizeof(EntityId));

    const double speed = 1.5f;

    EntityId sphere_object = create_game_object();
    GeometryId sphere = add_sphere(sphere_object, 1.0f, (Position){-2.4f, 1.2f, -6.0f});
    rotate_x(sphere_object, deg_to_rad(60));
    set_color(sphere_object, color_red);
    set_rotation_velocity(sphere_object, (Velocity){0.0f, 0.0f, speed});
    spheres[sphere].slices = slices;
    spheres[sphere].stacks = stacks;
    geometry_samples[0] = sphere_object;

    EntityId torus_object = create_game_object();
    GeometryId torus = add_torus(torus_object, 0.2f, 0.8f, (Position){2.4f, 1.2f, -6.0f});
    rotate_x(torus_object, deg_to_rad(60));
    set_color(torus_object, color_red);
    set_rotation_velocity(torus_object, (Velocity){0.0f, 0.0f, speed});
    toruses[torus].slices = slices;
    toruses[torus].stacks = stacks;
    geometry_samples[1] = torus_object;

    create_test_object();

    return EXIT_SUCCESS;
}


/* GLUT callback Handlers *\/

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void display(void)
{
    const double elapsed_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double speed = 90.0f;
    const double a = elapsed_time * speed;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO -> uses ecs to render these samples
    glColor3d(1,0,0);

    // old sample code
    // glPushMatrix();
    //     glTranslated(-2.4,1.2,-6);
    //     glRotated(60,1,0,0);
    //     glRotated(a,0,0,1);
    //     glutSolidSphere(1,slices,stacks);
    // glPopMatrix();

    glPushMatrix();
        glTranslated(0,1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutSolidCone(1,1,slices,stacks);
    glPopMatrix();

    // glPushMatrix();
    //     glTranslated(2.4,1.2,-6);
    //     glRotated(60,1,0,0);
    //     glRotated(a,0,0,1);
    //     glutSolidTorus(0.2,0.8,slices,stacks);
    // glPopMatrix();

    glPushMatrix();
        glTranslated(-2.4,-1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutWireSphere(1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,-1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutWireCone(1,1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
        glTranslated(2.4,-1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutWireTorus(0.2,0.8,slices,stacks);
    glPopMatrix();

    render_system();

    glutSwapBuffers();

    calculate_frames_per_second();
}


static void idle(void)
{
    const double elapsed_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    static double last_time = 0.0f;
    DeltaTime delta = elapsed_time - last_time;
    last_time = elapsed_time;

    process_system(delta);
    glutPostRedisplay();
}


#define escape 27
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case escape:
        case 'q':
            exit(0);
            break;

        case '+': {
            slices++;
            stacks++;
            update_geometry_slices_and_stacks();
        } break;

        case '-':
            if (slices > 3 && stacks > 3) {
                slices--;
                stacks--;
                update_geometry_slices_and_stacks();
            }
            break;

        default:
            input_system((InputEvent){key, glutGetModifiers(), {(float)x, (float)y}, KEYBOARD});
            break;
    }

    glutPostRedisplay();
}


/* Program entry point *\/
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(10, 10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("ECS Test");

    glutReshapeFunc(resize);

    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    int err = init();
    if (err != EXIT_SUCCESS)
        return err;

    glutMainLoop();

    return EXIT_SUCCESS;
}

*/
