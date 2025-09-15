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


void render_system() {
	auto render_mesh = [](Mesh &mesh) {
        Color color = mesh.color;
        Transform model = mesh.model;
        C_Polygon polygon = mesh.polygon;

        // loads mesh color
        glColor3f(color.r, color.g, color.b);

        glPushMatrix();
            // loads model matrix
            // glMatrixMode(GL_MODELVIEW);
            // glLoadIdentity(); // reset
            float m[16] = {// converts Transform to glMatrix
                model.x_axis.x, model.x_axis.y, model.x_axis.z, 0.0f, model.y_axis.x,
                model.y_axis.y, model.y_axis.z, 0.0f, model.z_axis.x, model.z_axis.y,
                model.z_axis.z, 0.0f, model.origin.x, model.origin.y, model.origin.z, 1.0f};
            // glLoadMatrixf(m);
            glMultMatrixf(m); // multiplica pela matriz do objeto

            glBegin(GL_TRIANGLES);
                // loads polygon vertices
                for (size_t i = 0; i < polygon.vertex_count; ++i) {
                    Position position = polygon.vertices[i];
                    glVertex3f(position.x, position.y, position.z);
                }
            glEnd();
        glPopMatrix();
    };
    auto render_sphere = [](Sphere &sphere) {
        Color color = sphere.color;
        Position position = sphere.center;

        glColor3f(color.r, color.g, color.b);
        glPushMatrix();
            glTranslatef(position.x, position.y, position.z);
            if (sphere.isSolid)
                glutSolidSphere(sphere.radius, sphere.slices, sphere.stacks);
            else
                glutWireSphere(sphere.radius, sphere.slices, sphere.stacks);
        glPopMatrix();
    };
    auto render_cuboid = [](Cuboid &cuboid) {
        Color color = cuboid.color;
        Position position = cuboid.center;

        glColor3f(color.r, color.g, color.b);
        glPushMatrix();
            glLoadMatrixf(&cuboid.model.columns[0].x);
            glTranslatef(position.x, position.y, position.z);
            if (cuboid.isSolid)
                glutSolidCube(1);
            else
                glutWireCube(1);
        glPopMatrix();
    };
    auto render_cube = [](Cube &cube) {
        Color color = cube.color;
        Vector dimensions = cube.dimensions;
        Position position = cube.center;

        glColor3f(color.r, color.g, color.b);
        glPushMatrix();
            glScalef(dimensions.x, dimensions.y, dimensions.z);
            glTranslated(position.x, position.y, position.z);
            if (cube.isSolid)
                glutSolidCube(1);
            else
                glutWireCube(1);
        glPopMatrix();
    };
    auto render_torus = [](Torus &torus) {
        Color color = torus.color;
        Position position = torus.center;
        float outer_radius = torus.outerRadius;
        float inner_radius = torus.innerRadius;

        glColor3f(color.r, color.g, color.b);

        glPushMatrix();
            glLoadMatrixf(&torus.model.columns[0].x);
            glTranslatef(position.x, position.y, position.z);
            if (torus.isSolid)
                glutSolidTorus(inner_radius, outer_radius, torus.slices, torus.stacks);
            else
                glutWireTorus(inner_radius, outer_radius, torus.slices, torus.stacks);
        glPopMatrix();

    };
    auto render_geometry = [&](Geometry geometry) {
        switch (geometry.type) {
            case GeometryType::MESH:
                render_mesh(meshes[geometry.id]);
                break;
            case GeometryType::SPHERE:
                render_sphere(spheres[geometry.id]);
                break;
            case GeometryType::CUBOID:
                render_cuboid(cuboids[geometry.id]);
                break;
            case GeometryType::CUBE:
                render_cube(cubes[geometry.id]);
                break;
            case GeometryType::TORUS:
                render_torus(toruses[geometry.id]);
                break;
            default:
                break;
        }
    };
    auto render_text = [](Label label) {
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
    };

    for (EntityId id = 0; id < game_objects_count; ++id)
        render_geometry(game_objects[id].geometry);

    for (EntityId id = 0; id < labels_count; ++id)
        render_text(labels[id]);
}


void process_system(DeltaTime delta)
{
    for (int e = 0; e < game_objects_count; e++)
        if (game_objects[e].process != nullptr)
            game_objects[e].process(delta);

    auto multiply = [](Vector a, float scalar){
        return Vector{a.x * scalar, a.y * scalar, a.z * scalar};
    };

    // Fill text buffer with approximate delta time
    // Assumes delta is always less than 10sec
    char template_string[] = "△t: 0000ms\t" "fps:00000"; // [23]
    static char* text = new char[sizeof(template_string) / sizeof(char)]; // destroyed when program ends
    static bool first = true;
    static EntityId label_id = create_label(text, {15, 15, 0});
    if (first) {
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
            translate(e, multiply(game_objects[e].velocity, delta));

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

void input_system(InputEvent input_event)
{
    for (int e = 0; e < game_objects_count; e++)
        if (game_objects[e].input != nullptr)
            game_objects[e].input(input_event);
}
