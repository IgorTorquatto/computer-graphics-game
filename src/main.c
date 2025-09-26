#include <GL/glut.h>
#include <GL/glu.h>

#include <math.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>

#include "utils/print.h"

#include "ecs/component.h" // model
#include "ecs/system.h" // menu

#include "game/state.h"
#include "game/tree.h"
#include "game/player.h"
#include "game/obstacle.h"

#include "ecs/systems/ranking.h"
#include <direct.h>

Player player;

float worldSpeed = 12.0f;
float distanciaPercorrida = 0.0f;
static const float fator = 0.1f;

Model treeModel;
Model bushModel;

float escalaArvoreDefault = 1.0f;

extern Model rockModel;
extern Model logModel;

float distanciaTotal = 0.0f;
float calcularDistanciaTotal() {
    int moedas = getCoinCount();
    int bonusPorGrupo = moedas / 10;  // grupos de 10 moedas
    float percentualBonus = 0.05f;    // 5% por grupo de 10 moedas
    float bonus = distanciaPercorrida * percentualBonus * bonusPorGrupo;
    return distanciaPercorrida + bonus;
}

int getTextWidth(const char *text, void *font) {
    int width = 0;
    for (; *text; ++text) width += glutBitmapWidth(font, *text);
    return width;
}

GLboolean validarRasterPos(int x, int y) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    return (x >= viewport[0] && x < viewport[0]+viewport[2]
        && y >= viewport[1] && y < viewport[1]+viewport[3]);
}

void drawText(const char *text, int x, int y, void *font, float r, float g, float b, int windowHeight) {
    int invertedY = windowHeight - y;

    // Desenha contorno preto
    glColor3f(0,0,0);
    int offsets[8][2] = {
        {-1,-1},{0,-1},{1,-1},
        {-1,0},       {1,0},
        {-1,1},{0,1},{1,1}};
    for(int i=0;i<8;i++){
        int ox = x + offsets[i][0];
        int oy = invertedY + offsets[i][1];
        if(!validarRasterPos(ox, oy)) continue;
        glRasterPos2i(ox, oy);
        for(const char *c = text; *c; c++) glutBitmapCharacter(font, *c);
    }
    glFlush();

    // Desenha texto colorido
    if(!validarRasterPos(x, invertedY)) return;
    glColor3f(r, g, b);
    glRasterPos2i(x, invertedY);
    for(const char *c = text; *c; c++) glutBitmapCharacter(font, *c);
}

void drawGameOverHUD() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);

    float distanciaFinal = calcularDistanciaTotal();
    char buffer[128];
    int x;

    snprintf(buffer, sizeof(buffer), "Game over! R para voltar ao Menu");
    x = (w - getTextWidth(buffer, GLUT_BITMAP_TIMES_ROMAN_24)) / 2;
    drawText(buffer, x, h/2 + 80, GLUT_BITMAP_TIMES_ROMAN_24, 1, 0, 0, h);

    snprintf(buffer, sizeof(buffer), "Moedas coletadas: %d", getCoinCount());
    x = (w - getTextWidth(buffer, GLUT_BITMAP_TIMES_ROMAN_24)) / 2;
    drawText(buffer, x, h/2 + 40, GLUT_BITMAP_TIMES_ROMAN_24, 1, 1, 0, h);

    snprintf(buffer, sizeof(buffer), "Distancia total com bonus: %.1f metros", distanciaFinal);
    x = (w - getTextWidth(buffer, GLUT_BITMAP_TIMES_ROMAN_24)) / 2;
    drawText(buffer, x, h/2 + 10, GLUT_BITMAP_TIMES_ROMAN_24, 1, 1, 1, h);

    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Colisão AABB
int aabbCollision(float ax, float ay, float az, float aw, float ah, float ad,
                  float bx, float by, float bz, float bw, float bh, float bd) {
    if (fabs(ax - bx) * 2.0f < (aw + bw) &&
        fabs(ay - by) * 2.0f < (ah + bh) &&
        fabs(az - bz) * 2.0f < (ad + bd)) return 1;
    return 0;
}

void drawRankingTitle(int ww, int wh) {
    char title[] = "Ranking - Distancias";
    int x = (ww - getTextWidth(title, GLUT_BITMAP_HELVETICA_18)) / 2;
    drawText(title, x, wh/2 + 120, GLUT_BITMAP_HELVETICA_18, 1, 1, 1, wh);

    char info[] = "Pressione ESC para voltar";
    x = (ww - getTextWidth(info, GLUT_BITMAP_HELVETICA_12)) / 2;
    drawText(info, x, wh/2 - 150, GLUT_BITMAP_HELVETICA_12, 1, 1, 1, wh);
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

    update_player(&player, dt);

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
            float finalScore = calcularDistanciaTotal();
            printf("Adicionando score: %.2f\n", finalScore);
            ranking_add(finalScore);
            ranking_save();
            modoAtual = MODO_GAMEOVER;
        }
    }

    distanciaPercorrida += worldSpeed * dt * fator;

    updateTrees(dt, worldSpeed);
    updateBushes(dt, worldSpeed);
    updateCoins(dt);
}

void renderScene() {
    if(modoAtual == MODO_MENU) {
        desenhaMenu();
        return;
    }

    if(modoAtual == MODO_RANKING) {
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    ranking_draw(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
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
    drawBushes();

    if(modoAtual == MODO_GAMEOVER) {
        drawGameOverHUD();
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
    if(key == 'p') {
        ranking_add(42.0f); // pontuação teste para salvar
        ranking_save();
        printf("Ranking manual salvo pela tecla 'p'\n");
        return;
    }

    if(key == 27) { // ESC
        if(modoAtual == MODO_RANKING) {
            modoAtual = MODO_MENU;
            return;
        }
        exit(0);
    }

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

#pragma region Anti-Aliasing // MSAA
    glEnable(GLUT_MULTISAMPLE);
    // Smoothing -> Modo antigo de anti-alias do opengl
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_POLYGON_SMOOTH);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
#pragma endregion

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


static int start_game() {
    resetGame();

    if(!loadOBJ("tree.obj", &treeModel)) {
        print_error("Falha ao carregar modelo de arvore.");
        exit(EXIT_FAILURE);
    } else {
        float alturaTree = treeModel.maxY - treeModel.minY;
        if (alturaTree > 0.1f) {
            escalaArvoreDefault = 2.0f / alturaTree;
        }
    }

    if(!loadOBJ("bush.obj", &bushModel)) {
        print_error("Falha ao carregar modelo de arvore.");
        exit(EXIT_FAILURE);
    }

    initCoinModel();
    initCoins();
    initTrees();
    initBushes();
}

int main(int argc, char** argv) {
    //para debug excluir depois
    char cwd[1024];
    if (_getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("diretorio atual: %s\n", cwd);
    } else {
        perror("erro ao obter diretorio atual");
    }
    srand((unsigned)time(NULL));
    //até aqui

    int window_size[2] = {1024, 600};

    // Glut Init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(window_size[0], window_size[1]);
    glutCreateWindow("Trabalho CG");
    initGL();

    ranking_load();
    if (ranking_getCount() == 0) {
        ranking_save();
    }

    start_game(); // game content created here

    // Event Bindings
    glutDisplayFunc(renderScene);
    glutIdleFunc(idleCB);
    glutKeyboardFunc(keyboardCB);
    glutSpecialFunc(specialCB);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseCB);

    glutMainLoop();

    // Clear memory
    freeModel(&treeModel);
    freeModel(&rockModel);
    freeModel(&logModel);

    return 0;
}
