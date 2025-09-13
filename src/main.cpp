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
 */

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>

// === ECS ===
#include "ecs/entity.h"
#include "ecs/component.h"
#include "ecs/system.h"


static int slices = 16;
static int stacks = 16;

#define rad_to_deg(radians) ((float)(radians) * 180 / M_PI)
#define deg_to_rad(degrees) ((float)(degrees) * M_PI / 180)


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

    if (EntityId e = create_entity(); e != INVALID_ENTITY) {
        set_color(e, Color{1, 1, 0});
        //set_position(e, Position{0, 0, 0});
        const size_t count = 12;
        Position *vertices = new Position[count * 3];
        Position v[8] = {
            {-0.5f, -0.5f, -0.5f}, // 0
            { 0.5f, -0.5f, -0.5f}, // 1
            { 0.5f,  0.5f, -0.5f}, // 2
            {-0.5f,  0.5f, -0.5f}, // 3
            {-0.5f, -0.5f,  0.5f}, // 4
            { 0.5f, -0.5f,  0.5f}, // 5
            { 0.5f,  0.5f,  0.5f}, // 6
            {-0.5f,  0.5f,  0.5f}  // 7
        };
        int faces[12][3] = {
            // fundo (z = -0.5)
            {0, 1, 2}, {0, 2, 3},
            // topo (z = +0.5)
            {4, 6, 5}, {4, 7, 6},
            // frente (y = +0.5)
            {3, 2, 6}, {3, 6, 7},
            // trás (y = -0.5)
            {0, 5, 1}, {0, 4, 5},
            // direita (x = +0.5)
            {1, 5, 6}, {1, 6, 2},
            // esquerda (x = -0.5)
            {0, 3, 7}, {0, 7, 4}
        };
        for (size_t i = 0; i < count; i++) {
            vertices[i*3 + 0] = v[faces[i][0]];
            vertices[i*3 + 1] = v[faces[i][1]];
            vertices[i*3 + 2] = v[faces[i][2]];
        }
        add_polygon(e, vertices, count * 3);
        //translate(e, Position{0, 0, -5.0f});
        set_position(e, Position{0, 0, -5.0f});

        rotate_x(e, deg_to_rad(30));
        rotate_y(e, deg_to_rad(-45));
    }

    return EXIT_SUCCESS;
}


/* GLUT callback Handlers */

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);

    glPushMatrix();
        glTranslated(-2.4,1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutSolidSphere(1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutSolidCone(1,1,slices,stacks);
    glPopMatrix();

    glPushMatrix();
        glTranslated(2.4,1.2,-6);
        glRotated(60,1,0,0);
        glRotated(a,0,0,1);
        glutSolidTorus(0.2,0.8,slices,stacks);
    glPopMatrix();

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
}


static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;

        case '+':
            slices++;
            stacks++;
            break;

        case '-':
            if (slices>3 && stacks>3)
            {
                slices--;
                stacks--;
            }
            break;
    }

    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT Shapes");

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
