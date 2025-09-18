#include <GL/glut.h>
#include "menu.h"
#include "estado.h"

#define LARGURA_BOTAO 200
#define ALTURA_BOTAO 50

#define BOTAO_JOGAR_X 412
#define BOTAO_JOGAR_Y 350

#define BOTAO_SAIR_X 412
#define BOTAO_SAIR_Y 250

extern void resetGame(); // garante que resetGame() definido em main.cpp será linkado

// Função para desenhar cada botão
void desenhaBotao(int x, int y, int largura, int altura, const char* texto) {
    glColor3f(0.2f, 0.5f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + largura, y);
        glVertex2f(x + largura, y + altura);
        glVertex2f(x, y + altura);
    glEnd();

    // Desenha texto centralizado no botão
    glColor3f(1, 1, 1);
    int len = 0;
    const char* c;
    for (c = texto; *c != '\0'; c++) len++;
    glRasterPos2f(x + largura/2 - 4*len, y + altura/2 - 4); // ajuste para centralizar
    for (c = texto; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

// Desenha o menu completo
void desenhaMenu() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configura projeção 2D temporária para o menu
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1024, 0, 600);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    desenhaBotao(BOTAO_JOGAR_X, BOTAO_JOGAR_Y, LARGURA_BOTAO, ALTURA_BOTAO, "Jogar");
    desenhaBotao(BOTAO_SAIR_X, BOTAO_SAIR_Y, LARGURA_BOTAO, ALTURA_BOTAO, "Sair");

    // Restaura matrizes originais
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

// Função para tratar clique do mouse no menu
void cliqueMenu(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int my = 600 - y; // altura da janela

        // Jogar
        if (x >= BOTAO_JOGAR_X && x <= BOTAO_JOGAR_X + LARGURA_BOTAO &&
            my >= BOTAO_JOGAR_Y && my <= BOTAO_JOGAR_Y + ALTURA_BOTAO) {
            modoAtual = MODO_JOGO;
            resetGame(); // inicia o jogo imediatamente
        }

        // Sair
        if (x >= BOTAO_SAIR_X && x <= BOTAO_SAIR_X + LARGURA_BOTAO &&
            my >= BOTAO_SAIR_Y && my <= BOTAO_SAIR_Y + ALTURA_BOTAO) {
            exit(0);
        }
    }
}
