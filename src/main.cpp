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

#include <algorithm> // std::fill
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
    for (EntityId id : geometry_samples) {
        if (id == INVALID_ENTITY)
            continue;
        GeometryId geometry_id = game_objects[id].geometry.id;
        switch (get_geometry_type(id)) {
            case GeometryType::SPHERE:
                get_sphere_ref(geometry_id).slices = slices;
                get_sphere_ref(geometry_id).stacks = stacks;
                break;

            case GeometryType::TORUS:
                get_torus_ref(geometry_id).slices = slices;
                get_torus_ref(geometry_id).stacks = stacks;
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
    std::fill(geometry_samples, geometry_samples + sizeof(geometry_samples) / sizeof(EntityId), INVALID_ENTITY);

    const double speed = 1.5f;

    EntityId sphere_object = create_game_object();
    GeometryId sphere = add_sphere(sphere_object, 1.0f, Position{-2.4f, 1.2f, -6.0f});
    rotate_x(sphere_object, deg_to_rad(60));
    set_color(sphere_object, color_red);
    set_rotation_velocity(sphere_object, Velocity{0.0f, 0.0f, speed});
    get_sphere_ref(sphere).slices = slices;
    get_sphere_ref(sphere).stacks = stacks;
    geometry_samples[0] = sphere_object;

    EntityId torus_object = create_game_object();
    GeometryId torus = add_torus(torus_object, 0.2f, 0.8f, Position{2.4f, 1.2f, -6.0f});
    rotate_x(torus_object, deg_to_rad(60));
    set_color(torus_object, color_red);
    set_rotation_velocity(torus_object, Velocity{0.0f, 0.0f, speed});
    get_torus_ref(torus).slices = slices;
    get_torus_ref(torus).stacks = stacks;
    geometry_samples[1] = torus_object;

    create_test_object();

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


static void key(unsigned char key, int x, int y)
{
    const unsigned char esc = 27;

    switch (key)
    {
        case esc:
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
            input_system({key, glutGetModifiers(), {(float)x, (float)y}, InputType::KEYBOARD});
            break;
    }

    glutPostRedisplay();
}


/* Program entry point */
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