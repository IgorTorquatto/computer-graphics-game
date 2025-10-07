#include <GL/glut.h>

// Implementation defined in model.c
#include "stb_image.h"

#pragma region Local Includes
    #include "utils/print.h"
    #include "utils/basics.h" // math
#pragma endregion

#include "floor.h"

extern float world_speed; // Variável global do jogo para velocidade do mundo

#pragma region Locals
    static GLuint floorTex = 0; // id da textura

    // tamanho do "tile" no mundo (cada tile usará 1 repetição da textura)
    static const float tile_size = 8.0f; // ajuste: 2.0 unidades por tile
    static float texture_offset = 0.0f; // acumulador
#pragma endregion


void update_floor(float delta)
{
    // Atualiza deslocamento (quanto maior, mais rápido o scroll)
    texture_offset += delta * world_speed / tile_size; // velocidade do scroll

    // mantém em [0, 1] para evitar overflow
    texture_offset = fmod(texture_offset, 1.0f);
}

void draw_floor()
{
    GLfloat floor_color[] = { 0.8f, 0.9f, 0.5f, 1.0f };
    GLfloat floor_specular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat floor_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, floor_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, floor_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, floor_diffuse);

    if (floorTex == 0) {
        // Draw floor without texture
        glBegin(GL_QUADS);
            glVertex3f(-1.5f, 0.0f, 5.0f);
            glVertex3f(6.5f, 0.0f, 5.0f);
            glVertex3f(6.5f, 0.0f, -z_far);
            glVertex3f(-1.5f, 0.0f, -z_far);
        glEnd();
        return;
    }

    // Draw floor with texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floorTex);

#pragma region Texture Repetition
    float x0 = -1.5f;
    float x1 = 6.5f;
    float z0 = 5.0f;
    float z1 = -z_far;


    float width = abs(x1 - x0);
    float depth = abs(z1 - z0);

    // quantas vezes a textura deve repetir em U/V
    float repeatU = width  / tile_size;
    float repeatV = depth  / tile_size;
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#pragma endregion

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.0f, texture_offset, 0.0f); // desloca no eixo V (t)
    glScalef(repeatU, repeatV, 1.0f); // por exemplo repeatU=8, repeatV=20
    glMatrixMode(GL_MODELVIEW);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f); // chão no plano XZ

        // canto inferior esquerdo
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x0, 0.0f, z0);

        // canto inferior direito
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(x1, 0.0f, z0);

        // canto superior direito
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(x1, 0.0f, z1); // repete a textura no eixo Z

        // canto superior esquerdo
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(x0, 0.0f, z1);
    glEnd();

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);
}


void init_floor()
{
    int width, height, channels;
    unsigned char* data = stbi_load(IMG_PATH_FLOOR, &width, &height, &channels, 0);

    if (data)
        print_success("Textura " IMG_PATH_FLOOR " carregada com sucesso!");
    else {
        print_error("Erro ao carregar textura: " IMG_PATH_FLOOR);
        floorTex = 0;
        return;
    }

    GLenum format;
    switch (channels) {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default: break;
    }

    glGenTextures(1, &floorTex);
    glBindTexture(GL_TEXTURE_2D, floorTex);

    // parâmetros de repetição
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // filtros
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // envia a imagem pra GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}
