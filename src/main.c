// Open GL
#include <GL/glut.h>
#include <GL/glu.h>
// STL

//#include <locale.h>

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/print.h"

#include "ecs/components/coin.h"
#include "ecs/components/floor.h"
#include "ecs/system.h" // menu

#include "game/state.h"
#include "game/tree.h"
#include "game/player.h"
#include "game/obstacle.h"
//#include "game/poste.h"
#include "ecs/systems/ranking.h"
#if defined(_WIN32) || defined(_WIN64)
    #include <direct.h>
#endif

Player player;

float world_speed = 12.0f;
float distanciaPercorrida = 0.0f;
static const float fator = 0.1f;

float z_far = 200.0f;

float escalaArvoreDefault = 1.0f;
float distanciaTotal = 0.0f;

int audio_mute = 0; // 0 = som ativado, 1 = mutado

typedef struct{
    int active;
    float x,y,z;
    float altura;
}Poste;

static Poste p;
static Poste q;

void initPoste(){
    p.active = 1;
    p.x = -3.0f;
    p.y = 0.0f;
    p.z = -60.0f;
    p.altura = 6.0f;

    q.active = 1;
    q.x = 8.0f;
    q.y = 0.0f;
    q.z = -60.0f;
    q.altura = 6.0f;
}

void updatePoste(float dt,float world_speed){
    if(p.active){
        p.z += dt*world_speed;
        if(p.z > 10.0f){
            p.z = -60.0f;
        }
    }
    if(q.active){
        q.z += dt*world_speed;
        if(q.z > 10.0f){
            q.z = -60.0f;
        }
    }

}

/*void drawPoste(){
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat light_position[] = { p.x, p.altura, p.z, 1.0f }; // Topo do poste
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 0.8f, 1.0f };   // Luz amarela suave
    GLfloat light_ambient[] = { 0.3f, 0.3f, 0.2f, 1.0f };   // Ambiente amarelado
    GLfloat light_specular[] = { 1.0f, 1.0f, 0.9f, 1.0f };  // Especular suave

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);


    glColor3f(0.0f,0.0f,0.0f);
    glPushMatrix();
    glTranslatef(p.x,p.altura/2,p.z);
    glScalef(0.5f,p.altura,0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(q.x,q.altura/2,q.z);
    glScalef(0.5f,q.altura,0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(p.x, p.altura + 0.3f, p.z);
    glColor3f(1.0f, 1.0f, 0.5f); // Amarelo brilhante
    glutSolidSphere(0.3f, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(q.x, q.altura + 0.3f, q.z);
    glColor3f(1.0f, 1.0f, 0.5f); // Amarelo brilhante
    glutSolidSphere(0.3f, 10, 10);
    glPopMatrix();
    
    glDisable(GL_COLOR_MATERIAL);
}*/

void drawPoste() {
    // Habilita uso de materiais baseados em cor
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    // ----- LUZ 1 (poste esquerdo) -----
    GLfloat light1_pos[] = { p.x, p.altura + 0.2f, p.z, 1.0f }; // w=1 => posicional
    GLfloat light1_diffuse[]  = { 1.0f, 0.95f, 0.7f, 1.0f };
    GLfloat light1_ambient[]  = { 0.05f, 0.045f, 0.035f, 1.0f };
    GLfloat light1_specular[] = { 0.8f, 0.8f, 0.7f, 1.0f };

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_ambient);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.05f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);

    // ----- LUZ 2 (poste direito) -----
    GLfloat light2_pos[] = { q.x, q.altura + 0.2f, q.z, 1.0f };
    GLfloat light2_diffuse[]  = { 1.0f, 0.95f, 0.7f, 1.0f };
    GLfloat light2_ambient[]  = { 0.05f, 0.045f, 0.035f, 1.0f };
    GLfloat light2_specular[] = { 0.8f, 0.8f, 0.7f, 1.0f };

    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  light2_diffuse);
    glLightfv(GL_LIGHT2, GL_AMBIENT,  light2_ambient);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION,    0.05f);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.01f);

    // ----- DESENHA OS POSTES -----
    glColor3f(0.15f, 0.15f, 0.15f); // cor cinza escuro

    glPushMatrix();
        glTranslatef(p.x, p.altura / 2, p.z);
        glScalef(0.4f, p.altura, 0.4f);
        glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(q.x, q.altura / 2, q.z);
        glScalef(0.4f, q.altura, 0.4f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // ----- TOPO BRILHANTE (esferas) -----
    GLfloat emissive[] = { 1.0f, 0.9f, 0.6f, 1.0f }; // brilho amarelo
    GLfloat no_emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
    glDisable(GL_LIGHTING); // garante que a esfera apareça brilhante

    glColor3f(1.0f, 1.0f, 0.5f);
    glPushMatrix();
        glTranslatef(p.x, p.altura + 0.3f, p.z);
        glutSolidSphere(0.25f, 16, 16);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(q.x, q.altura + 0.3f, q.z);
        glutSolidSphere(0.25f, 16, 16);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_emissive);
    glPopAttrib();

    glDisable(GL_COLOR_MATERIAL);
}


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

    glDisable(GL_LIGHTING);

    glColor3f(0,0,0);
    int offsets[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    for (int i = 0; i < 4; i++) {
        glRasterPos2i(x + offsets[i][0], invertedY + offsets[i][1]);
        for(const char *c = text; *c; c++) glutBitmapCharacter(font, *c);
    }

    glColor3f(r, g, b);
    glRasterPos2i(x, invertedY);
    for(const char *c = text; *c; c++) glutBitmapCharacter(font, *c);

    // Não reabilite iluminação aqui
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
    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0f, 1.0f, 1.0f);

    int rectWidth = w / 2;
    int rectHeight = 150;
    int rectX = (w - rectWidth) / 2;
    int rectY = (h - rectHeight) / 2;

    glBegin(GL_QUADS);
        glVertex2i(rectX, rectY);
        glVertex2i(rectX + rectWidth, rectY);
        glVertex2i(rectX + rectWidth, rectY + rectHeight);
        glVertex2i(rectX, rectY + rectHeight);
    glEnd();

    glFlush();

    float distanciaFinal = calcularDistanciaTotal();
    char buffer[128];
    int x;

    snprintf(buffer, sizeof(buffer), "Game over! ESC para voltar ao Menu");
    x = (w - getTextWidth(buffer, GLUT_BITMAP_TIMES_ROMAN_24)) / 2;
    drawText(buffer, x, h/2 + 80, GLUT_BITMAP_TIMES_ROMAN_24, 1, 0, 0, h);

    snprintf(buffer, sizeof(buffer), "Moedas coletadas: %d", getCoinCount());
    x = (w - getTextWidth(buffer, GLUT_BITMAP_TIMES_ROMAN_24)) / 2;
    drawText(buffer, x, h/2 + 40, GLUT_BITMAP_TIMES_ROMAN_24, 1, 1, 0, h);

    snprintf(buffer, sizeof(buffer), "Distancia total com bonus: %.1f metros", distanciaFinal);
    x = (w - getTextWidth(buffer, GLUT_BITMAP_TIMES_ROMAN_24)) / 2;
    drawText(buffer, x, h/2 + 10, GLUT_BITMAP_TIMES_ROMAN_24, 1, 1, 1, h);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // Restaura matrizes
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
    drawText(info, x, 30, GLUT_BITMAP_HELVETICA_12, 1, 1, 1, wh);
}

void resetGame() {
    audio_bus_stop_music();
    initPlayer(&player);

    initPoste();
    initObstacles();
    initObstacles();
    initCoins();

    world_speed = 12.0f;
    distanciaPercorrida = 0.0f;
}

void update(float dt) {
    if(modoAtual != MODO_JOGO) return;

    world_speed += dt * 0.5f;

    update_floor(dt);
    update_player(&player, dt);
    obstacleUpdate(dt);
    updatePoste(dt,world_speed);

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
            audio_bus_play_sfx(SFX_DAMAGE);
            print_info("Adicionando score: %.2f", finalScore);
            ranking_add(finalScore);
            ranking_save();
            modoAtual = MODO_GAME_OVER;
            audio_bus_stop_music();
            audio_bus_stop_all_channels();
            audio_bus_play_music(MUSIC_GAME_OVER, false);
            return;
        }
    }

    distanciaPercorrida += world_speed * dt * fator;

    updateTrees(dt, world_speed);
   // updateBushes(dt, world_speed);
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

        // DESABILITA ILUMINAÇÃO E DEPTH TEST PARA TEXTO 2D
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        drawRankingTitle(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        ranking_draw(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

        // REABILITA APÓS DESENHO
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glutSwapBuffers();
        return;
    }


#pragma region Render Game
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

    draw_floor();
    drawPlayer(&player);
    drawObstacles();
    drawCoins3D();
    drawTrees();
    drawPoste();
   // drawBushes();

    if(modoAtual == MODO_GAME_OVER) {
        drawGameOverHUD();
        glutSwapBuffers();
        return;
    }

    drawDistance(distanciaPercorrida);
    drawCoinsHUD(getCoinCount());

    glutSwapBuffers();
#pragma endregion
}


void idleCB() {
    static float last = 0.0f;
    float now = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    if(last == 0.0f)
        last = now;

    float dt = now - last;
    last = now;
    update(dt);
    glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y) {
    if(modoAtual == MODO_MENU)
        menu_motion(button, state, x, y);
}

void mouseMove(int x, int y) {
    if(modoAtual == MODO_MENU)
        menu_motion(-1, -1, x, y);
}

void keyboard_event(unsigned char key, int x, int y) {
    #define KEY_ENTER 13
    #define KEY_ESCAPE 27
    switch (key)
    {
        case KEY_ENTER: {
            if (modoAtual == MODO_MENU)
                menu_select_focused();
        } break;

        case KEY_ESCAPE: {
            switch (modoAtual) {
                case MODO_MENU:
                    exit(0);
                    break;
                case MODO_GAME_OVER:
                    modoAtual = MODO_MENU;
                    audio_bus_stop_music();
                    audio_bus_play_music(MUSIC_MENU, true);
                    break;
                case MODO_RANKING:
                    modoAtual = MODO_MENU;
                    audio_bus_play_sfx(SFX_UI_CANCEL);
                    break;
                default:
                    break;
            }
        } break;

        case 'p': {
            ranking_add(42.0f); // pontuação teste para salvar
            ranking_save();
            print_info("Ranking manual salvo pela tecla 'p'");
            return;
        } break;

         case 'g': case 'G': {
            if (modoAtual == MODO_JOGO) {
                float finalScore = calcularDistanciaTotal();
                audio_bus_play_sfx(SFX_DAMAGE);
                print_info("Game over! Score: %.2f", finalScore);
                ranking_add(finalScore);
                ranking_save();
                modoAtual = MODO_GAME_OVER;
                audio_bus_stop_music();
                audio_bus_stop_all_channels();
                audio_bus_play_music(MUSIC_GAME_OVER, false);
                return;
            }
        } break;

        // 'm' para mute/unmute
        case 'm': case 'M':
            audio_mute = !audio_mute;
            if (audio_mute) {
                audio_bus_stop_music();
                audio_bus_stop_all_channels();
            } else {
                // opcional: retomar música
                if (modoAtual == MODO_MENU)
                    audio_bus_play_music(MUSIC_MENU, true);
                else if (modoAtual == MODO_JOGO)
                    audio_bus_play_music(MUSIC_GAME, true);
        }
        print_info("Áudio %s", audio_mute ? "desativado" : "ativado");
        break;


        default:
            break;
    }

    if(modoAtual == MODO_JOGO)
        handlePlayerInput(&player, key);
}

void specialCB(int key, int x, int y) {
    switch (modoAtual)
    {
        case MODO_MENU:
            switch (key) {
                case GLUT_KEY_UP:
                    menu_focus_previous();
                    break;
                case GLUT_KEY_DOWN:
                    menu_focus_next();
                    break;
                default: break;
            } break;

        case MODO_JOGO:
            handlePlayerSpecial(&player, key);
            break;

        default:
            break;
    }
}

void reshape(int w, int h) {
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / (float)h, 0.1, z_far);
    glMatrixMode(GL_MODELVIEW);
}

void initGL() {
    // Define a cor de fundo da tela 
    glClearColor(0.04f, 0.02f, 0.1f, 1.0f); //azul escuro quase preto

    #pragma region Enable Culling and Depth Test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    #pragma endregion

    #pragma region Anti-Aliasing // MSAA
        glEnable(GLUT_MULTISAMPLE);
        // Se necessário, descomente para suavizar mais
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glEnable(GL_LINE_SMOOTH);
        //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        //glEnable(GL_POLYGON_SMOOTH);
        //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    #pragma endregion

    #pragma region Enable Lighting
        glEnable(GL_LIGHTING);

        glEnable(GL_LIGHT0);

        GLfloat light_ambient[]   = { 0.04f, 0.04f, 0.06f, 1.0f };
        GLfloat light_diffuse[]   = { 0.12f, 0.12f, 0.18f, 1.0f };
        GLfloat light_specular[]  = { 0.02f, 0.02f, 0.03f, 1.0f };
        GLfloat light_direction[] = { -0.8f, -0.7f, 0.4f, 0.0f };

        glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glLightfv(GL_LIGHT0, GL_POSITION, light_direction);

        glEnable(GL_LIGHT1);

        GLfloat light1_ambient[] = {0.2f, 0.2f, 0.1f, 1.0f};
        GLfloat light1_diffuse[] = {0.8f, 0.8f, 0.6f, 1.0f};
        GLfloat light1_specular[] = {0.9f, 0.9f, 0.8f, 1.0f};

        glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

        glEnable(GL_LIGHT2);

        GLfloat light2_ambient[]   = { 0.01f, 0.01f, 0.015f, 1.0f };
        GLfloat light2_diffuse[]   = { 0.25f, 0.25f, 0.2f, 1.0f };
        GLfloat light2_specular[]  = { 0.1f, 0.1f, 0.08f, 1.0f };

        glLightfv(GL_LIGHT2, GL_AMBIENT,  light2_ambient);
        glLightfv(GL_LIGHT2, GL_DIFFUSE,  light2_diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);

        glEnable(GL_LIGHT3);

        GLfloat light3_ambient[]   = { 0.02f, 0.02f, 0.03f, 1.0f };
        GLfloat light3_diffuse[]   = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat light3_specular[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat light3_direction[] = { 0.0f, -1.0f, 0.0f, 0.0f };

        glLightfv(GL_LIGHT3, GL_AMBIENT, light3_ambient);
        glLightfv(GL_LIGHT3, GL_DIFFUSE, light3_diffuse);
        glLightfv(GL_LIGHT3, GL_SPECULAR, light3_specular);
        glLightfv(GL_LIGHT3, GL_POSITION, light3_direction);


    #pragma endregion
    
    #pragma region Material Setup
        GLfloat mat_specular[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat mat_shininess[] = { 16.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

        // Ativar uso de cor como material ambiente e difuso
        //glEnable(GL_COLOR_MATERIAL);

        // desabilitar color material para garantir que glMaterial tenha efeito
        glDisable(GL_COLOR_MATERIAL);

        // normalização automática (útil se houver escalas)
        glEnable(GL_NORMALIZE);
    #pragma endregion
}

static void game_run() {
    world_speed = 12.0f;
    distanciaPercorrida = 0.0f;

    initObstacles();
    init_floor();
    initCoinModel();
    initCoins();
    initTrees();
    //initBushes();
    init_rock_model();
    init_logs_model();

    audio_bus_play_music(MUSIC_MENU, true);
}

int main(int argc, char** argv) {
    // DEBUG ->  excluir depois [@GersonFeDutra: ainda usando?]
#if defined(_WIN32) || defined(_WIN64)
    char cwd[1024];
    if (_getcwd(cwd, sizeof(cwd)) != NULL) {
        print_info("diretório atual: %s", cwd);
    } else {
        perror("erro ao obter diretório atual");
    }
#endif
    srand((unsigned)time(NULL));
    //setlocale(LC_ALL, "pt_BR.UTF-8");
    //até aqui

    int err; // error code
    int window_size[2] = {1024, 600};

    #pragma region GLUT Init
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
        glutInitWindowSize(window_size[0], window_size[1]);
        glutCreateWindow("Trabalho CG");
        initGL();
    #pragma endregion

    #pragma region SDL2 Init
        err = SDL_Init(SDL_INIT_AUDIO);
        if (err != EXIT_SUCCESS) {
            print_error("SDL initialization failed: %s", SDL_GetError());
            return err;
        }
        // TODO -> Suporte para WAV
        // Inicializa SDL_mixer com suporte a MP3
        int flags = MIX_INIT_MP3;
        if ((Mix_Init(flags) & flags) != flags) {
            print_error("SDL Mixer initialization failed: %s\n", Mix_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
            print_error("SDL Failed to load Audio device: %s\n", Mix_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }
        audio_bus_init();
    #pragma endregion

    ranking_load();
    if (ranking_getCount() == 0) {
        ranking_save();
    }

    game_run(); // game content created here

    #pragma region Event Bindings
        glutDisplayFunc(renderScene);
        glutIdleFunc(idleCB);
        glutKeyboardFunc(keyboard_event);
        glutSpecialFunc(specialCB);
        glutReshapeFunc(reshape);
        glutMouseFunc(mouseCB);
        glutPassiveMotionFunc(mouseMove);
    #pragma endregion

    glutMainLoop();

    #pragma region Free Memory
        free_rock_model();
        free_logs_model();
        free_coin_model();
        free_tree_model();
       // free_bush_model();
        audio_bus_close();
        Mix_Quit();
        SDL_Quit();
    #pragma endregion

    return 0;
}
