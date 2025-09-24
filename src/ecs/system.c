#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include "entity.h"
#include "component.h"
#include "system.h"

#include "stdio.h"
#include "string.h"


static void render_mesh(const Mesh* mesh, const Transform* model) {
    Color color = mesh->color;
    C_Polygon polygon = mesh->polygon;

    // loads mesh color
    glColor3f(color.r, color.g, color.b);

    glPushMatrix();
        // loads model matrix
        // glMatrixMode(GL_MODELVIEW);
        // glLoadIdentity(); // reset
        float m[16] = to_GLfloat_matrix(model);
        glMultMatrixf(m); // multiplica pela matriz do objeto

        glBegin(GL_TRIANGLES);
            // loads polygon vertices
            for (size_t i = 0; i < polygon.vertices_count; ++i) {
                Position position = polygon.vertices[i];
                glVertex3f(position.x, position.y, position.z);
            }
        glEnd();
    glPopMatrix();
}


static void render_sphere(const Sphere* sphere, const Transform* model) {
    Color color = sphere->color;
    Position position = sphere->center;

    glColor3f(color.r, color.g, color.b);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glTranslatef(position.x, position.y, position.z);

        float m[16] = to_GLfloat_matrix(model);
        glMultMatrixf(m);

        if (sphere->isSolid)
            glutSolidSphere(sphere->radius, sphere->slices, sphere->stacks);
        else
            glutWireSphere(sphere->radius, sphere->slices, sphere->stacks);
    glPopMatrix();
}


static void render_cuboid(const Cuboid* cuboid, const Transform* model) {
    Color color = cuboid->color;
    Position position = cuboid->center;

    glColor3f(color.r, color.g, color.b);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        // primeiro leva o objeto até a posição no mundo
        glTranslatef(position.x, position.y, position.z);
        // depois aplica rotações/escala
        float m[16] = to_GLfloat_matrix(model);
        glMultMatrixf(m);

        if (cuboid->isSolid)
            glutSolidCube(1);
        else
            glutWireCube(1);
    glPopMatrix();
}


static void render_cube(const Cube* cube, const Transform* model) {
    Color color = cube->color;
    Vector dimensions = cube->dimensions;
    Position position = cube->center;

    glColor3f(color.r, color.g, color.b);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glScalef(dimensions.x, dimensions.y, dimensions.z);

        glTranslatef(position.x, position.y, position.z);
        float m[16] = to_GLfloat_matrix(model);
        glMultMatrixf(m);

        if (cube->isSolid)
            glutSolidCube(1);
        else
            glutWireCube(1);
    glPopMatrix();
}


static void render_torus(const Torus* torus, const Transform* model) {
    Color color = torus->color;
    Position position = torus->center;
    float outer_radius = torus->outerRadius;
    float inner_radius = torus->innerRadius;

    glColor3f(color.r, color.g, color.b);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        float m[16] = to_GLfloat_matrix(model);
        glMultMatrixf(m);

        if (torus->isSolid)
            glutSolidTorus(inner_radius, outer_radius, torus->slices, torus->stacks);
        else
            glutWireTorus(inner_radius, outer_radius, torus->slices, torus->stacks);
    glPopMatrix();

}


static void render_geometry(Geometry geometry) {
    switch (geometry.type) {
        case MESH:
            render_mesh(&meshes[geometry.id], &geometry.model);
            break;
        case SPHERE:
            render_sphere(&spheres[geometry.id], &geometry.model);
            break;
        case CUBOID:
            render_cuboid(&cuboids[geometry.id], &geometry.model);
            break;
        case CUBE:
            render_cube(&cubes[geometry.id], &geometry.model);
            break;
        case TORUS:
            render_torus(&toruses[geometry.id], &geometry.model);
            break;
        default:
            break;
    }
}

static void render_text(Label label) {
    char *text = label.text;
    Position at = label.position;
    Color color = label.color;

    glColor3f(color.r, color.g, color.b);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        // projection em pixels: (0,0)=esq/inf, (w,h)=dir/top
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        glOrtho(0, w, 0, h, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();

            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);

            // glRasterPos usa o sistema atual (x, y) em pixels
            glRasterPos2f(at.x, at.y);

            for (const char *c = text; *c; c++)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void system_render() {

    for (EntityId id = 0; id < game_objects_count; ++id)
        render_geometry(game_objects[id].geometry);

    for (EntityId id = 0; id < labels_count; ++id)
        render_text(labels[id]);
}


void system_process(DeltaTime delta)
{
    for (int e = 0; e < game_objects_count; e++)
        if (game_objects[e].process != nullptr)
            game_objects[e].process(delta);

    // Fill text buffer with approximate delta time
    // Assumes delta is always less than 10sec
    char template_string[] = "△t: 0000ms\t" "fps:00000"; // [23]
    static char* text; // destroyed when program ends
    static bool first = true;
    static EntityId label_id;
    if (first) {
        text = (char *)malloc(sizeof(template_string) / sizeof(char));
        label_id = create_label(text, (Position){15, 15, 0});
        strcpy(text, template_string);
        //set_label_text(label_id, text);
        set_label_color(label_id, color_black);
        first = false;
    }
    snprintf(text, (sizeof(template_string) / sizeof(char)), "△t: %4.0fms\t" "fps:%5.0f", delta * ms, fps);

    // Process velocity
    for (int e = 0; e < game_objects_count; e++)
        if (game_objects[e].velocity.x != 0 ||
            game_objects[e].velocity.y != 0 ||
            game_objects[e].velocity.z != 0)
            translate(e, vector_mul(game_objects[e].velocity, delta));

    // Process rotation
    for (int e = 0; e < game_objects_count; e++)
        // WARNING -> Rotação em múltiplos eixos não foi implementado
        if (game_objects[e].rotationVelocity.x != 0)
            rotate_x(e, game_objects[e].rotationVelocity.x * delta);
        else if (game_objects[e].rotationVelocity.y != 0)
            rotate_y(e, game_objects[e].rotationVelocity.y * delta);
        else
            rotate_z(e, game_objects[e].rotationVelocity.z * delta);
}

void system_input(InputEvent input_event)
{
    for (int e = 0; e < game_objects_count; e++)
        if (game_objects[e].input != nullptr)
            game_objects[e].input(input_event);
}
