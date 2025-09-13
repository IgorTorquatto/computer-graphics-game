#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include "entity.h"
#include "component.h"
#include "system.h"

#include <iostream>

void render_system()
{
    for (int e = 0; e < meshes_count; e++) {
        Mesh mesh = static_meshes[e];
        Color color = mesh.color;
        Transform model = mesh.model;
        C_Polygon polygon = mesh.polygon;

        // loads mesh color
        glColor3f(color.r, color.g, color.b);


        glPushMatrix();
            // loads model matrix
            //glMatrixMode(GL_MODELVIEW);
            //glLoadIdentity(); // reset
            float m[16] = {
                // converts Transform to glMatrix
                model.x_axis.x, model.x_axis.y, model.x_axis.z, 0.0f,
                model.y_axis.x, model.y_axis.y, model.y_axis.z, 0.0f,
                model.z_axis.x, model.z_axis.y, model.z_axis.z, 0.0f,
                model.origin.x, model.origin.y, model.origin.z, 1.0f
            };
            //glLoadMatrixf(m);
            glMultMatrixf(m); // multiplica pela matriz do objeto

            glBegin(GL_TRIANGLES);
                // loads polygon vertices
                for (size_t i = 0; i < polygon.vertex_count; ++i) {
                    Position position = polygon.vertices[i];
                    glVertex3f(position.x, position.y, position.z);
                }
            glEnd();
        glPopMatrix();
    }
}


void process_system(DeltaTime delta)
{
}
