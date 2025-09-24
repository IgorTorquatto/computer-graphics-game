#include "hud.h"
#include <GL/glut.h>
#include <string.h>
#include <stdio.h>


/* Função para desenhar a distância percorrida */
void drawDistance(float distanciaPercorrida) {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%.1f m", distanciaPercorrida);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);

    glRasterPos2i(w - 10 * (int)strlen(buffer), h - 20);

    for (const char *c = buffer; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/* Função para desenhar o contador de moedas (opcional duplicado, pode chamar de coin.c) */
void drawCoinsHUD(int coinCount) {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Moedas: %d", coinCount);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glColor3f(1.0f, 1.0f, 0.0f);
    int px = w - 10 * (int)strlen(buffer);
    int py = h - 44;
    glRasterPos2i(px, py);

    for (const char *c = buffer; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
