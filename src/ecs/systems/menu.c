#include <GL/glut.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <string.h>

#include "menu.h"
#include "game/state.h"
#include "ecs/systems/music_bus.h"

#define LARGURA_BOTAO 240
#define ALTURA_BOTAO 60

extern void resetGame(); // função definida em main.c

enum button { BUTTON_PLAY, BUTTON_RANK, BUTTON_EXIT, BUTTON_NONE };
static enum button focus_on = BUTTON_NONE;

static void draw_button_at(enum button id, int x, int y, int largura, int altura, const char* texto) {
    // fundo do botão
    glColor3f(0.5f, 0.7f, 1.0f); // azul claro
    glBegin(GL_QUADS);
        glVertex2f((float)x, (float)y);
        glVertex2f((float)(x + largura), (float)y);
        glVertex2f((float)(x + largura), (float)(y + altura));
        glVertex2f((float)x, (float)(y + altura));
    glEnd();

    // se estiver em foco, desenhar borda
    if (focus_on == id) {
        glLineWidth(3.0f); // espessura da borda
        glColor3f(1.0f, 1.0f, 1.0f); // cor da borda: branca
        glBegin(GL_LINE_LOOP);
            glVertex2f((float)x, (float)y);
            glVertex2f((float)(x + largura), (float)y);
            glVertex2f((float)(x + largura), (float)(y + altura));
            glVertex2f((float)x, (float)(y + altura));
        glEnd();
    }

    // texto centralizado
    glColor3f(1.0f, 1.0f, 1.0f);
    int len = 0;
    const char* p;
    for (p = texto; *p; ++p)
        ++len;

    float approxCharW = 9.0f; // aproximação para GLUT_BITMAP_HELVETICA_18
    float textWidth = len * approxCharW;
    float rx = x + (largura - textWidth) * 0.5f;
    float ry = y + altura / 2 - 9; // centralizar verticalmente
    glRasterPos2f(rx, ry);

    for (p = texto; *p; ++p)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
}


/* desenha menu (usa projeção ortográfica temporária) */
void desenhaMenu() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int ww = glutGet(GLUT_WINDOW_WIDTH);
    int wh = glutGet(GLUT_WINDOW_HEIGHT);

    // Posições centralizadas dos botões
    int bx = (ww - LARGURA_BOTAO) / 2;
    int gap = 80;
    int by_play = wh / 2 + gap;
    int by_ranking = wh / 2;
    int by_exit = wh / 2 - gap;


    // Projeção 2D temporária
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ww, 0, wh);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Título simples
    glColor3f(0.01f, .1f, 0.05f);
    const char* title = "CORRA DO BICHO DO MATO!";
    float titleX = (ww - (int)strlen(title) * 10) * 0.5f;
    glRasterPos2f(titleX - 10, wh - 80);
    for (const char* c = title; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    // Botões
    draw_button_at(BUTTON_PLAY, bx, by_play, LARGURA_BOTAO, ALTURA_BOTAO, "JOGAR");
    draw_button_at(BUTTON_RANK, bx, by_ranking, LARGURA_BOTAO, ALTURA_BOTAO, "RANKING");
    draw_button_at(BUTTON_EXIT, bx, by_exit, LARGURA_BOTAO, ALTURA_BOTAO, "SAIR");

    // Restaura matrizes
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Restaura contexto 3D
    glEnable(GL_DEPTH_TEST);

    glutSwapBuffers();
}

static void select_button_game()
{
    modoAtual = MODO_JOGO;
    resetGame(); // inicia o jogo imediatamente
    audio_bus_play_sfx(SFX_UI_SELECT);
    audio_bus_play_music(MUSIC_GAME, true);
}

static void select_button_rank()
{
    modoAtual = MODO_RANKING; // novo estado para mostrar ranking
    audio_bus_play_sfx(SFX_UI_SELECT);
}

static void select_button_exit()
{
    audio_bus_play_sfx(SFX_UI_SELECT);
    exit(0);
}

void menu_motion(int button, int state, int x, int y)
{
    int ww = glutGet(GLUT_WINDOW_WIDTH);
    int wh = glutGet(GLUT_WINDOW_HEIGHT);
    int my = wh - y; // converte para coordenadas bottom-left

    int bx = (ww - LARGURA_BOTAO) / 2;
    int gap = 80;
    int by_play = wh / 2 + gap;
    int by_ranking = wh / 2;
    int by_exit = wh / 2 - gap;

    // Jogar
    if (x >= bx && x <= bx + LARGURA_BOTAO &&
            my >= by_play && my <= by_play + ALTURA_BOTAO)
    {
        if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
            if (focus_on != BUTTON_PLAY) {
                focus_on = BUTTON_PLAY;
                audio_bus_play_sfx(SFX_UI_1);
            }
            return;
        }
        select_button_game();
        return;
    }

    // Ranking
    if (x >= bx && x <= bx + LARGURA_BOTAO &&
            my >= by_ranking && my <= by_ranking + ALTURA_BOTAO)
    {
        if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
            if (focus_on != BUTTON_RANK) {
                focus_on = BUTTON_RANK;
                audio_bus_play_sfx(SFX_UI_1);
            }
            return;
        }
        select_button_rank();
        return;
    }

    // Sair
    if (x >= bx && x <= bx + LARGURA_BOTAO &&
            my >= by_exit && my <= by_exit + ALTURA_BOTAO)
    {
        if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
            if (focus_on != BUTTON_EXIT) {
                focus_on = BUTTON_EXIT;
                audio_bus_play_sfx(SFX_UI_1);
            }
            return;
        }
        select_button_exit();
        return;
    }
}


void menu_focus_next()
{
    switch (focus_on) {
        case BUTTON_PLAY: focus_on = BUTTON_RANK; break;
        case BUTTON_RANK: focus_on = BUTTON_EXIT; break;
        case BUTTON_EXIT: focus_on = BUTTON_PLAY; break;
        default: focus_on = BUTTON_PLAY; break;
    }
    audio_bus_play_sfx(SFX_UI_1);
}

void menu_focus_previous()
{
    switch (focus_on) {
        case BUTTON_PLAY: focus_on = BUTTON_EXIT; break;
        case BUTTON_RANK: focus_on = BUTTON_PLAY; break;
        case BUTTON_EXIT: focus_on = BUTTON_RANK; break;
        default: focus_on = BUTTON_PLAY; break;
    }
    audio_bus_play_sfx(SFX_UI_2);
}

void menu_select_focused()
{
    switch (focus_on) {
        case BUTTON_PLAY: select_button_game(); break;
        case BUTTON_RANK: select_button_rank(); break;
        case BUTTON_EXIT: select_button_exit(); break;
        default: break;
    }
}
