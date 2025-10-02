#pragma once

#include "GL/glut.h"

#include "../component.h"

#define MATERIAL_MAX_NAME 256

#pragma region Types
    typedef struct {
        char name[MATERIAL_MAX_NAME];
        GLuint texture_id; // 0 se sem textura
    } Material;

    typedef struct {
        int vertex[3]; // índices dos vértices, 3 por face (triângulos)
        int vertex_texture[3];
        int vertex_normal[3];
        int material_id; // índice do material para essa face (ou -1)
    } Face;

    typedef struct {
        size_t vertices_count, normals_count, texture_coords_count,
               materials_count, faces_count;
        Vec3 min, max;
        Vec3* vertices;
        Vec3* normals;
        Vec2* texture_coords;
        Material* materials;
        Face* faces;
        //int* faces; // FIXME
    } Model;
    #define model_empty (Model){0, 0, 0, 0, 0, (Vec3)vector_inf, (Vec3)(vector_neg_inf), NULL, NULL, NULL, NULL, NULL}
#pragma endregion

#pragma region Globals
    #pragma region Buffers
    #pragma endregion

/**
 * @brief Carrega um modelo 3D a partir de um arquivo .obj usando
 * realloc growth strategy_ e as funções auxiliares
 * @cond Assume que model está livre.
 * @attention Os ponteiros devem estar liberados para uso.
 * 
 * @param filename nome do arquivo OBJ a ser carregado
 * @param model estrutura para armazenar o modelo carregado
 * @return EXIT_ERROR, EXIT_SUCCESS
 * 
 * @note Carrega um modelo 3D a partir de um arquivo .obj em formato ASCII
 * (Wavefront OBJ). A função suporta v, vt, vn, f, mtllib, usemtl e ignora
 * outras linhas. A função também anexa uma normal calculada para cada face
 * que não tenha normal explícita, e remove alocações extras no final da
 * execução.
 */
bool load_obj(const char *filename, Model* model);

/**
 * @brief Desenha o modelo 3D representado por `model`
 * utilizando OpenGL 2.x.
 * 
 * @param model estrutura do modelo 3D a ser desenhado
 * 
 * @cond Requer OpenGL 2.x, GL_TEXTURE_2D e GL_NORMALIZE.
 * @cond assume model != NULL.
 */
void draw_model(const Model* model);

/**
 * Libera a memória alocada pelo modelo `model`.
 * @cond assume model != NULL.
 */
void free_model(Model* model);
