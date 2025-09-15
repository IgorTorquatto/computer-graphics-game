#include <math.h> // para sin, cos

#include <algorithm>
#include "component.h"
#include "../utils/print.h"
#include "entity.h"
#include "assert.h"

// === Entity Management ===
// Se a ordem for importante, e houver remoção, usar uma técnica de memory pool.
size_t game_objects_count = 0;
size_t meshes_count = 0;
size_t spheres_count = 0;
size_t cuboids_count = 0;
size_t cubes_count = 0;
size_t toruses_count = 0;
size_t polygons_count = 0;
size_t labels_count = 0;


EntityId create_game_object()
{
    EntityId id = game_objects_count;
    if (++game_objects_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    return id; // Empty geometry
}

EntityId create_label(char *text, Position p)
{
    EntityId id = labels_count;
    if (++labels_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    labels[id] = {text, p};
	return id;
}

// Remoção de entidades não é recomendado nesse sistema.
// A ordem dos elementos é alterada na remoção.
void remove_game_object(EntityId id)
{
    assert(id < game_objects_count);
    print_warning("Remoção de entidades não é recomendado nesse sistema.\n"
        "\tA ordem dos elementos é alterada na remoção.");

    GeometryId geometry_id = game_objects[id].geometry.id;
    GeometryType type = game_objects[id].geometry.type;

    switch (type)
    {
    case GeometryType::MESH: {
        std::swap(meshes[id], meshes[--meshes_count]); // troca com o ultimo e ignora o final
        delete meshes[geometry_id].polygon.vertices; // Clear memory
        meshes[geometry_id] = Mesh{}; // Reset
    } break;

    case GeometryType::SPHERE:
        std::swap(spheres[id], spheres[--spheres_count]);
        spheres[geometry_id] = Sphere{};
        break;

    case GeometryType::CUBOID:
        std::swap(cuboids[id], cuboids[--cuboids_count]);
        cuboids[geometry_id] = Cuboid{};
        break;

    case GeometryType::CUBE:
        std::swap(cubes[id], cubes[--cubes_count]);
        cubes[geometry_id] = Cube{};
        break;

    case GeometryType::TORUS:
        std::swap(toruses[id], toruses[--toruses_count]);
        toruses[geometry_id] = Torus{};
        break;

    default:
        break;
    }

    std::swap(game_objects[id], game_objects[--game_objects_count]);
    game_objects[geometry_id] = GameObject{}; // Reset
}

void remove_label(EntityId id)
{
    assert(id < labels_count);
    print_warning("Remoção de entidades não é recomendado nesse sistema.\n"
        "\tA ordem dos elementos é alterada na remoção.");

    std::swap(labels[id], labels[--labels_count]); // troca com o ultimo e ignora o final
    labels[labels_count] = Label{}; // Reset
}

void set_label_text(EntityId id, char *text)
{
    labels[id].text = text;
}

void set_label_color(EntityId id, Color color)
{
    labels[id].color = color;
}


void set_position(EntityId id, Position p) {
    GeometryId geometry_id = game_objects[id].geometry.id;
    GeometryType type = game_objects[id].geometry.type;

    switch (type) {
    case GeometryType::MESH: {
        auto mid_point = [](Vertices vertices, size_t n) {
            Position mid = {0, 0, 0};
            for (Position *p = vertices; p < vertices + n; p++)
                mid = sum(mid, *p);
            mid = div(mid, n);
            return mid;
        };
        auto offset_local_positions = [](Vertices vertices, Position offset, size_t n) {
            for (Position *p = vertices; p < vertices + n; p++)
                *p = sum(*p, offset);
        };
        offset_local_positions(
            meshes[geometry_id].polygon.vertices,
            sub(p, mid_point(
                meshes[geometry_id].polygon.vertices,
                meshes[geometry_id].polygon.vertices_count)),
            meshes[geometry_id].polygon.vertices_count);
    } break;

    case GeometryType::SPHERE:
        spheres[geometry_id].center = p;
        break;

    case GeometryType::CUBOID:
        cuboids[geometry_id].center = p;
        break;

    case GeometryType::CUBE:
        cubes[geometry_id].center = p;
        break;

    case GeometryType::TORUS:
        toruses[geometry_id].center = p;
        break;

    default:
        break;
    }
}

void translate(EntityId id, Position p)
{
    auto t_mat = Transform{
        Position{1, 0, 0},
        Position{0, 1, 0},
        Position{0, 0, 1},
        p
    };
    Transform *model_p = &game_objects[id].geometry.model;
    *model_p = transform(model_p, &t_mat);
}

void rotate_x(EntityId id, Angle theta)
{
    auto rotate = [](Transform* t, Angle theta) {
        float c = cosf(theta);
        float s = sinf(theta);

        // matriz de rotação em X
        Transform r = {
            Vector{1, 0, 0},  // x_axis permanece
            Vector{0, c, s}, // y_axis rotaciona
            Vector{0, -s, c},  // z_axis rotaciona
            Position{0, 0, 0}
        };

        return transform(t, &r);
    };
    Transform *model_p = &game_objects[id].geometry.model;
    *model_p = rotate(model_p, theta);
}


void rotate_y(EntityId id, Angle theta)
{
	auto rotate = [](Transform *t, Angle theta) {
        float c = cosf(theta);
        float s = sinf(theta);

        Transform r = {
            Vector{c, 0, -s}, // x_axis rotaciona
            Vector{0, 1, 0},  // y_axis permanece
            Vector{s, 0, c},  // z_axis rotaciona
            Position{0, 0, 0}
        };

	    return transform(t, &r);
    };
    Transform *model_p = &game_objects[id].geometry.model;
    *model_p = rotate(model_p, theta);
}


void rotate_z(EntityId id, Angle theta)
{
    auto rotate = [](Transform* t, Angle theta) {
        float c = cosf(theta);
        float s = sinf(theta);

        Transform r = {
            Vector{c, s, 0},  // x_axis rotaciona
            Vector{-s, c, 0}, // y_axis rotaciona
            Vector{0, 0, 1},  // z_axis permanece
            Position{0, 0, 0}
        };

        return transform(t, &r);
    };
    Transform *model_p = &game_objects[id].geometry.model;
    *model_p = rotate(model_p, theta);
}


void set_color(EntityId id, Color color) {
    GeometryId geometry_id = game_objects[id].geometry.id;
    GeometryType type = game_objects[id].geometry.type;

    // WATCH -> Use color as an attribute of Geometry instead?
    // In the future different types of geometry could draw color with variations
    switch (type)
    {
    case GeometryType::MESH:
        meshes[geometry_id].color = color;
        break;

    case GeometryType::SPHERE:
        spheres[geometry_id].color = color;
        break;

    case GeometryType::CUBOID:
        cuboids[geometry_id].color = color;
        break;

    case GeometryType::CUBE:
        cubes[geometry_id].color = color;
        break;

    case GeometryType::TORUS:
        toruses[geometry_id].color = color;
        break;

    default:
        break;
    }
}

void set_velocity(EntityId id, Velocity velocity)
{
    game_objects[id].velocity = velocity;
}

void set_rotation_velocity(EntityId id, Velocity Velocity)
{
    game_objects[id].rotationVelocity = Velocity;
}

EntityId add_mesh(EntityId id)
{
    GeometryId mesh_id = meshes_count;

    game_objects[id].geometry.type = GeometryType::MESH;
    game_objects[id].geometry.id = mesh_id;

    if (++meshes_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço

    return mesh_id; // Using default values for the mesh
}

GeometryId add_sphere(EntityId id, float radius, Position p)
{
    GeometryId sphere_id = spheres_count;

    game_objects[id].geometry.type = GeometryType::SPHERE;
    game_objects[id].geometry.id = sphere_id;

    if (++spheres_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    spheres[sphere_id].radius = radius;
    spheres[sphere_id].center = p;
    return sphere_id;
}

GeometryId add_cuboid(EntityId id, Position p)
{
    GeometryId cuboid_id = cuboids_count;

    game_objects[id].geometry.type = GeometryType::CUBOID;
    game_objects[id].geometry.id = cuboid_id;

    if (++cuboids_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    cuboids[cuboid_id].center = p;
    return cuboid_id;
}

GeometryId add_cube(EntityId id, Position p, Vector d)
{
    GeometryId cube_id = cubes_count;

    game_objects[id].geometry.type = GeometryType::CUBE;
    game_objects[id].geometry.id = cube_id;

    if (++cubes_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    cubes[cube_id].center = p;
    cubes[cube_id].dimensions = d;
    return cube_id;
}

GeometryId add_torus(EntityId id, float innerRadius, float outerRadius, Position p)
{
    GeometryId torus_id = toruses_count;

    game_objects[id].geometry.type = GeometryType::TORUS;
    game_objects[id].geometry.id = torus_id;

    if (++toruses_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    toruses[torus_id].innerRadius = innerRadius;
    toruses[torus_id].outerRadius = outerRadius;
    toruses[torus_id].center = p;
    return torus_id;
}

void add_polygon(GeometryId id, Position *vertices, size_t count) {
    meshes[id].polygon.vertices_count = count;

    delete[] meshes[id].polygon.vertices; // limpa a memória antiga
    meshes[id].polygon.vertices = vertices;

    // setter method
    //for (size_t i = 0; i < count; i++)
    //    meshes[id].polygon.vertices[i] = {vertices[i].x, vertices[i].y};
}

void bind_process_function(EntityId id, ProcessFunction f)
{
    game_objects[id].process = f;
}

void bind_input_function(EntityId id, InputFunction f)
{
    game_objects[id].input = f;
}
