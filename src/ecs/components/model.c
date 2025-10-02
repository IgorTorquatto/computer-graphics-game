#pragma region STL
    #include <ctype.h>
    #include <string.h>
#pragma endregion
#pragma region Libs
    #include <GL/freeglut.h>
    #include <GL/gl.h>

    #define STB_IMAGE_IMPLEMENTATION
    #include <stb_image.h>
#pragma endregion
#pragma region Local Includes
    #include "utils/file_system.h"
    #include "utils/print.h"
#pragma endregion

#include "model.h"

#define NOT_FOUND -1
#define DEFAULT_CAPACITY 1024
#define MAX_MAT_NAME 256


/*--- Funções estáticas auxiliares ---*/
#pragma region Local Functions
/**
 * Conta elementos de um modelo (vértices e faces)
 * a partir de um arquivo OBJ simples.
 *
 * @param file ponteiro para o arquivo OBJ a ser carregado
 * @param outVertCount ponteiro para o quantidade de vértices do modelo
 * @param outFaceCount ponteiro para a quantidade de faces do modelo
 */
static void countElements(FILE* file, int* outVertCount, int* outFaceCount)
{
    char line[256];
    *outVertCount = 0;
    *outFaceCount = 0;

    while(fgets(line, sizeof(line), file)) {
        if(strncmp(line, "v ", 2) == 0)
            (*outVertCount)++;
        else if(strncmp(line, "file ", 2) == 0)
            (*outFaceCount)++;
    }
}


/**
 * Para todas funções ensure_*_capacity abaixo:
 * @brief Realoca o buffer de uma lista para um
 * tamanho que atenda ao aumento de 'add' elementos.
 *
 * Se houver falha de realloc, fecha o arquivo 'file'
 * e sai do programa com exit(FAILURE = 1).
 *
 * @param add quantidade de elementos a serem adicionados ao buffer
 * @param file arquivo a ser fechado em caso de falha
 * @param list buffer a ser realocado
 * @param n_ptr tamanho atual do buffer
 * @param capacity capacidade atual do buffer
 * @param list_name nome do buffer para ser usado em mensagens de erro
 */

static void ensure_vec3_capacity(size_t add, FILE* file, Vec3** list_ptr,
        size_t n, size_t* capacity, const char* list_name)
{
    if (n + add > *capacity) {
        size_t new_capacity = (*capacity == 0) ? DEFAULT_CAPACITY : *capacity;
        while (n + add > new_capacity)
            new_capacity *= 2;

        Vec3* tmp = (Vec3*)realloc(*list_ptr, new_capacity * sizeof(Vec3));
        if (!tmp) {
            print_error("`realloc` failed for %s", list_name);
            if (file)
                fclose(file);
            exit(EXIT_FAILURE);
        }
        *list_ptr = tmp;
        *capacity = new_capacity;
    }
}

static void ensure_vec2_capacity(size_t add, FILE* file, Vec2** list_ptr,
        size_t n, size_t* capacity, const char* list_name)
{
    if (n + add > *capacity) {
        size_t new_capacity = (*capacity == 0) ? DEFAULT_CAPACITY : *capacity;
        while (n + add > new_capacity)
            new_capacity *= 2;

        Vec2* tmp = (Vec2*)realloc(*list_ptr, new_capacity * sizeof(Vec2));
        if (!tmp) {
            print_error("`realloc` failed for %s", list_name);
            if (file)
                fclose(file);
            exit(EXIT_FAILURE);
        }
        *list_ptr = tmp;
        *capacity = new_capacity;
    }
}

static void ensure_face_capacity(size_t add, FILE* file, Face** list_ptr,
        size_t n, size_t* capacity, const char* list_name) {
    if (n + add > *capacity) {
        size_t newcap = (*capacity == 0) ? DEFAULT_CAPACITY : *capacity;
        while (n + add > newcap)
            newcap *= 2;

        Face* tmp = (Face*)realloc(*list_ptr, newcap * sizeof(Face));
        if (!tmp) {
            print_error("`realloc` failed for %s", list_name);
            if (file)
                fclose(file);
            exit(EXIT_FAILURE);
        }
        *list_ptr = tmp;
        *capacity = newcap;
    }
}

static void ensure_material_capacity(size_t add, FILE* file, Material** list_ptr,
        size_t n, size_t* capacity, const char* list_name) {
    if (n + add > *capacity) {
        size_t newcap = (*capacity == 0) ? DEFAULT_CAPACITY : *capacity;
        while (n + add > newcap)
            newcap *= 2;

        Material* tmp = (Material*)realloc(*list_ptr, newcap * sizeof(Material));
        if (!tmp) {
            print_error("`realloc` failed for %s", list_name);
            if (file)
                fclose(file);
            exit(EXIT_FAILURE);
        }
        *list_ptr = tmp;
        *capacity = newcap;
    }
}

/**
 * Procura material por nome, retorna NOT_FOUND se não achar
 *
 * @param materials ponteiro para um array de materiais
 * @param n tamanho do array de materiais
 * @param name nome do material a ser procurado
 * @return índice do material no array, ou NOT_FOUND se não achar
 */
static int material_find(const char* name, Material* materials, size_t n) {
    if (!name)
        return NOT_FOUND;
    for (Material* m = materials; m < materials + n; ++m)
        if (strcmp(m->name, name) == 0)
            return (int)(m - materials);
    return NOT_FOUND;
}


/**
 * @brief Adiciona material (sem textura ainda).
 * @cond assume materials_ptr != NULL
 * 
 * @param name Nome do material.
 * @param file Ponteiro para o arquivo MTL.
 * @param materials_ptr Ponteiro para a estrutura de materiais.
 * @param n_ptr Ponteiro para o contador de materiais.
 * @param capacity Ponteiro para a capacidade da estrutura de materiais.
 * 
 * @return retorna índice (0-based).
 */
static int material_add(const char* name, FILE* file, Material** materials_ptr, size_t* n_ptr, size_t* capacity) {
    size_t n = *n_ptr;
    ensure_material_capacity(1, file, materials_ptr, n, capacity, "materials");
    (*materials_ptr)[n].texture_id = 0;
    (*materials_ptr)[n].name[0] = '\0';

    if (name)
        strncpy((*materials_ptr)[n].name, name, MAX_MAT_NAME-1);

    (*materials_ptr)[n].name[MAX_MAT_NAME-1] = '\0';
    *n_ptr = n + 1; /* incrementa o contador */

    return (int)n;  /* retorna índice válido */
}


/**
 * @brief Calcula a normal de uma face (unitária).
 * 
 * @param a Primeira aresta do triângulo.
 * @param b Segunda aresta do triângulo.
 * @param c Terceira aresta do triângulo.
 * @param out Normal calculada (unitária).
 *
 * A normal é calculada como o produto vetorial das arestas do triângulo.
 * Se a normal for zero (ou seja muito pequena), a saída será (0,0,0).
 */
static void compute_face_normal(const Vec3 *a, const Vec3 *b, const Vec3 *c, Vec3 *out) {
    float ux = b->x - a->x, uy = b->y - a->y, uz = b->z - a->z;
    float vx = c->x - a->x, vy = c->y - a->y, vz = c->z - a->z;
    out->x = uy * vz - uz * vy;
    out->y = uz * vx - ux * vz;
    out->z = ux * vy - uy * vx;
    float len = sqrtf(out->x * out->x + out->y * out->y + out->z * out->z);
    if (len > 1e-9f) {
        out->x /= len; out->y /= len; out->z /= len;
    }
    else {
        out->x = out->y = out->z = 0.0f;
    }
}

/**
 * @brief Carrega um arquivo MTL [Material Template Library]
 * e resolve os caminhos relativos para texturas.
 * @cond Assume que os ponteiros são != NULL
 *
 * @param mtl_fullpath caminho absoluto do arquivo MTL.
 * @param materials estrutura que armazenará os materiais lidos do MTL.
 * @param n_ptr quantidade de materiais a serem lidos.
 * @param capacity ponteiro para a capacidade atual da estrutura materials.
 *
 * @note O MTL pode conter caminhos relativos para texturas,
 * que devem ser resolvidos com base no diretório do próprio MTL.
 * 
 * @note A função assume que a estrutura materials tenha espaço suficiente para armazenar
 * os n materiais lidos. Caso contrário, a função não fará nada e retornará sem
 * alterar a estrutura.
 */
static void load_mtl(const char *mtl_fullpath, Material **materials_ptr, size_t* n_ptr, size_t* capacity) {
    assert(mtl_fullpath != NULL);
    assert(materials_ptr != NULL);
    assert(n_ptr != NULL);
    assert(capacity != NULL);

    FILE *file = fopen(mtl_fullpath, "r");
    if (!file) {
        print_error("Erro abrindo MTL: %s", mtl_fullpath);
        return;
    }

    /* Extrai diretório do mtl_fullpath (para resolver paths relativos) */
    char base_dir[DEFAULT_CAPACITY] = {0};
    directory_base(mtl_fullpath, base_dir, sizeof(base_dir));

    const char sep[2] = { (strchr(mtl_fullpath, '\\') ? '\\' : '/'), '\0' };
    char line[DEFAULT_CAPACITY];
    char token[MAX_MAT_NAME];
    int current_material = -1;
    size_t ignored_n = 0;

    while (fgets(line, sizeof(line), file)) {
        #pragma region New Material
        if (sscanf(line, "newmtl %511s", token) == 1) {
            int idx = material_find(token, *materials_ptr, *n_ptr);
            if (idx < 0)
                idx = material_add(token, file, materials_ptr, n_ptr, capacity);
            current_material = idx;

            // assegura texture_id=0 por padrão
            (*materials_ptr)[current_material].texture_id = 0;
            continue;
        }
        #pragma endregion

        #pragma region Diffuse Texture
        // procurar por "map_Kd" (padrão para textura difusa)
        if (strstr(line, "map_Kd") != NULL) {
            // 1. obter o resto da linha após "map_Kd"
            #pragma region Texture Name
                char tex_name_raw[MAX_MAT_NAME] = {0};
                // pega tudo após "map_Kd"
                const char *after = strstr(line, "map_Kd") + strlen("map_Kd");

                while (*after && isspace((unsigned char)*after))
                    ++after;

                // copia a parte restante até o final da linha
                strncpy(tex_name_raw, after, sizeof(tex_name_raw) - 1);
                trim_and_strip_quotes(tex_name_raw);
            #pragma endregion

            // 2. Resolver caminho: construir possíveis full paths para tentar:
            #pragma region Candidate Paths
                char candidate[DEFAULT_CAPACITY] = {0};
                char name_only[MAX_MAT_NAME] = {0};

                // 1) se tex_name_raw já for absoluto, use direto
                if (path_is_absolute(tex_name_raw)) {
                    snprintf(candidate, sizeof(candidate), "%s", tex_name_raw);
                } else {
                    path_join(base_dir, sep, tex_name_raw, candidate, sizeof(candidate));
                }// normalize separators in tex_name_raw a bit
                char tex_tmp[512];
                strncpy(tex_tmp, tex_name_raw, sizeof(tex_tmp)-1); tex_tmp[sizeof(tex_tmp)-1]=0;
                trim_and_strip_quotes(tex_tmp);

                // extrai apenas o basename (nome do ficheiro) para fallbacks
                const char *bs1 = strrchr(tex_tmp, '/');
                const char *bs2 = strrchr(tex_tmp, '\\');
                const char *bs = (bs1 || bs2) ? ((bs1 && bs2) ? ((bs1>bs2)?bs1:bs2) : (bs1?bs1:bs2)) : NULL;
                if (bs) strncpy(name_only, bs+1, sizeof(name_only)-1);
                else strncpy(name_only, tex_tmp, sizeof(name_only)-1);

                if (path_is_absolute(tex_tmp)) {
                    // 1) Se tex_name_raw é absoluto -> tente usar direto
                    snprintf(candidate, sizeof(candidate), "%s", tex_tmp);
                    // se não existir, tente procurar o basename ascendendo a partir de base_dir
                    if (access(candidate, F_OK) != 0) {
                        if (find_file_in_ancestors(base_dir, name_only, candidate, sizeof(candidate), 6)) {
                            print_warning("[MTL] absolute path not found; located '%s' by basename in ancestors -> %s", tex_tmp, candidate);
                        } else {
                            // fallback: use name-only (arquivo esperado no cwd)
                            snprintf(candidate, sizeof(candidate), "%s", name_only);
                        }
                    }
                } else {
                    // 2) caminho relativo ao diretório do MTL: resolve em relação ao base_dir
                    path_join(base_dir, sep, tex_tmp, candidate, sizeof(candidate));
                    // se não existir, tente procurar o basename ascendendo
                    if (access(candidate, F_OK) != 0 && find_file_in_ancestors(base_dir, name_only, candidate, sizeof(candidate), 6)) {
                        print_warning("[MTL] relative not found at resolved path; located '%s' by basename in ancestors -> %s", tex_tmp, candidate);
                    }
                }
            #pragma endregion

            // 2.1 Como fallback, tente apenas o nome no cwd (tex_name_raw)
            // Tentaremos abrir candidate, se falhar vamos tentar tex_name_raw
            #pragma region Fallback
                print_info("[MTL] tentando carregar textura: '%s' (resolved: '%s')", tex_name_raw, candidate);

                // testar existência com fopen antes de stbi
                FILE *tf = fopen(candidate, "rb");
                if (!tf) {
                    // tentar fallback: nome simples (sem base_dir)
                    tf = fopen(tex_name_raw, "rb");
                    if (!tf) {
                        print_error("[MTL] Falha ao localizar textura. Tentadas: '%s' e '%s'", candidate, tex_name_raw);
                        // pode continuar procurando outras linhas dentro do .mtl
                        continue;
                    } else
                        fclose(tf);
                } else {
                    fclose(tf);
                }
            #pragma endregion

            // 3. Carregar imagem com stb_image
            #pragma region Load Image
                int width, height, channels;
                unsigned char *data = stbi_load(candidate, &width, &height, &channels, 4);
                if (!data) {
                    print_error("[MTL] Erro carregando textura com stb_image: %s\n\t"
                                "stb_image failure reason: %s", candidate, stbi_failure_reason());
                    continue;
                }
            #pragma endregion

            // 4. Criar textura OpenGL (exemplo mínimo)
            #pragma region Create Texture
                GLuint texture_id;
                glGenTextures(1, &texture_id);
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);

                print_success("[MTL] textura carregada com sucesso: %s "
                        " -> texture_id=%u (width=%d height=%d)\n",
                        candidate, (unsigned)texture_id, width, height);
            #pragma endregion

            if (current_material < 0) {
                // sem newmtl declarado antes: crie material genérico
                current_material = material_add("generic", file, materials_ptr, n_ptr, capacity);
            }
            // Associa a textura ao material atual
            (*materials_ptr)[current_material].texture_id = texture_id;
            continue;
        }
        #pragma endregion

        ++ignored_n;
        // ignorar outras linhas
    }
    if (ignored_n > 0)
        print_warning("[MTL] %d linhas ignoradas", ignored_n);

    fclose(file);
}
#pragma endregion

bool load_obj(const char *filename, Model* model)
{
    // capacidades internas (mantém estado entre chamadas)
    size_t vertex_cap = 0, tex_coords_cap = 0, normal_cap = 0, face_cap = 0, material_cap = 0;
    *model = model_empty;

    // Calcule o diretório base do arquivo .obj (por ex. "Tree" para "Tree/Tree.obj")
    char base_dir[1024];
    directory_base(filename, base_dir, sizeof(base_dir));

    // determinar separador a usar (prefere o separador encontrado em filename)
    const char sep[2] = { (strchr(filename, '\\') ? '\\' : '/'), '\0' };

    FILE *file = fopen(filename, "r");
    if (!file) {
        print_error("Erro abrindo OBJ %s.", filename);
        return false;
    }

    char line[DEFAULT_CAPACITY];
    size_t ignored_count = 0;
    int current_material = -1; // índice do material atual nas faces; default -1

    enum { NONE=0, VERTICES=1, TEXTURE_VERTICES=2, MATERIAL_LIBS=4,
           MATERIALS=8, NORMALS=16, FACES=32 };
    unsigned unsupported_format = NONE;

#pragma region Parse OBJ
    // parsing robusto (suporta v, vt, vn, f, mtllib, usemtl)
    while (fgets(line, sizeof(line), file)) {

        if (line[0] == 'v' && line[1] == ' ') { // model->vertices
            Vec3 v;
            if (sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z) == 3) {
                ensure_vec3_capacity(1, file, &model->vertices,
                        model->vertices_count, &vertex_cap, "vertices");
                model->vertices[model->vertices_count++] = v;

                // Calcular dimensões mínimas e máximas
                if (v.x < model->min.x) model->min.x = v.x;
                if (v.y < model->min.y) model->min.y = v.y;
                if (v.z < model->min.z) model->min.z = v.z;
                if (v.x > model->max.x) model->max.x = v.x;
                if (v.y > model->max.y) model->max.y = v.y;
                if (v.z > model->max.z) model->max.z = v.z;
            }
            else
                unsupported_format |= VERTICES;

        } else if (line[0] == 'v' && line[1] == 't') { // Texture Coordinates
            Vec2 t;
            /* vt u v  (v pode faltar em alguns OBJs) */
            if (sscanf(line, "vt %f %f", &t.u, &t.v) >= 1) {
                if (sscanf(line, "vt %f %f", &t.u, &t.v) != 2)
                    t.v = 0.0f;

                ensure_vec2_capacity(
                        1, file, &model->texture_coords, model->texture_coords_count,
                        &tex_coords_cap, "texture_coords");
                model->texture_coords[model->texture_coords_count++] = t;
            }
            else
                unsupported_format |= TEXTURE_VERTICES;

        } else if (strncmp(line, "mtllib", 6) == 0) { // Material Template Library
            char mtl_file[512] = {0};
            if (sscanf(line, "mtllib %511s", mtl_file) == 1) {
                char full_mtl[DEFAULT_CAPACITY];
                // se o mtl_file já for um caminho absoluto, join_path deixa como está
                path_join(base_dir, sep, mtl_file, full_mtl, sizeof(full_mtl));
                // isso pode adicionar materiais
                load_mtl(full_mtl, &model->materials, &model->materials_count, &material_cap);
            }
            else
                unsupported_format |= MATERIAL_LIBS;

        } else if (strncmp(line, "usemtl", 6) == 0) {
            char material_name[512] = {0};
            if (sscanf(line, "usemtl %511s", material_name) == 1) {
                int idx = material_find(material_name, model->materials, model->materials_count);
                if (idx < 0)
                    // material referenciado mas não definido no .mtl -> criar placeholder
                    idx = material_add(material_name, file, &model->materials,
                                       &model->materials_count, &material_cap);
                current_material = idx;
                print_info("[OBJ] usemtl -> %s (idx=%d)", material_name, current_material);
            }
            else
                unsupported_format |= MATERIALS;

        } else if (line[0] == 'f' && (line[1] == ' ' || line[1] == '\t')) { // faces
            // parse robusto (suporta v/vt/vn, v//vn, v/vt, v v v)
            Face face;
            face.material_id = current_material; // atribui o material atual
            int vertex[3], vertex_texture[3], vertex_normal[3];

        #pragma region Try v/vt/vn
            int matches = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                                &vertex[0], &vertex_texture[0], &vertex_normal[0],
                                &vertex[1], &vertex_texture[1], &vertex_normal[1],
                                &vertex[2], &vertex_texture[2], &vertex_normal[2]);
            if (matches == 9) {
                for (int j = 0; j < 3; ++j) {
                    face.vertex[j] = vertex[j];
                    face.vertex_texture[j] = vertex_texture[j];
                    face.vertex_normal[j] = vertex_normal[j];
                }
                ensure_face_capacity(1, file, &model->faces, model->faces_count, &face_cap, "faces");
                model->faces[model->faces_count++] = face;
                continue;
            }
        #pragma endregion

        #pragma region Try v//vn
            matches = sscanf(line, "f %d//%d %d//%d %d//%d",
                            &vertex[0], &vertex_normal[0],
                            &vertex[1], &vertex_normal[1],
                            &vertex[2], &vertex_normal[2]);
            if (matches == 6) {
                for (int j = 0; j < 3; ++j) {
                    face.vertex[j]  = vertex[j];
                    face.vertex_texture[j] = vertex_texture[j];
                    face.vertex_normal[j] = vertex_normal[j];
                }
                ensure_face_capacity(1, file, &model->faces, model->faces_count, &face_cap, "faces");
                model->faces[model->faces_count++] = face;
                continue;
            }
        #pragma endregion

        #pragma region Try v/vt
            matches = sscanf(line, "f %d/%d %d/%d %d/%d",
                            &vertex[0], &vertex_texture[0],
                            &vertex[1], &vertex_texture[1],
                            &vertex[2], &vertex_texture[2]);
            if (matches == 6) {
                for (int j = 0; j < 3; j++) {
                    face.vertex[j]  = vertex[j];
                    face.vertex_texture[j] = vertex_texture[j];
                    face.vertex_normal[j] = 0;
                }
                ensure_face_capacity(1, file, &model->faces, model->faces_count, &face_cap, "faces");
                model->faces[model->faces_count++] = face;
                continue;
            }
        #pragma endregion

        #pragma region Try v v v
            matches = sscanf(line, "f %d %d %d", &vertex[0], &vertex[1], &vertex[2]);
            if (matches == 3) {
                for (int j = 0; j < 3; ++j) {
                    face.vertex[j]  = vertex[j];
                    face.vertex_normal[j] = face.vertex_texture[j] = 0;
                }
                ensure_face_capacity(1, file, &model->faces, model->faces_count, &face_cap, "faces");
                model->faces[model->faces_count++] = face;
                continue;
            }

        #pragma endregion

            // formato não suportado (ngon etc.) -> ignorar
            unsupported_format |= FACES;

        } else if (line[0]=='v' && line[1]=='n') {
            Vec3 n;
            if (sscanf(line, "vn %f %f %f", &n.x, &n.y, &n.z) == 3) {
                ensure_vec3_capacity(1, file, &model->normals,
                        model->normals_count, &normal_cap, "normals");
                model->normals[model->normals_count++] = n;
            }
            else
                unsupported_format |= NORMALS;

        }
        else if (line[0] == '#' || line[0] == '\n') {
            continue; // ignorar comentários e linhas vazias
        }
        else {
            /* outras linhas (o, s, etc.) ignoradas aqui */
            if (!isblank(*line) && line[0] != EOF)
                ++ignored_count;
        }
    }
#pragma endregion

#pragma region Warn skiked lines
    if (unsupported_format != NONE) {
        print_warning("Formato de arquivo desconhecido na leitura das faces:%s%s%s%s%s%s",
                    unsupported_format & VERTICES ? " v" : "",
                    unsupported_format & TEXTURE_VERTICES ? " vt" : "",
                    unsupported_format & MATERIAL_LIBS ? " mtllib" : "",
                    unsupported_format & MATERIALS ? " usemtl" : "",
                    unsupported_format & NORMALS ? " vn" : "",
                    unsupported_format & FACES ? " f" : "");
    }
    if (ignored_count)
        print_warning("%zu linhas ignoradas\n", ignored_count);

    fclose(file);
#pragma endregion

#pragma region Patch Missing Normals
    for (size_t i = 0; i < model->faces_count; ++i) { // percorre faces
        bool any_invalid = false;
        for (int j = 0; j < 3; ++j) {
            int vn_idx = model->faces[i].vertex_normal[j]; // seu código usa 1-based
            if (vn_idx <= 0 || vn_idx > (int)model->normals_count) {
                any_invalid = true;
                break;
            }
        }

        if (!any_invalid)
            continue; // face já tem todas as normals válidas

        // calcula normal da face (a, b, c são posições)
        Vec3 a = model->vertices[ model->faces[i].vertex[0] - 1 ];
        Vec3 b = model->vertices[ model->faces[i].vertex[1] - 1 ];
        Vec3 c = model->vertices[ model->faces[i].vertex[2] - 1 ];
        Vec3 fn;
        compute_face_normal(&a, &b, &c, &fn);

        // anexa normal à lista de normals e obtenha índice 1-based
        ensure_vec3_capacity(1, NULL, &model->normals, model->normals_count, &normal_cap, "normals");
        model->normals[model->normals_count] = fn;
        model->normals_count++;
        int new_normal_index = (int)model->normals_count; // 1-based index para OBJ-style

        // preencha apenas as posições de vertex_normal inválidas com esse índice
        for (int j = 0; j < 3; ++j) {
            int vn_idx = model->faces[i].vertex_normal[j];
            if (vn_idx <= 0 || vn_idx > (int)model->normals_count-1) {
                model->faces[i].vertex_normal[j] = new_normal_index;
            }
        }
    }
#pragma endregion

    /* Opcional: reduzir (shrink) as alocações para o tamanho real */
#pragma region Shrink allocations
    if (vertex_cap > model->vertices_count) {
        Vec3* tmp = (Vec3*)realloc(model->vertices,
                (model->vertices_count ? model->vertices_count : 1) * sizeof(Vec3));
        if (tmp) {
            model->vertices = tmp;
            vertex_cap = model->vertices_count;
        }
    }
    if (tex_coords_cap > model->texture_coords_count) {
        Vec2* tmp = (Vec2*)realloc(model->texture_coords,
                (model->texture_coords_count ? model->texture_coords_count : 1) * sizeof(Vec2));
        if (tmp) {
            model->texture_coords = tmp;
            tex_coords_cap = model->texture_coords_count;
        }
    }
    if (normal_cap > model->normals_count) {
        Vec3* tmp = (Vec3*)realloc(model->normals,
                (model->normals_count ? model->normals_count : 1) * sizeof(Vec3));
        if (tmp) {
            model->normals = tmp;
            normal_cap = model->normals_count;
        }
    }
    if (face_cap > model->faces_count) {
        Face* tmp = (Face*)realloc(model->faces,
                (model->faces_count ? model->faces_count : 1) * sizeof(Face));
        if (tmp) {
            model->faces = tmp;
            face_cap = model->faces_count;
        }
    }
#pragma endregion

    return true;
}

void draw_model(const Model* model)
{
    size_t vertices_count = model->vertices_count;
    size_t normals_count = model->normals_count;
    size_t texture_coords_count = model->texture_coords_count;
    size_t materials_count = model->materials_count;
    size_t faces_count = model->faces_count;
    Vec3* vertices = model->vertices;
    Vec3* normals = model->normals;
    Vec2* texture_coords = model->texture_coords;
    Material* materials = model->materials;
    Face* faces = model->faces;

    glColor3f(1, 1, 1); // garante que a textura não seja multiplicada por cor preta ou modulação

    // Bind da textura principal (use o certo por material)
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);

    int last_mat = -2; // inválido inicialmente
    for (Face* face = faces; face < faces + faces_count; ++face) {
        int material_id = face->material_id;
        // se mudou o material -> terminar batch e iniciar novo
        if (material_id != last_mat) {
            if (last_mat != -2) {
                glEnd(); // finaliza batch anterior
            }
            // iniciar novo batch
            glBindTexture(GL_TEXTURE_2D,
                    (material_id >= 0 && material_id < (int)materials_count)
                      ? materials[material_id].texture_id : 0);
            glBegin(GL_TRIANGLES);
            last_mat = material_id;
        }

        // desenha a face i (3 vértices)
        for (int j = 0; j < 3; j++) {
            // Texture Coordinates
            int vti = face->vertex_texture[j] - 1;
            if (vti >= 0 && vti < (int)texture_coords_count) {
                Vec2 t = texture_coords[vti];
                glTexCoord2f(t.u, t.v);
            } else {
                // fallback
                glTexCoord2f(0.0f, 0.0f);
            }

            // Normals
            int ni = face->vertex_normal[j] - 1;
            Vec3 n = normals[ni];
            glNormal3f(n.x, n.y, n.z);

            // Faces
            int vi = face->vertex[j] - 1; // .obj é 1-based
            Vec3 v = vertices[vi];
            glVertex3f(v.x, v.y, v.z);
        }
    }
    if (last_mat != -2) // fecha último batch
        glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_NORMALIZE);
}

void free_model(Model* model)
{
    // Se o ponteiro for NULL, free não faz nada
    free(model->vertices);
    free(model->normals);
    free(model->texture_coords);
    free(model->materials);
    free(model->faces);
    *model = model_empty;
}
