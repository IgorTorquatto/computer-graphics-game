#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include "entity.h"
#include "component.h"
#include "system.h"


void render_system()
{
    for (int e = 0; e < MAX_ENTITIES; e++) {
        if (entityAlive[e]) {
            auto [color, model, polygon] = meshes[e];

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
                    for (size_t i = 0; i < polygon.vertex_count; i++) {
                        auto [x, y, z] = polygon.vertices[i];
                        glVertex3f(x, y, z);
                    }
                glEnd();
            glPopMatrix();
        }
    }
}


void process_system(DeltaTime delta)
{
}
