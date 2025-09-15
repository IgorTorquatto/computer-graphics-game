#include <math.h> // para sin, cos

#include "component.h"
#include "../utils/print.h"
#include "entity.h"

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
    EntityId id = meshes_count;
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
void remove_entity(EntityId id)
{
    print_warning("Remoção de entidades não é recomendado nesse sistema.\n"
        "\tA ordem dos elementos é alterada na remoção.");

    auto reset_mesh = [](Mesh* m) {
        m->model = Transform{
            Position{1, 0, 0},
            Position{0, 1, 0},
            Position{0, 0, 1},
            Position{0, 0, 0}
        };
        m->color = Color{1, 1, 1};
        delete[] m->polygon.vertices;
        m->polygon = C_Polygon{};
    };
    auto override_mesh = [](Mesh* from, Mesh* to) {
        to->model = from->model;
        to->color = from->color;
        to->polygon = from->polygon;
    };

    reset_mesh(&meshes[id]);
    override_mesh(&meshes[id], &meshes[--meshes_count]);
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
    set_origin(&meshes[id].model, p);
}

void translate(EntityId id, Position p) {

    EntityId geometry_id = game_objects[id].geometry.id;
    GeometryType type = game_objects[id].geometry.type;

    switch (type) {
    case GeometryType::SPHERE:
        spheres[geometry_id].center = sum(spheres[geometry_id].center, p);
        break;

    case GeometryType::CUBE:
        cubes[geometry_id].center = sum(cubes[geometry_id].center, p);
        break;

    default: {
        auto t_mat = Transform{
            Position{1, 0, 0},
            Position{0, 1, 0},
            Position{0, 0, 1},
            p
        };
        switch (type) {
        case GeometryType::MESH:
            meshes[geometry_id].model = transform(&meshes[geometry_id].model, &t_mat);
            break;

        case GeometryType::CUBOID:
            cuboids[geometry_id].model = transform(&cuboids[geometry_id].model, &t_mat);
            break;

        case GeometryType::TORUS:
            toruses[geometry_id].model = transform(&toruses[geometry_id].model, &t_mat);
            break;

        default:
            break;
        }
    } break;
    }
}

void rotate_x(EntityId id, Angle theta)
{
    auto rotate = [](Transform* t, Angle theta) {
        float c = cosf(theta);
        float s = sinf(theta);

        // matriz de rotação em X
        Transform r = {
            Vector{1, 0, 0},         // x_axis permanece
            Vector{0, c, -s},        // y_axis rotaciona
            Vector{0, s, c},         // z_axis rotaciona
            Position{0, 0, 0}          // origem é zero (aplicar depois)
        };

        return transform(t, &r);
    };

    GeometryType type = game_objects[id].geometry.type;
    EntityId geometry_id = game_objects[id].geometry.id;
    switch (game_objects[id].geometry.type) {
    case GeometryType::CUBOID:
        cuboids[geometry_id].model = rotate(&cuboids[geometry_id].model, theta);
        break;
    case GeometryType::TORUS:
        toruses[geometry_id].model = rotate(&toruses[geometry_id].model, theta);
        break;
    case GeometryType::MESH:
        meshes[geometry_id].model = rotate(&meshes[geometry_id].model, theta);
        break;
    default:
        break;
    }
}


void rotate_y(EntityId id, Angle theta)
{
	auto rotate = [](Transform *t, Angle theta) {
        float c = cosf(theta);
        float s = sinf(theta);

        Transform r = {Vector{c, 0, s}, // x_axis rotaciona
            Vector{0, 1, 0},            // y_axis permanece
            Vector{-s, 0, c},           // z_axis rotaciona
            Position{0, 0, 0}};

	    return transform(t, &r);
    };

    GeometryType type = game_objects[id].geometry.type;
    EntityId geometry_id = game_objects[id].geometry.id;
    switch (game_objects[id].geometry.type) {
    case GeometryType::CUBOID:
        cuboids[geometry_id].model = rotate(&cuboids[geometry_id].model, theta);
        break;
    case GeometryType::TORUS:
        toruses[geometry_id].model = rotate(&toruses[geometry_id].model, theta);
        break;
    case GeometryType::MESH:
        meshes[geometry_id].model = rotate(&meshes[geometry_id].model, theta);
        break;
    default:
        break;
    }
}


void rotate_z(EntityId id, Angle theta)
{
    auto rotate = [](Transform* t, Angle theta) {
        float c = cosf(theta);
        float s = sinf(theta);

        Transform r = {
            Vector{c, -s, 0},        // x_axis rotaciona
            Vector{s, c, 0},         // y_axis rotaciona
            Vector{0, 0, 1},         // z_axis permanece
            Position{0, 0, 0}
        };

        return transform(t, &r);
    };

    GeometryType type = game_objects[id].geometry.type;
    EntityId geometry_id = game_objects[id].geometry.id;
    switch (game_objects[id].geometry.type) {
    case GeometryType::CUBOID:
        cuboids[geometry_id].model = rotate(&cuboids[geometry_id].model, theta);
        break;
    case GeometryType::TORUS:
        toruses[geometry_id].model = rotate(&toruses[geometry_id].model, theta);
        break;
    case GeometryType::MESH:
        meshes[geometry_id].model = rotate(&meshes[geometry_id].model, theta);
        break;
    default:
        break;
    }
}

void set_mesh_color(EntityId id, Color color) {
    meshes[id].color = color;
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
    EntityId mesh_id = meshes_count;

    game_objects[id].geometry.type = GeometryType::MESH;
    game_objects[id].geometry.id = mesh_id;

    if (++meshes_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço

    return mesh_id; // Using default values for the mesh
}

EntityId add_sphere(EntityId id, float radius, Position p)
{
    EntityId sphere_id = spheres_count;

    game_objects[id].geometry.type = GeometryType::SPHERE;
    game_objects[id].geometry.id = sphere_id;

    if (++spheres_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    spheres[sphere_id].radius = radius;
    spheres[sphere_id].center = p;
    return sphere_id;
}

EntityId add_cuboid(EntityId id, Position p, Transform model)
{
    EntityId cuboid_id = cuboids_count;

    game_objects[id].geometry.type = GeometryType::CUBOID;
    game_objects[id].geometry.id = cuboid_id;

    if (++cuboids_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    cuboids[cuboid_id].center = p;
    cuboids[cuboid_id].model = model;
    return cuboid_id;
}

EntityId add_cube(EntityId id, Position p, Vector d)
{
    EntityId cube_id = cubes_count;

    game_objects[id].geometry.type = GeometryType::CUBE;
    game_objects[id].geometry.id = cube_id;

    if (++cubes_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    cubes[cube_id].center = p;
    cubes[cube_id].dimensions = d;
    return cube_id;
}

EntityId add_torus(EntityId id, float innerRadius, float outerRadius, Position p)
{
    EntityId torus_id = toruses_count;

    game_objects[id].geometry.type = GeometryType::TORUS;
    game_objects[id].geometry.id = torus_id;

    if (++toruses_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    toruses[torus_id].innerRadius = innerRadius;
    toruses[torus_id].outerRadius = outerRadius;
    toruses[torus_id].center = p;
    return torus_id;
}

void add_polygon(EntityId id, Position *vertices, size_t count) {
    meshes[id].polygon.vertex_count = count;

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
