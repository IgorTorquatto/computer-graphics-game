#include <GL/glut.h>
#include <GL/glu.h>

#include <math.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>

#include "estado.h"
#include "menu.h"
#include "model.h"
#include "tree.h"
#include "player.h"
#include "coin.h"
#include "hud.h"
#include "obstacle.h"

Player player;

float worldSpeed = 12.0f;
float distanciaPercorrida = 0.0f;
static const float fator = 0.1f;

Model treeModel;
float escalaArvoreDefault = 1.0f;

extern Model rockModel;
extern Model logModel;

void mostrarEixos(){
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-10.0f, 0.1f, 0.0f);
    glVertex3f(10.0f, 0.1f, 0.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    for(int x = -10; x <= 10; x += 5) {
        glRasterPos3f(x, 0.2f, 0.0f);
        char label[10];
        snprintf(label, 10, "%d", x);
        for(const char *c = label; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 5.0f, 0.0f);
    glEnd();

    for(int y = 0; y <= 5; y += 1) {
        glRasterPos3f(0.2f, y, 0.0f);
        char label[10];
        snprintf(label, 10, "%d", y);
        for(const char *c = label; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.1f, -20.0f);
    glVertex3f(0.0f, 0.1f, 20.0f);
    glEnd();

    for(int z = -20; z <= 20; z += 5) {
        glRasterPos3f(0.2f, 0.2f, z);
        char label[10];
        snprintf(label, 10, "%d", z);
        for(const char *c = label; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// Colisão AABB (mantida como antes)
int aabbCollision(float ax, float ay, float az, float aw, float ah, float ad,
                  float bx, float by, float bz, float bw, float bh, float bd) {
    if (fabs(ax - bx) * 2.0f < (aw + bw) &&
        fabs(ay - by) * 2.0f < (ah + bh) &&
        fabs(az - bz) * 2.0f < (ad + bd)) return 1;
    return 0;
}

void resetGame() {
    initPlayer(&player);
    initObstacleModels();
    initObstacles();
    worldSpeed = 12.0f;
    distanciaPercorrida = 0.0f;
    initCoins();
}

void update(float dt) {
    if(modoAtual != MODO_JOGO) return;

    worldSpeed += dt * 0.5f;

    updatePlayer(&player, dt);

    obstacleUpdate(dt);

    Obstacle* obstacles = getObstacles();
    int maxObs = getMaxObstacles();

    for(int i = 0; i < maxObs; i++) {
        if(!obstacles[i].active) continue;

        float ph = player.height, pw = player.width, pd = player.depth;

        float obstCenterX = obstacles[i].x;
        float obstWidth = obstacles[i].w;
        float obstHeight = obstacles[i].h;
        float obstDepth = obstacles[i].d;
        float obstY = obstacles[i].y + obstHeight * 0.5f;

        if (aabbCollision(
            player.x, player.y + ph * 0.5f, player.z, pw, ph, pd,
            obstCenterX, obstY, obstacles[i].z, obstWidth, obstHeight, obstDepth)
        ) {
            modoAtual = MODO_GAMEOVER;
        }
    }

    distanciaPercorrida += worldSpeed * dt * fator;

    updateTrees(dt, worldSpeed);
    updateCoins(dt);
}

void renderScene() {
    if(modoAtual == MODO_MENU) {
        desenhaMenu();
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float camX = player.x;
    float camY = 4.0f;
    float camZ = player.z + 8.0f;
    gluLookAt(camX, camY, camZ,
              player.x, 1.0f, player.z - 8.0f,
              0.0f, 1.0f, 0.0f);

    GLfloat light_position[] = { player.x + 5.0f, 10.0f, player.z + 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    //glEnable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


    glDisable(GL_LIGHTING);
    glColor3f(0.9f, 0.9f, 0.9f);
    for(int i = -100; i < 100; i++) {
        float zpos = i * 5.0f;
        glBegin(GL_QUADS);
            glVertex3f(-1.5f, 0.0f, zpos);
            glVertex3f(6.5f, 0.0f, zpos);
            glVertex3f(6.5f, 0.0f, zpos - 5.0f);
            glVertex3f(-1.5f, 0.0f, zpos - 5.0f);
        glEnd();
    }
    glEnable(GL_LIGHTING);

    drawPlayer(&player);
    drawObstacles();
    drawCoins3D();
    drawTrees();

    if(modoAtual == MODO_GAMEOVER) {
        int w = glutGet(GLUT_WINDOW_WIDTH), h = glutGet(GLUT_WINDOW_HEIGHT);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, w, 0, h);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2i(w / 2 - 80, h / 2);
        const char *s = "Game over! R para reiniciar";
        for(const char *c = s; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        glEnable(GL_LIGHTING);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    drawDistance(distanciaPercorrida);
    drawCoinsHUD(getCoinCount());

    glutSwapBuffers();
}

void idleCB() {
    static float last = 0.0f;
    float now = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    if(last == 0.0f) last = now;
    float dt = now - last;
    last = now;
    update(dt);
    glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y) {
    if(modoAtual == MODO_MENU) cliqueMenu(button, state, x, y);
}

void keyboardCB(unsigned char key, int x, int y) {
    if(key == 27) exit(0);

    if(modoAtual == MODO_GAMEOVER && (key == 'r' || key == 'R')) {
        modoAtual = MODO_MENU;
        return;
    }

    if(modoAtual != MODO_JOGO) return;

    handlePlayerInput(&player, key);
}

void specialCB(int key, int x, int y) {
    if(modoAtual != MODO_JOGO) return;
    handlePlayerSpecial(&player, key);
}

void reshape(int w, int h) {
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / (float)h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

void initGL() {
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Configuração da luz
    GLfloat light_ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_diffuse[]  = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat light_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat light_position[] = { 5.0f, 10.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Material: especular e brilho
    GLfloat mat_specular[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat mat_shininess[] = { 16.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    // ESSENCIAL: Ativar uso de cor como material ambiente e difuso
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

}


int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1024, 600);
    glutCreateWindow("Prototipo");
    initGL();
    resetGame();

    if(!loadOBJ("tree.obj", &treeModel)) {
        fprintf(stderr, "Falha ao carregar modelo de arvore.\n");
    } else {
        float alturaTree = treeModel.maxY - treeModel.minY;
        if (alturaTree > 0.1f) {
            escalaArvoreDefault = 2.0f / alturaTree;
        }
    }

    initTrees();

    glutDisplayFunc(renderScene);
    glutIdleFunc(idleCB);
    glutKeyboardFunc(keyboardCB);
    glutSpecialFunc(specialCB);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseCB);

    glutMainLoop();

    freeModel(&treeModel);
    freeModel(&rockModel);
    freeModel(&logModel);

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
